
#include <string>

#include "Terminal.h"
#include "codingRateMessage_m.h"
#include "Oracle.h" // TODO: Only used to convert coding rates to string literals

Define_Module(Terminal);


void Terminal::initialize()
{
    satellite = getParentModule()->getSubmodule("satellite");
    delaySignal = registerSignal("delay");
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
        // TODO: for each packet in the received frame (msg) belonging to this terminal
        {
            // TODO: change with (simTime() - packet->getCreationTime()).dbl()
            double delay = uniform(0, 0.08);
            emit(delaySignal, delay);

            EV_DEBUG << "[terminal " << getIndex() << "]> Received a packet of size "
                    << "X" << " with a delay of " << delay << endl;

            delete msg;
        }
    }

}

void Terminal::finish()
{

}
