
#include "GroundStation.h"
#include "codingRateMessage_m.h"
#include "Oracle.h" // TODO: Only used to convert coding rates to string literals

Define_Module(GroundStation);


void GroundStation::initialize()
{
    satellite = getModuleByPath("^.satellite");

    // TODO: Move this to the constructor iff there also are other things to put there, otherwise it's ugly lol
    acknowledgedCodingRates = 0;

    /*
     * The status of each terminal (id, current coding rate and queue) is stored in a
     *  vector of terminalStatus structs indexed by the id field (i.e. terminals[i].id = i).
     * This allows to update the coding rate of any terminal in O(1) whenever a new codingRateMessage arrives.
     *
     * In order to serve the terminals according to a maximum-coding-rate policy without losing the
     *  indexing property described before, a vector of pointers to those terminalStatus structs is used.
     * This way, we don't actually sort the terminalStatus structs, but the pointers to them.
     */

    numTerminals = getParentModule()->par("N").intValue();
    terminals.resize(numTerminals);
    sortedTerminals.resize(numTerminals);

    for (int i = 0; i < numTerminals; i++)
    {
        terminals[i] = {i, 0, cQueue()};  // TODO: the cQueue() constructor may have been already called with resize()
        sortedTerminals[i] = &terminals[i];
    }
}

void GroundStation::handleMessage(cMessage *msg)
{

    if (msg->isSelfMessage())
    {
        // TODO: implement packet arrivals
    }
    else
    {
        CodingRateMessage *codingRateMessage = check_and_cast<CodingRateMessage*>(msg);

        int terminalId = codingRateMessage->getTerminalId();
        CODING_RATE codingRate = codingRateMessage->getCodingRate();

        terminals[terminalId].codingRate = codingRate;
        acknowledgedCodingRates++;

        EV_DEBUG << "[groundStation]> Acknowledged coding rate " << codingRateToString[codingRate]
                 << " for terminal "<< terminalId << ", "
                 << numTerminals - acknowledgedCodingRates << " remaining" << endl;

        /* All of the coding rates have been received */
        if (acknowledgedCodingRates == numTerminals)
        {
            acknowledgedCodingRates = 0;

            std::sort(
                sortedTerminals.begin(), sortedTerminals.end(),
                [](const TerminalStatus *a, const TerminalStatus *b)
                {
                    return a->codingRate > b->codingRate;
                }
            );

            EV_DEBUG << "[groundStation]> Sorted terminals:" << endl;
            for (TerminalStatus* &terminal: sortedTerminals)
            {
                EV_DEBUG << "[groundStation]> ID: " << terminal->id << ", CR: "
                        << codingRateToString[terminal->codingRate]
                        << ", Queue length: " << terminal->queue.getLength() << endl;
            }

            EV_DEBUG << "[groundStation]> Building the frame..." << endl;

            cMessage *frame = buildFrame();
            sendDirect(frame, satellite, "in");

            EV_DEBUG << "[groundStation]> Frame sent!" << endl;
        }

        delete codingRateMessage;
    }
}

cMessage *GroundStation::buildFrame()
{
    cMessage *frame = new cMessage("frame");

    for (TerminalStatus *terminal : sortedTerminals) {

        EV_DEBUG << "[groundStation]> Considering terminal with ID: " << terminal->id
                << ", CR: " << terminal->codingRate
                << ", Queue length: " << terminal->queue.getLength() << endl;

        // TODO: actually schedule stuff...

    }

    return frame;
}

void GroundStation::finish()
{

}
