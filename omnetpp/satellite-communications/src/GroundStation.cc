
#include "GroundStation.h"
#include "Oracle.h"

Define_Module(GroundStation);


void GroundStation::initialize()
{
    satellite = getModuleByPath("^.satellite");
    numTerminals = getParentModule()->par("N");
    acknowledgedCodingRates = 0; // Maybe move this to constructor?

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
    for (int i = 0; i < numTerminals; ++i) terminals[i] = {i, 0, cQueue()};

}

void GroundStation::handleMessage(cMessage *msg)
{

    if (msg->isSelfMessage())
    {

    }
    else
    {
        /*
         * TODO: Choose how to make the terminal send its terminal ID to the GS
         * here I'm assigning it randomly just for testing purposes
         */
        terminals[intuniform(0, numTerminals - 1)].crValue = std::stoi(msg->getName());
        acknowledgedCodingRates++;

        EV_DEBUG << "[groundStation]> Acknowledged coding rate " << codingRateToString[std::stoi(msg->getName())]
                 << " for terminal "<< "X" << ", "
                 << numTerminals - acknowledgedCodingRates << " remaining" << endl;

        if (acknowledgedCodingRates == numTerminals)
        {
            acknowledgedCodingRates = 0;
            scheduleTerminals();
            cMessage *frame = new cMessage("frame");
            sendDirect(frame, satellite, "in");

            EV_DEBUG << "[groundStation]> Received all coding rates, sending the frame back...";
        }

        delete msg;
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
