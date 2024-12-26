
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
     * Since, in the real systems, the frame actually arrives to the terminals within a communication slot,
     *  then it is fair to emit the throughputSignal a communication slot after the current simulation time.
     */
    cTimestampedValue scheduledEmit(simTime() + communicationSlotDuration, frame->getBitLength());
    emit(throughputSignal, &scheduledEmit);

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

const int PacketScheduler::getMaxBlockBytes(CODING_RATE codingRate) const
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
    return blockSize / blocksPerFrame;
}

void PacketScheduler::initBlock(Block *block, CODING_RATE codingRate, bool isForNewPacket, int currentBlockIndex)
{
    int maxBytes = getMaxBlockBytes(codingRate);

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
            + freeBlocks * getMaxBlockBytes(codingRate);

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

        /*
        * Each terminal is responsible for deallocating its own packets.
        * In order to do that, ownership of such packets must be claimed.
        * This is because packets are not sent with a traditional "send()" function,
        *  but carried as objects inside another message (the frame).
        * Of course, the sender must collaborate and drop() the ownership
        *  of the packets before appending them to the frame.
        */
        drop(packet);

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

    drop(packetSegment);

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
    /*
     * Deallocate all the packets still in the queues.
     * Not really necessary but avoids the annoying "undisposed object" messages.
     */
    for (TerminalDescriptor &terminal : terminals)
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
