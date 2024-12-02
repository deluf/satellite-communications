
#ifndef GROUNDSTATION_H_
#define GROUNDSTATION_H_

#include <vector>

#include "omnetpp.h"

using namespace omnetpp;

struct TerminalStatus
{
    int id;
    int codingRate;
    cQueue queue;
};

class GroundStation : public cSimpleModule
{
    cModule *satellite;
    int numTerminals;
    int acknowledgedCodingRates;
    std::vector<TerminalStatus> terminals;
    std::vector<TerminalStatus*> sortedTerminals;

    cMessage *buildFrame();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* GROUNDSTATION_H_ */
