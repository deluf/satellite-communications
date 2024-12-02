
#include "Satellite.h"

Define_Module(Satellite);

void Satellite::initialize()
{
    /*
     * Keeping a pointer to the ground station and to all the terminals,
     * so that we don't have to fetch them each time later on
     */

    groundStation = getModuleByPath("^.groundStation");

    cModule *satCom = getParentModule();
    numTerminals = satCom->par("N");

    // TODO: Maybe it's not the "c++" way of doing this
    terminals = new cModule*[numTerminals];
    for (int i = 0; i < numTerminals; i++)
    {
        terminals[i] = satCom->getSubmodule("terminal", i);
    }
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
        for (int i = 0; i < numTerminals; i++)
        {
            /* Duplicating the message because we can't send the same message object to multiple terminals */
            sendDirect(msg->dup(), terminals[i] , "in");
        }

        /* Since we are duplicating it, the original message can be deleted */
        delete msg;

        EV_DEBUG << "[satellite]> Received a message from the ground station, forwarding it to all the terminals" << endl;
    }

}

void Satellite::finish()
{
    delete terminals;
}
