
#include "PacketScheduler.h"

Define_Module(PacketScheduler);


void PacketScheduler::initialize()
{
    cModule *satCom = getParentModule()->getParentModule();
    cModule *oracleModule = satCom->getSubmodule("oracle");
    oracle = check_and_cast<Oracle*>(oracleModule);

    satellite = satCom->getSubmodule("satellite");
    terminalCount = satCom->par("terminalCount").intValue();
    blocksPerFrame = par("blocksPerFrame").intValue();
    communicationSlotDuration =
                    SimTime(satCom->par("communicationSlotDuration").doubleValue());

    /*
     * In order to measure the throughput accurately, the warmup period must be
     *  an integer multiple of the communication slot duration (which is not
     *  a problem given that the first should be way bigger than the second).
     */
    long warmupTimeMS = getSimulation()->getWarmupPeriod().inUnit(SIMTIME_MS);
    long communicationSlotDurationMS = communicationSlotDuration.inUnit(SIMTIME_MS);

    if (warmupTimeMS % communicationSlotDurationMS != 0)
    {
        throw cRuntimeError(this, "The warmup time should be an integer multiple of the communication slot duration");
    }

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
    instantaneousThroughputSignal = registerSignal("instantaneousThroughput");

    receivedCodingRateCount = 0;
    totalBitsSent = 0;
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
        maxCRScheduling();
    }

    delete codingRatePacket;
}

void PacketScheduler::maxCRScheduling()
{
    /* Terminals are sorted from highest (H3) to lowest (L3) coding rate */
    std::sort(
        sortedTerminals.begin(), sortedTerminals.end(),
        [](const TerminalDescriptor *a, const TerminalDescriptor *b)
        {
            return a->codingRate > b->codingRate;
        }
    );

#ifdef DEBUG_SCHEDULER
    EV_DEBUG << endl << "[packetScheduler]> Sorted terminals:" << endl;
    for (TerminalDescriptor* &terminal: sortedTerminals)
    {
        EV_DEBUG << "[packetScheduler]> \t\t{ id: " << terminal->id << ", codingRate: "
                << codingRateToString[terminal->codingRate] << ", queueLength: "
                << terminal->packetQueue.getLength() << " }" << endl;
    }
#endif

    Frame *frame = buildFrame();

    if (frame->getByteLength() > maxBytesInBlock(H3) * blocksPerFrame)
    {
        throw cRuntimeError(this, "The size of the current frame is greater than its maximum theoretical size");
    }

    if (simTime() > getSimulation()->getWarmupPeriod())
    {
        totalBitsSent += frame->getBitLength();

        /* Wrapping the emits in the if statement avoids dividing by zero in the first emit call */
        emit(throughputSignal, totalBitsSent / (simTime() - getSimulation()->getWarmupPeriod()).dbl());
        emit(instantaneousThroughputSignal, frame->getBitLength() / communicationSlotDuration.dbl());
    }

    sendDirect(frame, satellite, "in");

    EV_INFO << "[packetScheduler]> Sent a frame of size " << frame->getByteLength() << " bytes" << endl;
}

Frame *PacketScheduler::buildFrame()
{
    Frame *frame = new Frame("frame");

    frame->setByteLength(0);
    frame->setBlocksArraySize(blocksPerFrame);

    for (int i = 0; i < blocksPerFrame; i++)
    {
        frame->getBlocksForUpdate(i).setUsedBytes(0);
    }

    /* The index of the block that the scheduler is currently trying to fill */
    int currentBlockIndex = 0;

    for (TerminalDescriptor *terminal : sortedTerminals)
    {

#ifdef DEBUG_SCHEDULER
        EV_DEBUG << endl << "[packetScheduler]> Serving terminal { id: " << terminal->id
                << ", codingRate: " << codingRateToString[terminal->codingRate]
                << ", queueLength: " << terminal->packetQueue.getLength() << " }" << endl;

        if (terminal->packetQueue.isEmpty())
        {
            EV_DEBUG << "[packetScheduler]> \t\tTerminal's queue is empty. Moving to the next terminal" << endl;
        }
#endif

        /*
         * A packet can be scheduled in multiple blocks, if this variable is != -1 it
         *  means that there are still bytes left from the previous scheduled packet.
         */
        int lastPacketBytesLeft = -1;

        while (!terminal->packetQueue.isEmpty() && currentBlockIndex < blocksPerFrame)
        {
            /* There are still packets in queue and there are still free blocks in the frame */
            Block *block = &frame->getBlocksForUpdate(currentBlockIndex);

            if (block->getUsedBytes() == 0)
            {
                /* If the block is empty, initialize it with the coding rate of the current terminal */
                bool isForNewPacket = lastPacketBytesLeft == -1;
                initBlock(block, terminal->codingRate, isForNewPacket, currentBlockIndex);
            }
            else if (!canSchedule(terminal, block, currentBlockIndex))
            {
                /* If coding rate constraints are not met, move to the next block */
                currentBlockIndex++;
                continue;
            }

            Packet *packet = check_and_cast<Packet*>(terminal->packetQueue.front());

            /* If there are still bytes left from the previous packet, this check has already been made */
            if (lastPacketBytesLeft == -1)
            {
                /* If the packet can't be scheduled in the frame due to its size, move to the next terminal */
                if (!fits(packet, frame, currentBlockIndex))
                {
                    break;
                }
                lastPacketBytesLeft = packet->getByteLength();
            }

            /* Try to allocate the whole packet in the current block. If it does not fit, split it */
            allocatePacketSegment(packet, frame, lastPacketBytesLeft, currentBlockIndex);
        }

        /* There either is no space left, or no terminals can use it due to coding rate constraints */
        if (currentBlockIndex == blocksPerFrame)
        {
            break;
        }

    }

#ifdef DEBUG_SCHEDULER
    EV_DEBUG << endl << "[packetScheduler]> Scheduling process terminated" << endl << endl;
#endif

    return frame;
}

