
#include <string>

#include "Terminal.h"
#include "Oracle.h"

Define_Module(Terminal);


void Terminal::initialize()
{
    satellite = getModuleByPath("^.satellite");
    timer = new cMessage("timer");
    scheduleAt(simTime(), timer);
}

void Terminal::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        /* Schedule the next timer */
        scheduleAt(simTime() + SimTime(80, SIMTIME_MS), timer);

        /* Send the coding rate to the ground station */
        int codingRateIndex = par("codingRateIndex").intValue();
        std::string codingRateMessageName = std::to_string(codingRateIndex);
        cMessage *codingRateMessage = new cMessage(codingRateMessageName.c_str());
        sendDirect(codingRateMessage, satellite, "in");

        EV_DEBUG << "[terminal " << getIndex() << "]> My coding rate is now "
                << codingRateToString[codingRateIndex] << endl;
    }
    else
    {
        EV_DEBUG << "[terminal " << getIndex() << "]> Received " << msg->getName() << endl;
        delete msg;
    }

}

void Terminal::finish()
{
    delete timer;
}
