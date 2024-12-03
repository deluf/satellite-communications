
#include "GroundStation.h"
#include "codingRateMessage_m.h"
#include "terminalPacket_m.h"
#include "customTimer_m.h"
#include "Oracle.h" // TODO: Only used to convert coding rates to string literals

Define_Module(GroundStation);

/*
 * TODO:
 * buildFrame()
 * independent RNGs
 * omnetpp.ini
 * statistics
 */

void GroundStation::initialize()
{
    satellite = getParentModule()->getSubmodule("satellite");

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
        terminals[i].id = i;
        sortedTerminals[i] = &terminals[i];

        CustomTimer *customTimer = new CustomTimer("customTimer");
        customTimer->setTerminalId(i);

        // todo: may be a code duplicate
        double nextArrivalTime = par("T").doubleValue();
        scheduleAt(simTime() + SimTime(nextArrivalTime), customTimer);

        EV_DEBUG << "[groundStation]> the first packet for terminal " << i
                << " is scheduled to arrive in " << nextArrivalTime * 1e3 << " ms" << endl;
    }

    receivedCodingRates = 0;
}

void GroundStation::handleMessage(cMessage *msg)
{

    if (msg->isSelfMessage())
    {
        CustomTimer *customTimer = check_and_cast<CustomTimer*>(msg);

        int terminalId = customTimer->getTerminalId();
        int byteLength = par("S").intValue();

        TerminalPacket *terminalPacket = new TerminalPacket();
        terminalPacket->setTerminalId(terminalId);
        terminalPacket->setByteLength(byteLength);

        terminals[terminalId].queue.insert(terminalPacket);

        // todo: may be a code duplicate
        double nextArrivalTime = par("T").doubleValue();
        scheduleAt(simTime() + SimTime(nextArrivalTime), customTimer);

        EV_DEBUG << "[groundStation]> terminal " << terminalId << " received a new packet of size "
                << byteLength << ", the next one is scheduled to arrive in " << nextArrivalTime * 1e3 << " ms" << endl;
    }
    else
    {
        CodingRateMessage *codingRateMessage = check_and_cast<CodingRateMessage*>(msg);

        int terminalId = codingRateMessage->getTerminalId();
        CODING_RATE codingRate = codingRateMessage->getCodingRate();

        terminals[terminalId].codingRate = codingRate;
        receivedCodingRates++;

        EV_DEBUG << "[groundStation]> Acknowledged coding rate " << codingRateToString[codingRate]
                 << " for terminal "<< terminalId << ", "
                 << numTerminals - receivedCodingRates << " remaining" << endl;

        /* All of the coding rates have been received */
        if (receivedCodingRates == numTerminals)
        {
            receivedCodingRates = 0;

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

    const int M = par("M");
    int numBlocks = 0;

    for (TerminalStatus *terminal : sortedTerminals) {

        EV_DEBUG << "[groundStation]> Considering terminal with ID: " << terminal->id
                << ", CR: " << terminal->codingRate
                << ", Queue length: " << terminal->queue.getLength() << endl;


        // todo: remember to delete the packet custom messages


        // TODO: actually schedule stuff...
        /*
         * { Frame
         *    dimensione
         *    [
         *      { Blocco
         *          dimensioneMax
         *          [
         *              { Pacchetto
         *                  terminalId
         *                  dimensione
         *
         *              }
         *              ...
         *              { Pacchetto
         *                  ...
         *              }
         *          ]
         *      }
         *      ...
         *      { Blocco
         *          ...
         *      }
         *    ]
         * }
         *
         */

    }

    return frame;
}

void GroundStation::finish()
{

}
