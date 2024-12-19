
#ifndef PACKETSCHEDULER_H_
#define PACKETSCHEDULER_H_

#include <vector>

#include "omnetpp.h"
#include "Oracle.h"
#include "Frame_m.h"
#include "CodingRatePacket_m.h"

using namespace omnetpp;

struct TerminalStatus
{
    int id;
    CODING_RATE codingRate;
    cQueue queue;
};

class PacketScheduler : public cSimpleModule
{
    Oracle *oracle;
    cModule *satellite;
    simsignal_t throughputSignal;
    std::vector<TerminalStatus> terminals;
    std::vector<TerminalStatus*> sortedTerminals;
    int numTerminals;
    int receivedCodingRates;
    long debugTotalBitsSent;    // TODO: Just for debugging
    int blocksPerFrame;

    Frame *buildFrame();
    int getBlockSizeForCodingRate(CODING_RATE codingRate);
    void initBlock(Block* block, CODING_RATE codingRate);

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* PACKETSCHEDULER_H_ */
