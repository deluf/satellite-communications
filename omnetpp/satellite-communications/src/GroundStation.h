
#ifndef GROUNDSTATION_H_
#define GROUNDSTATION_H_

#include "omnetpp.h"

using namespace omnetpp;

struct TerminalInfo {
    int id;
    int crValue;
    cQueue *queue;
};

class GroundStation : public cSimpleModule
{
    int numTerminals;
    int acknowledgedCodingRates;
    std::vector<int> crValues;
    std::vector<cQueue> terminalQueues;
    cModule *satellite;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    static bool maxCRPolicy(const TerminalInfo &a, const TerminalInfo &b);
    void buildFrame(std::vector<TerminalInfo>& terminals);
    void scheduleTerminals();
};

#endif /* GROUNDSTATION_H_ */
