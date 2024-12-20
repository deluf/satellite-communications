
#include "PacketScheduler.h"

Define_Module(PacketScheduler);


void PacketScheduler::initialize()
{
    cModule *satCom = getParentModule()->getParentModule();

    cModule *oracleModule = satCom->getSubmodule("oracle");
    oracle = check_and_cast<Oracle*>(oracleModule);

    satellite = satCom->getSubmodule("satellite");
    terminalCount = satCom->par("terminalCount").intValue();
    communicationSlotDuration =
                SimTime(satCom->par("communicationSlotDuration").doubleValue());
    blocksPerFrame = par("blocksPerFrame").intValue();

    /*
     * All the informations about each terminal { id, codingRate, packetQueue } are stored in a
     *  vector of terminalDescriptor structs indexed by the id field (i.e. terminals[i].id = i).
     *
     * This property allows to update the coding rate of any terminal
     *  in O(1) whenever a new codingRatePacket arrives.
     *
     * In order to serve the terminals according to a maximum-coding-rate policy without losing the
     *  indexing property described before, a vector of pointers to terminalDescriptors is used.
     * This way, we don't actually sort the terminalDescriptors structs, but the pointers to them.
     */

    terminals.resize(terminalCount);
    sortedTerminals.resize(terminalCount);

    for (int i = 0; i < terminalCount; i++)
    {
        terminals[i].id = i;
        sortedTerminals[i] = &terminals[i];
    }

    throughputSignal = registerSignal("throughput");
    receivedCodingRateCount = 0;

#ifdef DEBUG_SCHEDULER
    totalBitsSent = 0;
#endif

}

void PacketScheduler::handleMessage(cMessage *msg)
{
    if (msg->isName("packet"))
    {
        Packet *packet = check_and_cast<Packet*>(msg);
        handlePacket(packet);
    }
    else if (msg->isName("codingRatePacket"))
    {
        CodingRatePacket *codingRatePacket = check_and_cast<CodingRatePacket*>(msg);
        handleCodingRatePacket(codingRatePacket);
    }
    else
    {
        throw cRuntimeError(this, "The packetScheduler can't handle the received "
                "message. Supported types are: \"codingRatePacket\", \"packet\"");
    }
}

void PacketScheduler::handlePacket(Packet *packet)
{
    /* When a packet arrives from a packetGenerator, it is inserted into the appropriate packetQueue */

    int terminalId = packet->getTerminalId();
    terminals[terminalId].packetQueue.insert(packet);

    EV_INFO << "[packetScheduler]> Added packet with id " << packet->getTreeId()
            << " to the queue of terminal " << terminalId << ". The queue now contains "
            << terminals[terminalId].packetQueue.getLength() << " packets" << endl;
}

void PacketScheduler::handleCodingRatePacket(CodingRatePacket *codingRatePacket)
{
    int terminalId = codingRatePacket->getTerminalId();
    CODING_RATE codingRate = codingRatePacket->getCodingRate();

    terminals[terminalId].codingRate = codingRate;
    receivedCodingRateCount++;

    EV_INFO << "[packetScheduler]> Received coding rate " << codingRateToString[codingRate]
             << " for terminal "<< terminalId << ", "
             << terminalCount - receivedCodingRateCount << " remaining" << endl;

    /* The scheduling process can begin when all of the coding rates have been received */
    if (receivedCodingRateCount == terminalCount)
    {
        receivedCodingRateCount = 0;
        schedulePackets();
    }

    delete codingRatePacket;
}

void PacketScheduler::schedulePackets()
{
    std::sort(
        sortedTerminals.begin(), sortedTerminals.end(),
        [](const TerminalDescriptor *a, const TerminalDescriptor *b)
        {
            return a->codingRate > b->codingRate;
        }
    );

#ifdef DEBUG_SCHEDULER
    EV_DEBUG << "[packetScheduler]> Sorted terminals:" << endl;
    for (TerminalDescriptor* &terminal: sortedTerminals)
    {
        EV_DEBUG << "[packetScheduler]> ID: " << terminal->id << ", CR: "
                << codingRateToString[terminal->codingRate]
                << ", Queue length: " << terminal->packetQueue.getLength() << endl;
    }
#endif

    Frame *frame = buildFrame();

#ifdef DEBUG_SCHEDULER
    totalBitsSent += frame->getBitLength();
#endif

    /*
     * The throughput statistic uses sumPerDuration(throughputSignal) as a source, and records its last value.
     *
     * The sumPerDuration filter does the following:
     *  1. Adds the emitted value to to the sum of all the values emitted until then
     *  2. Divides that number by the "duration" of the filter
     *  3. Outputs the result of the last operation
     * By default, the "duration" of the filter is equal to the current simulation time.
     *
     * This means that since the first emission happens at simulation time 0, without
     *  particular precautions, the first recorded throughput would be infinite.
     *
     * Since, in the reals system, the frame actually arrives to the terminals within a communication slot,
     *  then it is fair to emit the throughputSignal a communication slot after the current simulation time.
     */
    cTimestampedValue scheduledEmit(simTime() + communicationSlotDuration, frame->getBitLength());
    emit(throughputSignal, &scheduledEmit);

    sendDirect(frame, satellite, "in");

    EV_INFO << "[packetScheduler]> Sent a frame of size " << frame->getByteLength() << " bytes" << endl;
}

