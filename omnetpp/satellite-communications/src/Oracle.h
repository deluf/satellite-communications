
#ifndef ORACLE_H_
#define ORACLE_H_

#include "omnetpp.h"

using namespace omnetpp;

static const char *codingRateToString[7] = {
    "L3", "L2", "L1", "R", "H1", "H2", "H3"
};

class Oracle : public cSimpleModule
{
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* ORACLE_H_ */
