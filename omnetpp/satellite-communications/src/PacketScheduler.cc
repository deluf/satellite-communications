
#include "PacketScheduler.h"
#include "codingRateMessage_m.h"
#include "terminalPacket_m.h"
#include "Oracle.h" // TODO: Only used to convert coding rates to string literals

Define_Module(PacketScheduler);


void PacketScheduler::initialize()
{
    throughputSignal = registerSignal("throughput");

    cModule *satCom = getParentModule()->getParentModule();
    satellite = satCom->getSubmodule("satellite");
    numTerminals = satCom->par("N").intValue();

    /*
     * The status of each terminal (id, current coding rate and queue) is stored in a
     *  vector of terminalStatus structs indexed by the id field (i.e. terminals[i].id = i).
     * This allows to update the coding rate of any terminal in O(1) whenever a new codingRateMessage arrives.
     *
     * In order to serve the terminals according to a maximum-coding-rate policy without losing the
     *  indexing property described before, a vector of pointers to those terminalStatus structs is used.
     * This way, we don't actually sort the terminalStatus structs, but the pointers to them.
     */

    terminals.resize(numTerminals);
    sortedTerminals.resize(numTerminals);

    for (int i = 0; i < numTerminals; i++)
    {
        terminals[i].id = i;
        sortedTerminals[i] = &terminals[i];
    }

    receivedCodingRates = 0;
    debugTotalBitsSent = 0;
}

void PacketScheduler::handleMessage(cMessage *msg)
{
    if (msg->isName("terminalPacket"))
    {
        TerminalPacket *terminalPacket = check_and_cast<TerminalPacket*>(msg);

        // TODO: We don't really need the terminalId, since we should be able to deduce it from the arrival gate
        int terminalId = terminalPacket->getTerminalId();
        int byteLength = terminalPacket->getByteLength();

        terminals[terminalId].queue.insert(terminalPacket);

        EV_DEBUG << "[packetScheduler]> terminal " << terminalId << " received a new packet of size " << byteLength << endl;
    }

    /* Message received from the satellite */
    else
    {
        CodingRateMessage *codingRateMessage = check_and_cast<CodingRateMessage*>(msg);

        int terminalId = codingRateMessage->getTerminalId();
        CODING_RATE codingRate = codingRateMessage->getCodingRate();

        terminals[terminalId].codingRate = codingRate;
        receivedCodingRates++;

        EV_DEBUG << "[packetScheduler]> Acknowledged coding rate " << codingRateToString[codingRate]
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

            EV_DEBUG << "[packetScheduler]> Sorted terminals:" << endl;
            for (TerminalStatus* &terminal: sortedTerminals)
            {
                EV_DEBUG << "[packetScheduler]> ID: " << terminal->id << ", CR: "
                        << codingRateToString[terminal->codingRate]
                        << ", Queue length: " << terminal->queue.getLength() << endl;
            }

            EV_DEBUG << "[groundStation]> Building the frame..." << endl;

            cMessage *frame = buildFrame();

            // TODO: Change with the actual size of the frame
            long bitLength = 100;
            debugTotalBitsSent += bitLength;

            /*
             * The throughput statistic uses sumPerDuration(throughputSignal) as a source, and records the last value.
             * The sumPerDuration filter does the following:
             *  For each value, computes the sum of values received so far, divides it by the duration, and outputs the result.
             * By default, the duration is equal to the current simulation time,
             *  since the first emission happens at simulation time 0, the throughput would be infinite
             *  and since, in the reals system, the frame actually arrives to the terminals in at most 80ms,
             * then we manually fix the emission to happen 80ms after the current simulation time.
             */
            cTimestampedValue tmp(simTime() + SimTime(80, SIMTIME_MS), (intval_t)bitLength);
            emit(throughputSignal, &tmp);

            sendDirect(frame, satellite, "in");

            EV_DEBUG << "[packetScheduler]> Frame of size " << bitLength << " bits sent!" << endl;
        }

        delete codingRateMessage;
    }
}

cMessage *PacketScheduler::buildFrame()
{
    cMessage *frame = new cMessage("frame");

    for (TerminalStatus *terminal : sortedTerminals)
    {

        EV_DEBUG << "[packetScheduler]> Considering terminal with ID: " << terminal->id
                << ", CR: " << terminal->codingRate
                << ", Queue length: " << terminal->queue.getLength() << endl;

        // TODO: remember to delete the terminalPacket(s) after removing them from the queue,
        //       or, if we embed them in the frame, remember to delete them in the terminals

        // TODO: actually schedule stuff...


    }

    return frame;
}

void PacketScheduler::finish()
{
    EV_DEBUG << "### Simulation finished ###" << endl
            << "\tTotal number of bits sent: " << debugTotalBitsSent << endl
            << "\tSimulation time: " << simTime() << " s" << endl
            << "\tExpected throughput: " << ((double)debugTotalBitsSent)/(simTime().dbl()) << " bps" << endl;
}
