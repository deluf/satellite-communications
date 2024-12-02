
#include "GroundStation.h"
#include "codingRateMessage_m.h"
#include "Oracle.h" // TODO: Only used to convert coding rates to string literals

Define_Module(GroundStation);


void GroundStation::initialize()
{
    satellite = getModuleByPath("^.satellite");

    // TODO: Move this to the constructor, but only if there also are other things to put there
    acknowledgedCodingRates = 0;

    /* Storing CR values and queues in a terminal-id-indexed vector of TerminalInfo structs
     *
     * - when new CR arrives, read the sender's terminal id (using custom msg I guess)
     * and store the CR in the proper slot
     *
     * - Do the same when a new packet arrives and store the packet in
     * the proper terminal queue
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
                EV_DEBUG << "[groundStation]> CR: " << codingRateToString[terminal->codingRate] << ", ID: "
                        << terminal->id << ", Qlength: " << terminal->queue.getLength() << endl;
            }

            cMessage *frame = new cMessage("frame");
            sendDirect(frame, satellite, "in");

            EV_DEBUG << "[groundStation]> Received all coding rates, sending the frame..." << endl;
        }

        delete codingRateMessage;
    }
}

void GroundStation::buildFrame(std::vector<int>& sortedIndexes)
{
    for (int index : sortedIndexes) {
        TerminalStatus &terminal = terminals[index];

        EV_DEBUG << "[groundStation]> Adding Terminal ID=" << terminal.id
                << " with CR=" << terminal.codingRate
                << " to frame. Queue size=" << terminal.queue.getLength() << endl;

        /*
        * TODO: build and send out the frame using &queue and codingRate
        */
    }
}

void GroundStation::finish()
{

}