const int PacketScheduler::maxBytesInBlock(CODING_RATE codingRate) const
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
            throw cRuntimeError(this, "The specified coding rate does not have an associated block size");
    }

    /*
     * In case the result of the division is not an integer, it gets truncated.
     *  e.g. blocksPerFrame = 5, codingRate = R => maxBytes = floor(2712 / 5) = floor(542.4) = 542
     */
    return blockSize / blocksPerFrame;
}

void PacketScheduler::initBlock(Block *block, CODING_RATE codingRate, bool isForNewPacket, int currentBlockIndex)
{
    int maxBytes = maxBytesInBlock(codingRate);

    block->setCodingRate(codingRate);
    block->setMaxBytes(maxBytes);

    #ifdef DEBUG_SCHEDULER
    /*
     * If the initialized block is for a new packet (not a segment of a
     *  previous packet), then print the full description of the block.
     */
    if (isForNewPacket)
    {
        EV_DEBUG << "[packetScheduler]> \t\tAllocating a new block { index: " << currentBlockIndex
                << ", codingRate: " << codingRateToString[block->getCodingRate()]
                << ", usedBytes: 0, maxBytes: " << block->getMaxBytes() << "}" << endl;
    }
    #endif
}

bool PacketScheduler::canSchedule(TerminalDescriptor *terminal, Block *block, int currentBlockIndex)
{
#ifdef DEBUG_SCHEDULER
    EV_DEBUG << "[packetScheduler]> \t\tConsidering the existing block { index: " << currentBlockIndex
            << ", codingRate: " << codingRateToString[block->getCodingRate()] << ", usedBytes: "
            << block->getUsedBytes() << ", maxBytes: " << block->getMaxBytes() << "}" << endl;
#endif

    /*
     * If instead it already has been initialized, then coding rate constraints must be checked:
     *  A terminal with coding rate T can only be scheduled in blocks with coding rate B <= T.
     *
     * i.e., a terminal with coding rate H3 can be scheduled anywhere, while a terminal
     *  with coding rate R can only be scheduled in R, L1, L2 or L3 blocks.
     */
    if (terminal->codingRate < block->getCodingRate())
    {
#ifdef DEBUG_SCHEDULER
        EV_DEBUG << "[packetScheduler]> \t\tThe coding rate of block " << currentBlockIndex << " ("
                << codingRateToString[block->getCodingRate()] << ") is too high for terminal "
                << terminal->id << " (" << codingRateToString[terminal->codingRate]
                << "), moving to the next block" << endl;
#endif

        return false;
    }
    return true;
}

