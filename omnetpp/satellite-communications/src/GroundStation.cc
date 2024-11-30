
#include "GroundStation.h"
#include "Oracle.h"

Define_Module(GroundStation);


void GroundStation::initialize()
{
    satellite = getModuleByPath("^.satellite");
    numTerminals = getParentModule()->par("N");
    acknowledgedCodingRates = 0;
}

void GroundStation::handleMessage(cMessage *msg)
{

    if (msg->isSelfMessage())
    {

    }
    else
    {
        acknowledgedCodingRates++;

        EV_DEBUG << "[groundStation]> Acknowledged coding rate " << codingRateToString[std::stoi(msg->getName())]
                << " for terminal "<< "X" << ", "
                << numTerminals - acknowledgedCodingRates << " remaining" << endl;

        if (acknowledgedCodingRates == numTerminals)
        {
            acknowledgedCodingRates = 0;
            cMessage *frame = new cMessage("frame");
            sendDirect(frame, satellite, "in");

            EV_DEBUG << "[groundStation]> Received all coding rates, sending the frame back...";
        }

        delete msg;
    }


}

void GroundStation::finish()
{

}
