
#ifndef GROUNDSTATION_H_
#define GROUNDSTATION_H_

#include "omnetpp.h"

using namespace omnetpp;

struct TerminalInfo
{
    int id;
    int crValue;    // TODO: this can just be named "codingRate", and could be of type CODING_RATE (defined in "oracle.h")
    cQueue queue;
};

class GroundStation : public cSimpleModule
{
    cModule *satellite;
    int numTerminals;
    int acknowledgedCodingRates;
    std::vector<TerminalInfo> terminals;

    void buildFrame(std::vector<int>& sortedIndices);
    void scheduleTerminals();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* GROUNDSTATION_H_ */
