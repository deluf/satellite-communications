
#include <algorithm>

#include "PacketScheduler.h"

Define_Module(PacketScheduler);


void PacketScheduler::initialize()
{
    throughputSignal = registerSignal("throughput");

    cModule *satCom = getParentModule()->getParentModule();

    cModule *oracleModule = satCom->getSubmodule("oracle");
    oracle = check_and_cast<Oracle*>(oracleModule);

    satellite = satCom->getSubmodule("satellite");
    numTerminals = satCom->par("N").intValue();
    blocksPerFrame = par("blocksPerFrame").intValue();
    /*
     * The status of each terminal (id, current coding rate and queue) is stored in a
     *  vector of terminalStatus structs indexed by the id field (i.e. terminals[i].id = i).
     * This allows to update the coding rate of any terminal in O(1) whenever a new codingRateMessage arrives.
     *
     * In order to serve the terminals according to a maximum-coding-rate policy without losing the
     *  indexing property described before, a vector of pointers to those terminalStatus structs is used.
     * This way, we don't actually sort the terminalStatus structs, but the pointers to them.
     */

    terminals.resize(numTerminals);
    sortedTerminals.resize(numTerminals);

    for (int i = 0; i < numTerminals; i++)
    {
        terminals[i].id = i;
        sortedTerminals[i] = &terminals[i];
    }

    receivedCodingRates = 0;
    debugTotalBitsSent = 0;
}

void PacketScheduler::handleMessage(cMessage *msg)
{
    if (msg->isName("packet"))
    {
        Packet *packet = check_and_cast<Packet*>(msg);

        int terminalId = packet->getTerminalId();
        int byteLength = packet->getByteLength();

        terminals[terminalId].queue.insert(packet);

        EV_DEBUG << "[packetScheduler]> terminal " << terminalId << " received a new packet of size " << byteLength << endl;
    }

    /* Message received from the satellite */
    else
    {
        CodingRatePacket *codingRatePacket = check_and_cast<CodingRatePacket*>(msg);

        int terminalId = codingRatePacket->getTerminalId();
        CODING_RATE codingRate = codingRatePacket->getCodingRate();

        terminals[terminalId].codingRate = codingRate;
        receivedCodingRates++;

        EV_DEBUG << "[packetScheduler]> Acknowledged coding rate " << codingRateToString[codingRate]
                 << " for terminal "<< terminalId << ", "
                 << numTerminals - receivedCodingRates << " remaining" << endl;

        /* All of the coding rates have been received */
        if (receivedCodingRates == numTerminals)
        {
            receivedCodingRates = 0;

            std::sort(
                sortedTerminals.begin(), sortedTerminals.end(),
                [](const TerminalStatus *a, const TerminalStatus *b)
                { return a->codingRate > b->codingRate; }
            );

            EV_DEBUG << "[packetScheduler]> Sorted terminals:" << endl;
            for (TerminalStatus* &terminal: sortedTerminals)
            {
                EV_DEBUG << "[packetScheduler]> ID: " << terminal->id << ", CR: "
                        << codingRateToString[terminal->codingRate]
                        << ", Queue length: " << terminal->queue.getLength() << endl;
            }

            EV_DEBUG << "[packetScheduler]> Building the frame..." << endl;

            Frame *frame = buildFrame();

            long bitLength = frame->getBitLength();
            debugTotalBitsSent += bitLength;

            /*
             * The throughput statistic uses sumPerDuration(throughputSignal) as a source, and records the last value.
             * The sumPerDuration filter does the following:
             *  For each value, computes the sum of values received so far, divides it by the duration, and outputs the result.
             * By default, the duration is equal to the current simulation time,
             *  since the first emission happens at simulation time 0, the throughput would be infinite
             *  and since, in the reals system, the frame actually arrives to the terminals in at most 80ms,
             * then we manually fix the emission to happen 80ms after the current simulation time.
             */
            cTimestampedValue tmp(simTime() + SimTime(80, SIMTIME_MS), (intval_t)bitLength);
            emit(throughputSignal, &tmp);

            if (bitLength > 0)
            {
                sendDirect(frame, satellite, "in");
            }
            else
            {
                delete frame;
            }

            EV_DEBUG << "[packetScheduler]> Frame of size " << bitLength << " bits sent!" << endl;
        }

        delete codingRatePacket;
    }
}

int PacketScheduler::getBlockSizeForCodingRate(CODING_RATE codingRate)
{
    int blockSize;
    switch (codingRate) {
        case L3: blockSize = 904; break;
        case L2: blockSize = 1356; break;
        case L1: blockSize = 1808; break;
        case R:  blockSize = 2260; break;
        case H1: blockSize = 2712; break;
        case H2: blockSize = 3164; break;
        case H3: blockSize = 3616; break;
        default: throw cRuntimeError("Unknown coding rate");
    }

    return blockSize / blocksPerFrame;
}


