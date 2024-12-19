
#include <iomanip>

#include "Terminal.h"
#include "CodingRatePacket_m.h"
#include "Frame_m.h"

Define_Module(Terminal);


void Terminal::initialize()
{
    cModule *satCom = getParentModule();

    cModule *oracleModule = satCom->getSubmodule("oracle");
    oracle = check_and_cast<Oracle*>(oracleModule);

    satellite = satCom->getSubmodule("satellite");

    delaySignal = registerSignal("delay");
    id = getIndex();
    timer = new cMessage("timer");
    scheduleAt(simTime(), timer);
}

void Terminal::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        /* Schedule the next timer */
        scheduleAt(simTime() + SimTime(80, SIMTIME_MS), timer);

        /* Send the coding rate to the ground station */
        CODING_RATE codingRate = (CODING_RATE)par("codingRate").intValue();

        // TODO: Check if i can do something like #ifdef DEBUG_RNGS then ...
        // EV_INFO << std::fixed << std::setprecision(5) << simTime().dbl() << " - [terminal " << getIndex() << "]> Extracted CR | Total: " << getRNG(0)->getNumbersDrawn() << endl;

        CodingRatePacket *codingRatePacket = new CodingRatePacket("codingRate");
        codingRatePacket->setTerminalId(id);
        codingRatePacket->setCodingRate(codingRate);

        sendDirect(codingRatePacket, satellite, "in");

        EV_DEBUG << "[terminal " << id << "]> My coding rate is now "
                << codingRateToString[codingRate] << endl;
    }
    else
    {
        Frame *frame = check_and_cast<Frame*>(msg);

        std::list<PacketLocation> &packetLocations = oracle->getPacketLocations(id);

        while (!packetLocations.empty())
        {
            PacketLocation &packetLocation = packetLocations.front();

            int blockIndex = packetLocation.blockIndex;
            int packetIndex = packetLocation.packetIndex;

            Block *block = &frame->getBlocksForUpdate(blockIndex);
            Packet *packet = block->getPacketsForUpdate(packetIndex);

            EV_DEBUG << "[terminal " << id << "]> Fetched { blockIndex: " << packetLocation.blockIndex
                    << ", packetIndex: " << packetLocation.packetIndex << ", lastPiece: "
                    << packetLocation.isLastSegment << "}" << endl;

            // TOOD: if packet->terminalId != id error or something

            if (packetLocation.isLastSegment) {
                double delay = (simTime() - packet->getCreationTime()).dbl();
                emit(delaySignal, delay);
                EV_DEBUG << "[terminal " << id << "]> Received packet "
                        << packet->getTreeId() << " with a delay of " << delay * 1e3 << " ms" << endl;
            }

            take(packet);
            delete packet;

            packetLocations.pop_front();
        }

        delete frame;
    }

}

void Terminal::finish()
{
    cancelAndDelete(timer);
}
