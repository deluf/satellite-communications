
#include <string>

#include "Terminal.h"
#include "codingRateMessage_m.h"
#include "Oracle.h" // TODO: Only used to convert coding rates to string literals

Define_Module(Terminal);


void Terminal::initialize()
{
    satellite = getParentModule()->getSubmodule("satellite");
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
        CODING_RATE codingRate = (CODING_RATE)par("codingRate").intValue();

        CodingRateMessage *codingRateMessage = new CodingRateMessage("codingRate");
        codingRateMessage->setTerminalId(getIndex());
        codingRateMessage->setCodingRate(codingRate);

        sendDirect(codingRateMessage, satellite, "in");

        EV_DEBUG << "[terminal " << getIndex() << "]> My coding rate is now "
                << codingRateToString[codingRate] << endl;
    }
    else
    {
        EV_DEBUG << "[terminal " << getIndex() << "]> Received: " << msg->getName() << endl;
        delete msg;
    }

}

void Terminal::finish()
{

}
