
#include "Satellite.h"

Define_Module(Satellite);


void Satellite::initialize()
{

}

void Satellite::handleMessage(cMessage *msg)
{
    cModule *gs = getModuleByPath("^.groundStation");
    sendDirect(msg, gs , "in");
}

void Satellite::finish()
{

}
