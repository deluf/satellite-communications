
#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "omnetpp.h"

using namespace omnetpp;

class Terminal : public cSimpleModule
{
    cMessage *timer;
    cModule *satellite;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* TERMINAL_H_ */
