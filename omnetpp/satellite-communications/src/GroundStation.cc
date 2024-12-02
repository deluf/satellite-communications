
#include "GroundStation.h"
#include "Oracle.h"     // TODO: Only used to convert the coding rates enum to string literals
#include "codingRateMessage_m.h"

Define_Module(GroundStation);


void GroundStation::initialize()
{
    satellite = getModuleByPath("^.satellite");
    numTerminals = getParentModule()->par("N");

    // TODO: Move this to the constructor if there also are other things to put there
    acknowledgedCodingRates = 0;

    /* Storing CR values and queues in a terminal-id-indexed vector of TerminalInfo structs
     *
     * - when new CR arrives, read the sender's terminal id (using custom msg I guess)
     * and store the CR in the proper slot
     *
     * - Do the same when a new packet arrives and store the packet in
     * the proper terminal queue
     */

    terminals.resize(numTerminals);
    // Default crValue is 0 and the queue is empty
    for (int i = 0; i < numTerminals; ++i)
    {
        terminals[i] = {i, 0, cQueue()};
    }

}

void GroundStation::handleMessage(cMessage *msg)
{

    if (msg->isSelfMessage())
    {

    }
    else
    {

        CodingRateMessage *codingRateMessage = check_and_cast<CodingRateMessage*>(msg);

        int terminalId = codingRateMessage->getTerminalId();
        CODING_RATE codingRate = codingRateMessage->getCodingRate();

        terminals[terminalId].crValue = codingRate;
        acknowledgedCodingRates++;

        EV_DEBUG << "[groundStation]> Acknowledged coding rate " << codingRateToString[codingRate]
                 << " for terminal "<< terminalId << ", "
                 << numTerminals - acknowledgedCodingRates << " remaining" << endl;

        if (acknowledgedCodingRates == numTerminals)
        {
            acknowledgedCodingRates = 0;
            scheduleTerminals();
            cMessage *frame = new cMessage("frame");
            sendDirect(frame, satellite, "in");

            EV_DEBUG << "[groundStation]> Received all coding rates, sending the frame back..." << endl;
        }

        // todo: or msg? or both ? (i don't think both, and probably either delete codingRateMessage or delete msg is fine)
        delete codingRateMessage;
    }
}


void GroundStation::scheduleTerminals()
{
    /*
     * Creating a permutation of the indexes so that we can access
     * terminal structs in the proper order
     */
    std::vector<int> sortedIndexes(numTerminals);
    for (int i = 0; i < numTerminals; ++i) sortedIndexes[i] = i;

    /* Custom comparator to apply maxCRPolicy */
    std::sort(sortedIndexes.begin(), sortedIndexes.end(),
              [this](int a, int b) {
                  return terminals[a].crValue > terminals[b].crValue;
              });

    EV_DEBUG << "[groundStation]> Scheduling terminals based on CR values" << endl;
    buildFrame(sortedIndexes);
}

void GroundStation::buildFrame(std::vector<int>& sortedIndexes)
{
    for (int index : sortedIndexes) {
        TerminalInfo &terminal = terminals[index];

        EV_DEBUG << "[groundStation]> Adding Terminal ID=" << terminal.id
                << " with CR=" << terminal.crValue
                << " to frame. Queue size=" << terminal.queue.getLength() << endl;

        /*
        * TODO: build and send out the frame using &queue and crValue
        */
    }
}

void GroundStation::finish()
{

}
