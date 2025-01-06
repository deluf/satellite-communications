
#ifndef PACKETGENERATOR_H_
#define PACKETGENERATOR_H_

#include <iomanip>

#include "omnetpp.h"
#include "Packet_m.h"

using namespace omnetpp;

class PacketGenerator : public cSimpleModule
{
    /* Maximum allowed size, a packet exceeding this value will NEVER be scheduled
     * as it exceeds the maximum possible frame size (obtained when all blocks are H3)
     */
    static const int MAX_ALLOWED_SIZE = 3616;

    cMessage *timer;
    int id;
    int minPacketSize;
    int maxPacketSize;
    double meanPacketInterarrivalTime;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* PACKETGENERATOR_H_ */
