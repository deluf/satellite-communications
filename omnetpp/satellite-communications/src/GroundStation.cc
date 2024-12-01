
#include "GroundStation.h"
#include "Oracle.h"

Define_Module(GroundStation);


void GroundStation::initialize()
{
    satellite = getModuleByPath("^.satellite");
    numTerminals = getParentModule()->par("N");
    acknowledgedCodingRates = 0; // Maybe move this to constructor?

    /* Storing CR values and queues in terminal-id indexed vectors
     *
     * - when new CR arrives, read the sender's terminal id (using custom msg I guess)
     * and store the CR in the proper slot
     *
     * - Do the same when a new packet arrives and store the packet in
     * the proper terminal queue
     */
    crValues.resize(numTerminals);
    terminalQueues.resize(numTerminals);
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
        crValues[intuniform(0, numTerminals)] = std::stoi(msg->getName());
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

bool GroundStation::maxCRPolicy(const TerminalInfo &a, const TerminalInfo &b)
{
    return a.crValue > b.crValue;
}

void GroundStation::scheduleTerminals()
{
    /*
     * Creating a list of terminals with their IDs, CR values, and queues
     * Just a "fancy" way to group TerminalInfos together, not strictly necessary, an array of integers would do
     */
    std::vector<TerminalInfo> terminals;
    for (int i = 0; i < numTerminals; ++i) {
        terminals.push_back({i, crValues[i], terminalQueues[i]});
    }

    /* Custom comparator to apply maxCRPolicy */
    std::sort(terminals.begin(), terminals.end(), maxCRPolicy);

    EV_DEBUG << "[groundStation]> Scheduling terminals based on CR values" << endl;
    buildFrame(terminals);
}

void GroundStation::buildFrame(std::vector<TerminalInfo>& terminals)
{
    for (TerminalInfo &terminal : terminals) {
        int crValue = terminal.crValue;
        cQueue &queue = terminal.queue;

        EV_DEBUG << "[groundStation]> Adding Terminal ID=" << terminal.id
                 << " with CR=" << crValue
                 << " to frame. Queue size=" << queue.getLength() << endl;

        /*
        * TODO: build and send out the frame using &queue and crValue
        */
    }
}

void GroundStation::finish()
{

}
