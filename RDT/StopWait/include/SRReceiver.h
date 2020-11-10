//
// Created by yue on 10/24/20.
//

#ifndef STOPWAIT_SRRECEIVER_H
#define STOPWAIT_SRRECEIVER_H
#include "RdtReceiver.h"
#include <vector>
class SRReceiver :public RdtReceiver {
private:
    Packet lastAckPkt;//记录刚刚接受到的报文的信息(ACK等)
    enum {RECEIVED, UNKNOWN};
    std::vector<Packet *> *buffers;
    std::vector<int> *isReceived;
    int rcv_base;

    // 填充空的packet
    void push_empty_packet();
    void copy_packet(const Packet * source, Packet * dest);
    void printWindow();
public:
    SRReceiver();
    virtual ~SRReceiver();

public:
    void receive(const Packet &packet);
};
#endif //STOPWAIT_SRRECEIVER_H
