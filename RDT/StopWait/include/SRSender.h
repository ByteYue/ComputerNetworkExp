//
// Created by yue on 10/24/20.
//

#ifndef STOPWAIT_SRSENDER_H
#define STOPWAIT_SRSENDER_H
#include "RdtSender.h"
#include <vector>
class SRSender: public RdtSender{
private:
    enum state{RECEIVED,UNKNOWN};
    bool waitingState;				// 是否处于等待Ack的状态
    int base;
    int nextseqnum;
    std::vector<Packet *> *packets; //窗口
    std::vector<int>    *isReceived; //记录对应分组的packet是否被正确接受

public:
    static Packet* makePkt(int nextseqnum, const Message &message);
    bool getWaitingState() override;
    bool send(const Message &message) override;						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt) override;						//接受确认Ack，将被NetworkServiceSimulator调用
    void timeoutHandler(int seqNum) override;					//Timeout handler，将被NetworkServiceSimulator调用
    void printWindow();
public:
    SRSender();
    ~SRSender() override;
};

#endif //STOPWAIT_SRSENDER_H