const int PacketScheduler::getBlockSizeForCodingRate(CODING_RATE codingRate) const
{
    int blockSize;
    switch (codingRate)
    {
        case L3:
            blockSize = 904;
            break;
        case L2:
            blockSize = 1356;
            break;
        case L1:
            blockSize = 1808;
            break;
        case R:
            blockSize = 2260;
            break;
        case H1:
            blockSize = 2712;
            break;
        case H2:
            blockSize = 3164;
            break;
        case H3:
            blockSize = 3616;
            break;
        default:
            throw cRuntimeError("Unknown coding rate");
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

    /* Serve each terminal according to a maximum-coding-rate policy */
    for (TerminalDescriptor *terminal : sortedTerminals)
    {

        /* While there are still packets in queue and there are still free blocks... */
        while (!terminal->packetQueue.isEmpty() && currentBlockIndex < blocksPerFrame)
        {
            Block* block = &frame->getBlocksForUpdate(currentBlockIndex);

            /* Initialize the block if empty */
            if (block->getUsedSize() == 0)
            {
                initBlock(block, terminal->codingRate);
            }

            /*
             * A terminal with coding rate T can only be scheduled in blocks with coding rate B <= T.
             * i.e., a terminal with coding rate H3 can be scheduled anywhere,
             *  while a terminal with coding rate R can only be scheduled in R, L1, L2, L3 blocks
             */
            if (terminal->codingRate < block->getCodingRate()) {
#ifdef DEBUG_SCHEDULER
                EV_DEBUG << "[packetScheduler]> Block coding rate (" << block->getCodingRate()
                         << ") too high (Terminal's CR: " << terminal->codingRate
                         << "), moving to next block" << endl;
#endif
                currentBlockIndex++;
                continue;
            }

            Packet *packet = check_and_cast<Packet*>(terminal->packetQueue.front());
            int remainingSize = packet->getByteLength();

#ifdef DEBUG_SCHEDULER
            EV_DEBUG << "[packetScheduler]> Scheduling packet for terminal: " << packet->getTerminalId()
                     << " with size: " << packet->getByteLength() << " bytes" << endl;
#endif

            /* checking if the packet can be scheduled at all */
            int blocksLeft = blocksPerFrame - currentBlockIndex - 1;
            int availableSpace = block->getMaxSize() - block->getUsedSize();
            int totalAvailableSpace = availableSpace + blocksLeft * getBlockSizeForCodingRate(terminal->codingRate);

            if (remainingSize > totalAvailableSpace)
            {
                /* If the packet can't be scheduled in the frame due to its size, we move to the next terminal */
#ifdef DEBUG_SCHEDULER
                EV_DEBUG << "[packetScheduler]> Packet (" << packet->getByteLength()
                         << " bytes) too large to fit in frame ("
                         << totalAvailableSpace
                         << " bytes left), moving to next terminal" << endl;
#endif
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
                    terminal->packetQueue.pop();
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
#ifdef DEBUG_SCHEDULER
                    EV_DEBUG << "[packetScheduler]> Block " << currentBlockIndex
                             << " has been filled" << endl;
#endif

                    block = &frame->getBlocksForUpdate(++currentBlockIndex);
                    initBlock(block, terminal->codingRate);
                }
            }
        }

    }

    return frame;
}

void PacketScheduler::finish()
{
    /*
     * Deallocate all the packets still in the queues.
     * Not really necessary but avoids the annoying "undisposed object" messages.
     */
    for (TerminalDescriptor& terminal : terminals)
    {
        terminal.packetQueue.clear();
    }

#ifdef DEBUG_SCHEDULER
    /* Print the expected throughput */
    EV_DEBUG << "[packetScheduler]> " << endl
            << "\tTotal bits sent: " << totalBitsSent << endl
            << "\tSimulation time: " << simTime().dbl() << " s" << endl
            << "\tExpected throughput: " << (double)totalBitsSent / simTime().dbl() << " bps" << endl;
#endif
}
