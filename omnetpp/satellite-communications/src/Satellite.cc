
#include "Satellite.h"

Define_Module(Satellite);


void Satellite::initialize()
{
    cModule *satCom = getParentModule();
    terminals.resize(satCom->par("N").intValue());
    int i = 0;
    for (cModule* &terminal : terminals)
    {
        terminal = satCom->getSubmodule("terminal", i++);
    }
    groundStation = satCom->getSubmodule("groundStation");
}

void Satellite::handleMessage(cMessage *msg)
{
    if (msg->isName("codingRate"))
    {
        EV_DEBUG << "[satellite]> Received a message from terminal " << msg->getSenderModule()->getIndex()
                << ", forwarding it to the ground station" << endl;

        sendDirect(msg, groundStation, "in");
    }
    else if (msg->isName("frame"))
    {
        EV_DEBUG << "[satellite]> Received a message from the ground station, forwarding it to all the terminals" << endl;

        for (cModule* &terminal : terminals)
        {
            /* Duplicating the message because we can't send the same message object to multiple terminals */
            sendDirect(msg->dup(), terminal, "in");
        }

        /* Since we are duplicating it, the original message can be deleted */
        delete msg;
    }
    else
    {
        throw cRuntimeError(this, "The satellite can't handle the received "
                "message. Supported types are: \"codingRate\", \"frame\"");
    }

}
