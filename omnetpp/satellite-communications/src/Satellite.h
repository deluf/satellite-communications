
#ifndef SATELLITE_H_
#define SATELLITE_H_

#include <vector>

#include "omnetpp.h"

using namespace omnetpp;

class Satellite : public cSimpleModule
{
    /*
     * Keeping a pointer to the ground station and to all the terminals,
     *  so that it doesn't have to fetch them each time a message arrives.
     */
    cModule *groundStation;
    std::vector<cModule*> terminals;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif /* SATELLITE_H_ */
