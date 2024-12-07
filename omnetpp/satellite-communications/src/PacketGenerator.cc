
#include <iomanip>

#include "PacketGenerator.h"
#include "terminalPacket_m.h"

Define_Module(PacketGenerator);


void PacketGenerator::initialize()
{
    terminalId = getIndex();
    meanT = par("meanT").doubleValue();
    minS = par("minS").intValue();
    maxS = par("maxS").intValue();
    timer = new cMessage("timer");
    double nextArrivalTime = exponential(meanT);
    scheduleAt(simTime() + SimTime(nextArrivalTime), timer);

    // EV_INFO << std::fixed << std::setprecision(5) << simTime().dbl() << " - [packetGenerator " << terminalId << "]> Extracted T for terminal " << terminalId << " | Total: " << getRNG(0)->getNumbersDrawn() << endl;

    EV_DEBUG << "[packetGenerator " << terminalId << "]> the first packet for terminal " << terminalId
            << " is scheduled to arrive in " << nextArrivalTime * 1e3 << " ms" << endl;
}

void PacketGenerator::handleMessage(cMessage *msg)
{
    if (!msg->isSelfMessage())
    {
        // TODO: Error
    }

    int byteLength = intuniform(minS, maxS);

    // EV_INFO << std::fixed << std::setprecision(5) << simTime().dbl() << " - [packetGenerator " << getIndex() << "]> Extracted S for terminal " << getIndex() << " | Total: " << getRNG(0)->getNumbersDrawn() << endl;
    EV_DEBUG <<"[packetGenerator " << terminalId << "]> Generated packet id: " << packetId << endl;
    TerminalPacket *terminalPacket = new TerminalPacket("terminalPacket");
    terminalPacket->setTerminalId(terminalId);
    terminalPacket->setPacketId(packetId++);
    terminalPacket->setByteLength(byteLength);
    send(terminalPacket, "out");

    double nextArrivalTime = exponential(meanT);
    scheduleAt(simTime() + SimTime(nextArrivalTime), timer);

    // EV_INFO << std::fixed << std::setprecision(5) << simTime().dbl() << " - [packetGenerator " << getIndex() << "]> Extracted T for terminal " << getIndex() << " | Total: " << getRNG(0)->getNumbersDrawn() << endl;

    EV_DEBUG << "[packetGenerator " << terminalId << "]> the next packet for terminal " << terminalId <<
                    " is scheduled to arrive in " << nextArrivalTime * 1e3 << " ms" << endl;
}

void PacketGenerator::finish()
{

}