void PacketScheduler::initBlock(Block* block, CODING_RATE codingRate)
{
    block->setCodingRate(codingRate);
    block->setMaxSize(getBlockSizeForCodingRate(codingRate));
}

Frame *PacketScheduler::buildFrame()
{
    Frame *frame = new Frame("frame");
    frame->setByteLength(0);

    frame->setBlocksArraySize(blocksPerFrame);
    for (int i = 0; i < blocksPerFrame; i++)
    {
        frame->getBlocksForUpdate(i).setUsedSize(0);
    }

    int currentBlockIndex = 0;
    for (TerminalStatus *terminal : sortedTerminals)
    {
        while (!terminal->queue.isEmpty() && currentBlockIndex < blocksPerFrame)
        {
            Block* block = &frame->getBlocksForUpdate(currentBlockIndex);

            /* Initialize the block if empty */
            if (block->getUsedSize() == 0)
            {
                initBlock(block, terminal->codingRate);
            }

            /* Block's CR must be <= Terminal's CR */
            if (block->getCodingRate() > terminal->codingRate) {
                EV_DEBUG << "[packetScheduler]> Block coding rate (" << block->getCodingRate()
                         << ") too high (Terminal's CR: " << terminal->codingRate
                         << "), moving to next block" << endl;
                currentBlockIndex++;
                continue;
            }

            Packet *packet = check_and_cast<Packet*>(terminal->queue.front());
            int remainingSize = packet->getByteLength();
            EV_DEBUG << "[packetScheduler]> Scheduling packet for terminal: " << packet->getTerminalId()
                     << " with size: " << packet->getByteLength() << " bytes" << endl;

            /* checking if the packet can be scheduled at all */
            int blocksLeft = blocksPerFrame - currentBlockIndex - 1;
            int availableSpace = block->getMaxSize() - block->getUsedSize();
            int totalAvailableSpace = availableSpace + blocksLeft * getBlockSizeForCodingRate(terminal->codingRate);

            if (remainingSize > totalAvailableSpace)
            {
                /* If the packet can't be scheduled in the frame due to its size, we move to the next terminal */
                EV_DEBUG << "[packetScheduler]> Packet (" << packet->getByteLength()
                         << " bytes) too large to fit in frame ("
                         << totalAvailableSpace
                         << " bytes left), moving to next terminal" << endl;
                break;
            }

            while (remainingSize > 0)
            {
                availableSpace = block->getMaxSize() - block->getUsedSize();
                if (remainingSize <= availableSpace)
                {
                    block->setUsedSize(block->getUsedSize() + remainingSize);
                    block->appendPackets(packet);
                    frame->addByteLength(remainingSize);
                    remainingSize = 0;
                    terminal->queue.pop();
                    oracle->registerPacket(terminal->id, currentBlockIndex, block->getPacketsArraySize() - 1);
                    drop(packet);
                }
                else
                {
                    /* Current block is full , we split the packet*/
                    Packet *packetSegment = packet->dup();
                    packetSegment->setByteLength(availableSpace);
                    block->setUsedSize(block->getMaxSize());
                    block->appendPackets(packetSegment);

                    oracle->registerPacket(terminal->id, currentBlockIndex, block->getPacketsArraySize() - 1, false);
                    drop(packetSegment);

                    frame->addByteLength(availableSpace);
                    remainingSize -= availableSpace;
                    packet->setByteLength(remainingSize);

                    /* Packet will be scheduled in the next block, must be initialized */
                    EV_DEBUG << "[packetScheduler]> Block " << currentBlockIndex
                             << " has been filled" << endl;
                    block = &frame->getBlocksForUpdate(++currentBlockIndex);
                    initBlock(block, terminal->codingRate);
                }
            }
        }

        if (currentBlockIndex == blocksPerFrame) break;
    }

    EV_DEBUG << "[packetScheduler]> Finished building frame with size: "
            << frame->getByteLength() << " bytes" << endl;
    return frame;
}


void PacketScheduler::finish()
{
    // TODO: boh a sto punto fare anche delete dei vari timer
    for (TerminalStatus& terminalStatus : terminals)
    {
        terminalStatus.queue.clear();
    }

    EV_DEBUG << "### Simulation finished ###" << endl
            << "\tTotal number of bits sent: " << debugTotalBitsSent << endl
            << "\tSimulation time: " << simTime() << " s" << endl
            << "\tExpected throughput: " << ((double)debugTotalBitsSent)/(simTime().dbl()) << " bps" << endl;
}
