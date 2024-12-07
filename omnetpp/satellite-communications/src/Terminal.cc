
#include <iomanip>

#include "Terminal.h"
#include "codingRateMessage_m.h"
#include "Oracle.h" // TODO: Only used to convert coding rates to string literals
#include "Frame_m.h"

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
        Frame *frame = check_and_cast<Frame*>(msg);
        for (int i = 0; i < frame->getBlocksArraySize(); ++i)
        {
           Block& block = frame->getBlocksForUpdate(i);
           for (int j = 0; j < block.getPacketsArraySize(); ++j)
           {
               TerminalPacket* packet = block.getPacketsForUpdate(j);
               if (packet != nullptr && packet->getTerminalId() == id)
               {
                   double delay = (simTime() - packet->getCreationTime()).dbl();
                   emit(delaySignal, delay);
                   EV_DEBUG << "[terminal " << id << "]> Received packet "
                            << packet->getPacketId()
                            << " with a delay of " << delay << endl;
                   delete packet;
               }
           }
        }
        delete frame;
    }

}

void Terminal::finish()
{

}
