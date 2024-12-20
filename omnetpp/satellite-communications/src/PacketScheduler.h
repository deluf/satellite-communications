
#ifndef PACKETSCHEDULER_H_
#define PACKETSCHEDULER_H_

#include <vector>
#include <algorithm>

#include "omnetpp.h"
#include "Oracle.h"
#include "CodingRatePacket_m.h"
#include "Frame_m.h"

using namespace omnetpp;

struct TerminalDescriptor
{
    int id;
    CODING_RATE codingRate;
    cQueue packetQueue;
};

class PacketScheduler : public cSimpleModule
{
    Oracle *oracle;
    cModule *satellite;
    simsignal_t throughputSignal;
    simtime_t communicationSlotDuration;
    std::vector<TerminalDescriptor> terminals;
    std::vector<TerminalDescriptor*> sortedTerminals;
    int terminalCount;
    int receivedCodingRateCount;
    int blocksPerFrame;

#ifdef DEBUG_SCHEDULER
    long totalBitsSent;
#endif

    void handlePacket(Packet *packet);
    void handleCodingRatePacket(CodingRatePacket *codingRatePacket);
    void schedulePackets();

    Frame *buildFrame();
    void initBlock(Block* block, CODING_RATE codingRate);
    const int getBlockSizeForCodingRate(CODING_RATE codingRate) const;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif /* PACKETSCHEDULER_H_ */
