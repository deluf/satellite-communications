
#ifndef GROUNDSTATION_H_
#define GROUNDSTATION_H_

#include "omnetpp.h"

using namespace omnetpp;

struct TerminalInfo {
    int id;
    int crValue;
    cQueue queue;
};

class GroundStation : public cSimpleModule
{
    int numTerminals;
    int acknowledgedCodingRates;
    std::vector<TerminalInfo> terminals;
    cModule *satellite;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    void buildFrame(std::vector<int>& sortedIndices);
    void scheduleTerminals();
};

#endif /* GROUNDSTATION_H_ */