bool PacketScheduler::fits(Packet *packet, Frame *frame, int currentBlockIndex)
{
    /*
     * A new packet is being considered, let's see if it fits in the frame.
     *
     * A packet can be scheduled if and only if it can be entirely splitted
     *  between the current block and the remaining free ones (which, in
     *  case, will assume the coding rate of the terminal).
     */

    Block *block = &frame->getBlocksForUpdate(currentBlockIndex);
    CODING_RATE codingRate = terminals[packet->getTerminalId()].codingRate;

    int packetBytesLeft = packet->getByteLength();
    int freeBlocks = blocksPerFrame - 1 - currentBlockIndex;
    int availableBytesInCurrentBlock = block->getMaxBytes() - block->getUsedBytes();
    int availableBytesInFrame = availableBytesInCurrentBlock
            + freeBlocks * maxBytesInBlock(codingRate);

    if (packetBytesLeft > availableBytesInFrame)
    {
#ifdef DEBUG_SCHEDULER
        EV_DEBUG << "[packetScheduler]> \t\tPacket { id: " << packet->getTreeId() << ", byteLength: "
                << packet->getByteLength() << " } is too large to fit in the frame (only " << availableBytesInFrame
                << " bytes left), moving to the next terminal" << endl;
#endif
        return false;
    }

#ifdef DEBUG_SCHEDULER
    EV_DEBUG << "[packetScheduler]> \t\tPacket { id: " << packet->getTreeId() << ", byteLength: "
            << packet->getByteLength() << " } fits in the frame, splitting it among the remaining blocks:"<< endl;
#endif

    return true;
}

void PacketScheduler::allocatePacketSegment(Packet *packet, Frame *frame, int &lastPacketBytesLeft, int &currentBlockIndex)
{
    TerminalDescriptor *terminal = &terminals[packet->getTerminalId()];
    Block *block = &frame->getBlocksForUpdate(currentBlockIndex);
    int availableBytesInCurrentBlock = block->getMaxBytes() - block->getUsedBytes();

    /* Whatever is left of the packet fits in the current block */
    if (lastPacketBytesLeft <= availableBytesInCurrentBlock)
    {
        block->setUsedBytes(block->getUsedBytes() + lastPacketBytesLeft);
        block->appendPackets(packet);
        frame->addByteLength(lastPacketBytesLeft);
        terminal->packetQueue.pop();

#ifdef DEBUG_SCHEDULER
        EV_DEBUG << "[packetScheduler]> \t\t\t - Allocated the last (or only) segment of "
               << lastPacketBytesLeft << " bytes in block " << currentBlockIndex << endl;
#endif

        oracle->registerPacket(terminal->id, currentBlockIndex, block->getPacketsArraySize() - 1);

        lastPacketBytesLeft = -1;
        return;
    }

    /*
     * The packet must be segmented in multiple blocks.
     * This is achieved by duplicating the same packet but with different sizes.
     */
    Packet *packetSegment = packet->dup();
    packetSegment->setByteLength(availableBytesInCurrentBlock);

    block->setUsedBytes(block->getMaxBytes());
    block->appendPackets(packetSegment);
    frame->addByteLength(availableBytesInCurrentBlock);

#ifdef DEBUG_SCHEDULER
    EV_DEBUG << "[packetScheduler]> \t\t\t - Allocated a segment of " << availableBytesInCurrentBlock
       << " bytes in block " << currentBlockIndex << ", which is now full, allocating a new block" << endl;
#endif

    oracle->registerPacket(terminal->id, currentBlockIndex, block->getPacketsArraySize() - 1, false);

    lastPacketBytesLeft -= availableBytesInCurrentBlock;
    packet->setByteLength(lastPacketBytesLeft);
    currentBlockIndex++;
}

void PacketScheduler::finish()
{
    /* In order to avoid annoying "undisposed object" messages (that could also hide a memory leak) we must: */

    /* 1. Deallocate all the packets still in the queues */
    for (TerminalDescriptor &terminal : terminals)
    {
        terminal.packetQueue.clear();
    }

    /* 2. Deallocate all the packets embedded in ongoing frames (if any) */
    cFutureEventSet* FES = getSimulation()->getFES();
    int numEvents = FES->getLength();
    for (int i = 0; i < numEvents; i++)
    {
        cEvent *event = FES->get(i);

        if(!event->isName("frame"))
        {
            continue;
        }

        Frame *frame = check_and_cast<Frame *>(event);

        /* Iterating through all blocks to delete all packets */
        int numBlocks = frame->getBlocksArraySize();
        for (int i = 0; i < numBlocks; i++)
        {
            Block *block = &frame->getBlocksForUpdate(i);
            int numPackets = block->getPacketsArraySize();
            for (int j = 0; j < numPackets; j++)
            {
                Packet *packet = block->getPacketsForUpdate(j);
                if (packet != nullptr)
                {
                    delete packet;
                    block->setPackets(j, nullptr);
                }
            }
        }

        /* All the frames contain the same list of pointers, which must only be freed once */
        break;
    }

}
