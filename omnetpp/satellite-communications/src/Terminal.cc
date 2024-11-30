
#include "Terminal.h"

Define_Module(Terminal);


void Terminal::initialize()
{
    cMessage *timer = new cMessage("timer");
    scheduleAt(simTime() + uniform(0, 1), timer);
}

void Terminal::handleMessage(cMessage *msg)
{
    cMessage *test = new cMessage("test");
    cModule *satellite = getModuleByPath("^.satellite");
    sendDirect(test, satellite , "in");
}

void Terminal::finish()
{

}
