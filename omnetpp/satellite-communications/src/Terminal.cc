
#include "Terminal.h"

Define_Module(Terminal);


void Terminal::initialize()
{
    cModule *satCom = getParentModule();

    satellite = satCom->getSubmodule("satellite");
    delaySignal = registerSignal("delay");

    id = getIndex();
    communicationSlotDuration =
            SimTime(satCom->par("communicationSlotDuration").doubleValue());

    cModule *oracleModule = satCom->getSubmodule("oracle");
    oracle = check_and_cast<Oracle*>(oracleModule);

    /* A timer tick represents the start of a new communication slot */
    timer = new cMessage("timer");
    scheduleAt(simTime(), timer);
}

void Terminal::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        handleTimer();
    }
    else if (msg->isName("frame"))
    {
        Frame *frame = check_and_cast<Frame*>(msg);
        handleFrame(frame);
    }
    else
    {
        throw cRuntimeError(this, "The terminal can't handle the received "
                "message. Supported types are: \"timer\" (selfMessage), \"frame\"");
    }
}

void Terminal::handleTimer()
{
    /* Each communication slot begins with the terminals sending their coding rates to the ground station */
    CODING_RATE codingRate = (CODING_RATE)par("codingRate").intValue();

#ifdef DEBUG_RNGS
    EV_DEBUG << std::fixed << std::setprecision(5) << simTime().dbl() << " - [terminal " << id
            << "]> Extracted codingRate | Total RNG calls: " << getRNG(0)->getNumbersDrawn() << endl;
#endif

    CodingRatePacket *codingRatePacket = new CodingRatePacket("codingRatePacket");
    codingRatePacket->setTerminalId(id);
    codingRatePacket->setCodingRate(codingRate);
    sendDirect(codingRatePacket, satellite, "in");

    EV_INFO << "[terminal " << id << "]> For the communication slot [" << simTime().dbl() << " - "
            << (simTime() + communicationSlotDuration).dbl() << "]s my coding rate will be "
            << codingRateToString[codingRate] << endl;

    /* Schedule the next communication slot */
    scheduleAt(simTime() + communicationSlotDuration, timer);
}

void Terminal::handleFrame(Frame *frame)
{
    /* When a frame is received, each terminal asks the oracle where to look (in the frame) to read their messages */
    std::list<PacketLocation> &packetLocations = oracle->getPacketLocations(id);

    if (packetLocations.empty())
    {
        EV_INFO << "[terminal " << id << "]> This frame does not carry any packets for me" << endl;
    }

    while (!packetLocations.empty())
    {
        PacketLocation &packetLocation = packetLocations.front();

        int blockIndex = packetLocation.blockIndex;
        int packetIndex = packetLocation.packetIndex;

        Block *block = &frame->getBlocksForUpdate(blockIndex);
        Packet *packet = block->getPacketsForUpdate(packetIndex);

#ifdef DEBUG_TERMINALS
        EV_DEBUG << "[terminal " << id << "]> Fetched { blockIndex: " << packetLocation.blockIndex
                << ", packetIndex: " << packetLocation.packetIndex << ", lastPiece: "
                << packetLocation.isLastSegment << "}" << endl;
#endif

        if (packet->getTerminalId() != id)
        {
            throw cRuntimeError(this, "A terminal was allowed to read a packet destined to another terminal");
        }

        /* The delay statistic of a packet can only be emitted when the packet has been fully fetched */
        if (packetLocation.isLastSegment)
        {
            double delay = (simTime() - packet->getCreationTime()).dbl();
            emit(delaySignal, delay);
            EV_INFO << "[terminal " << id << "]> Received packet { id: " << packet->getTreeId() << ", byteLength: "
                    << packet->getByteLength() << " } with a delay of " << delay * 1e3 << " ms" << endl;
        }

        /*
         * Each terminal is responsible for deallocating its own packets.
         * In order to do that, ownership of such packets must be claimed.
         * This is because packets are not sent with a traditional "send()" function,
         *  but carried as objects inside another message (the frame).
         * Of course, the sender must collaborate and drop() the ownership
         *  of the packets before appending them to the frame.
         */
        take(packet);
        delete packet;

        packetLocations.pop_front();
    }

    /* Each terminal receives a copy of the same frame, and is responsible for its deallocation */
    delete frame;
}

void Terminal::finish()
{
    cancelAndDelete(timer);
}
