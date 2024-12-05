
#ifndef PACKETSCHEDULER_H_
#define PACKETSCHEDULER_H_

#include <vector>

#include "omnetpp.h"

using namespace omnetpp;

struct TerminalStatus
{
    int id;
    int codingRate;
    cQueue queue;
};

class PacketScheduler : public cSimpleModule
{
    cModule *satellite;
    simsignal_t throughputSignal;
    std::vector<TerminalStatus> terminals;
    std::vector<TerminalStatus*> sortedTerminals;
    int numTerminals;
    int receivedCodingRates;
    long debugTotalBitsSent;    // TODO: Just for debugging

    cMessage *buildFrame();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* PACKETSCHEDULER_H_ */
