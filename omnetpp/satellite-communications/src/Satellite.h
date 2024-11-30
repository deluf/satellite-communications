
#ifndef SATELLITE_H_
#define SATELLITE_H_

#include <omnetpp.h>

using namespace omnetpp;

class Satellite : public cSimpleModule {

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* SATELLITE_H_ */
