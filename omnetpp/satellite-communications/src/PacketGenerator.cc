
#include "PacketGenerator.h"

Define_Module(PacketGenerator);


void PacketGenerator::initialize()
{
    meanPacketInterarrivalTime = par("meanPacketInterarrivalTime").doubleValue();
    if (meanPacketInterarrivalTime <= 0.0)
        throw cRuntimeError("meanPacketInterarrivalTime (%f) must be a positive value.", meanPacketInterarrivalTime);

    minPacketSize = par("minPacketSize").intValue();
    maxPacketSize = par("maxPacketSize").intValue();

    if (minPacketSize <= 0)
        throw cRuntimeError("minPacketSize (%d) must be a positive integer.", minPacketSize);
    if (minPacketSize > maxPacketSize)
        throw cRuntimeError("minPacketSize (%d) cannot be greater than maxPacketSize (%d).", minPacketSize, maxPacketSize);
    if (maxPacketSize > MAX_ALLOWED_SIZE)
        throw cRuntimeError("maxPacketSize (%d) must be less than or equal to MAX_ALLOWED_SIZE (%d).", maxPacketSize, MAX_ALLOWED_SIZE);

    id = getIndex();

    /* A timer tick represents the arrival of a packet from the external world */
    timer = new cMessage("timer");
    double nextArrivalTime = exponential(meanPacketInterarrivalTime);
    scheduleAt(simTime() + SimTime(nextArrivalTime), timer);

#ifdef DEBUG_RNGS
    EV_DEBUG << std::fixed << std::setprecision(5) << simTime().dbl() << " - [packetGenerator " << id
            << "]> Extracted nextArrivalTime for terminal " << id << " | Total RNG calls: " << getRNG(0)->getNumbersDrawn() << endl;
#endif

    EV_INFO << "[packetGenerator " << id << "]> The first packet is scheduled to arrive in "
            << nextArrivalTime * 1e3 << " ms" << endl;
}

void PacketGenerator::handleMessage(cMessage *msg)
{
    /*
     * Received messages are self messages (timers) by design (there are no input ports).
     * When a timer ticks, a new packet gets created and sent to the scheduler.
     */

    int byteLength = intuniform(minPacketSize, maxPacketSize);

    Packet *packet = new Packet("packet");
    packet->setTerminalId(id);
    packet->setByteLength(byteLength);
    send(packet, "out");

#ifdef DEBUG_RNGS
    EV_DEBUG << std::fixed << std::setprecision(5) << simTime().dbl() << " - [packetGenerator " << id
            << "]> Extracted byteLength for terminal " << id << " | Total RNG calls: " << getRNG(0)->getNumbersDrawn() << endl;
#endif

    EV_INFO << "[packetGenerator " << id << "]> Generated packet { id: "
            << packet->getTreeId() << ", byteLength: " << byteLength << " }" << endl;

    double nextArrivalTime = exponential(meanPacketInterarrivalTime);
    scheduleAt(simTime() + SimTime(nextArrivalTime), timer);

#ifdef DEBUG_RNGS
    EV_DEBUG << std::fixed << std::setprecision(5) << simTime().dbl() << " - [packetGenerator " << id
            << "]> Extracted nextArrivalTime for terminal " << id << " | Total RNG calls: " << getRNG(0)->getNumbersDrawn() << endl;
#endif

    EV_INFO << "[packetGenerator " << id << "]> The next packet is scheduled to arrive in "
            << nextArrivalTime * 1e3 << " ms" << endl;
}

void PacketGenerator::finish()
{
    cancelAndDelete(timer);
}
