
#include "Oracle.h"

Define_Module(Oracle);


void Oracle::initialize()
{
    int N = getParentModule()->par("N");
    packetLocations.resize(N);
}

void Oracle::registerPacket(int terminalId, int blockIndex, int packetIndex, bool isLastSegment)
{
    /*
     * TODO: Maybe do some checks on the inputs (not because they need to be sanitized,
     *  but because it may be useful in debugging) (and dr. Nardini may like it lol)
     */

    EV_DEBUG << "[oracle]> Registered packet { blockIndex: " << blockIndex << ", packetIndex: "
        << packetIndex << ", isLastSegment: " << isLastSegment << "} for terminal " << terminalId << endl;

    packetLocations[terminalId].push_back(
        { blockIndex, packetIndex, isLastSegment }
    );
}

std::list<PacketLocation>& Oracle::getPacketLocations(int terminalId)
{
    return packetLocations[terminalId];
}
