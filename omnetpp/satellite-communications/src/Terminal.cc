
#include <string>

#include "Terminal.h"
#include "Oracle.h"     // TODO: Only used to convert the coding rates enum to string literals
#include "codingRateMessage_m.h"

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
    delete timer;
}
