
#include <iomanip>

#include "PacketGenerator.h"
#include "Packet_m.h"

Define_Module(PacketGenerator);


void PacketGenerator::initialize()
{
    meanPacketInterarrivalTime = par("meanPacketInterarrivalTime").doubleValue();
    minPacketSize = par("minPacketSize").intValue();
    maxPacketSize = par("maxPacketSize").intValue();
    id = getIndex();

    timer = new cMessage("timer");
    double nextArrivalTime = exponential(meanPacketInterarrivalTime);
    scheduleAt(simTime() + SimTime(nextArrivalTime), timer);

#ifdef RNG_DEBUG
    EV_DEBUG << std::fixed << std::setprecision(5) << simTime().dbl() << " - [packetGenerator " << id << "]> Extracted T for terminal " << id << " | Total: " << getRNG(0)->getNumbersDrawn() << endl;
#endif

    EV_DEBUG << "[packetGenerator " << id << "]> the first packet is scheduled to arrive in "
            << nextArrivalTime * 1e3 << " ms" << endl;
}

void PacketGenerator::handleMessage(cMessage *msg)
{
    /*
     * Received messages are always self messages (timers) by design (there are no input ports).
     */

    int byteLength = intuniform(minPacketSize, maxPacketSize);

    Packet *packet = new Packet("packet");
    packet->setTerminalId(id);
    packet->setByteLength(byteLength);
    send(packet, "out");

    // EV_INFO << std::fixed << std::setprecision(5) << simTime().dbl() << " - [packetGenerator " << getIndex() << "]> Extracted S for terminal " << getIndex() << " | Total: " << getRNG(0)->getNumbersDrawn() << endl;
    EV_DEBUG <<"[packetGenerator " << id << "]> Generated packet with id: " << packet->getTreeId() << endl;

    double nextArrivalTime = exponential(meanPacketInterarrivalTime);
    scheduleAt(simTime() + SimTime(nextArrivalTime), timer);

    // EV_INFO << std::fixed << std::setprecision(5) << simTime().dbl() << " - [packetGenerator " << getIndex() << "]> Extracted T for terminal " << getIndex() << " | Total: " << getRNG(0)->getNumbersDrawn() << endl;

    EV_DEBUG << "[packetGenerator " << id << "]> the next packet is scheduled to arrive in "
            << nextArrivalTime * 1e3 << " ms" << endl;
}

void PacketGenerator::finish()
{
    cancelAndDelete(timer);
}
