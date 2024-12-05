
#ifndef PACKETGENERATOR_H_
#define PACKETGENERATOR_H_

#include "omnetpp.h"

using namespace omnetpp;

class PacketGenerator : public cSimpleModule
{
    cMessage *timer;
    int terminalId;
    int minS;
    int maxS;
    double meanT;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* PACKETGENERATOR_H_ */
