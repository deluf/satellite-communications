
#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "omnetpp.h"
#include "Oracle.h"

using namespace omnetpp;

class Terminal : public cSimpleModule
{
    Oracle *oracle;
    cMessage *timer;
    cModule *satellite;
    simsignal_t delaySignal;
    int id;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* TERMINAL_H_ */
