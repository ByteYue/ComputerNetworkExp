//
// Created by yue on 10/22/20.
//

#ifndef STOPWAIT_GBNRECEIVER_H
#define STOPWAIT_GBNRECEIVER_H
#include "RdtReceiver.h"

class GBNReceiver: public RdtReceiver{

private:
    int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
    Packet lastAckPkt;				//上次发送的确认报文,Packet.seqnum在GBNReceiver里不需要使用
public:
    GBNReceiver();
    ~GBNReceiver() override;
    void receive(const Packet &packet); //接收报文，将被NetworkService调用
};


#endif //STOPWAIT_GBNRECEIVER_H
