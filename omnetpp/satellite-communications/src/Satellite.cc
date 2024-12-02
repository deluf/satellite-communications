
#include "Satellite.h"

Define_Module(Satellite);

void Satellite::initialize()
{
    /*
     * Keeping a pointer to the ground station and to all the terminals,
     * so that we don't have to fetch them each time later on
     */

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
    cModule *sender = msg->getSenderModule();

    if (sender->isName("terminal"))
    {
        EV_DEBUG << "[satellite]> Received a message from terminal " << sender->getIndex()
                << ", forwarding it to the ground station" << endl;

        sendDirect(msg, groundStation, "in");
    }
    else if (sender->isName("groundStation"))
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

}

void Satellite::finish()
{

}
