
#ifndef GROUNDSTATION_H_
#define GROUNDSTATION_H_

#include "omnetpp.h"

using namespace omnetpp;

class GroundStation : public cSimpleModule
{
    int numTerminals;
    int acknowledgedCodingRates;
    cModule *satellite;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* GROUNDSTATION_H_ */
