//
// Created by yue on 10/22/20.
//

#ifndef STOPWAIT_GBNSENDER_H
#define STOPWAIT_GBNSENDER_H
#include "RdtSender.h"
#include <vector>
class GBNSender: public RdtSender{
private:
    //int expectSequenceNumberSend;	// 下一个发送序号
    bool waitingState;				// 是否处于等待Ack的状态
    //Packet packetWaitingAck;		//已发送并等待Ack的数据包
    int base;
    int nextseqnum;
    std::vector<Packet *> *packets;
    //Packet** packets;
public:

    bool getWaitingState() override;
    bool send(const Message &message) override;						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt) override;						//接受确认Ack，将被NetworkServiceSimulator调用
    void timeoutHandler(int seqNum) override;					//Timeout handler，将被NetworkServiceSimulator调用
    void printWindow();
public:
    GBNSender();
    ~GBNSender() override;
};
#endif //STOPWAIT_GBNSENDER_H
