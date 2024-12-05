
#include <iomanip>

#include "Terminal.h"
#include "codingRateMessage_m.h"
#include "Oracle.h" // TODO: Only used to convert coding rates to string literals

Define_Module(Terminal);


void Terminal::initialize()
{
    satellite = getParentModule()->getSubmodule("satellite");
    delaySignal = registerSignal("delay");
    id = getIndex();
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

        // TODO: Check if i can do something like #ifdef DEBUG_RNGS then ...
        // EV_INFO << std::fixed << std::setprecision(5) << simTime().dbl() << " - [terminal " << getIndex() << "]> Extracted CR | Total: " << getRNG(0)->getNumbersDrawn() << endl;

        CodingRateMessage *codingRateMessage = new CodingRateMessage("codingRate");
        codingRateMessage->setTerminalId(id);
        codingRateMessage->setCodingRate(codingRate);

        sendDirect(codingRateMessage, satellite, "in");

        EV_DEBUG << "[terminal " << id << "]> My coding rate is now "
                << codingRateToString[codingRate] << endl;
    }
    else
    {
        // TODO: for each packet in the received frame (msg) belonging to this terminal ...
        {
            // TODO: change with (simTime() - packet->getCreationTime()).dbl()
            double delay = 0.01;
            emit(delaySignal, delay);

            EV_DEBUG << "[terminal " << id << "]> Received a packet of size "
                    << "X" << " with a delay of " << delay << endl;

            delete msg;
        }
    }

}

void Terminal::finish()
{

}
