//
// Created by yue on 10/24/20.
//
#include "SRSender.h"
#include "Global.h"
SRSender::SRSender():waitingState(false),base(1),nextseqnum(1) {
    this->packets = new std::vector<Packet*>();
    this->isReceived = new std::vector<int>();
}

SRSender::~SRSender() noexcept {
    if(this->packets){
        delete this->packets;
        this->packets = nullptr;
    }
    if(this->isReceived){
        delete this->isReceived;
        this->isReceived = nullptr;
    }
}

Packet *SRSender::makePkt(int nextseqnum, const Message &message){
    auto *packet = new Packet();
    packet->acknum = -1;
    packet->checksum = 0;
    packet->seqnum = nextseqnum;
    memcpy(packet->payload, message.data, sizeof(message.data));
    packet->checksum = pUtils->calculateCheckSum(*packet);
    return packet;
}

bool SRSender::send(const Message &message) {
    if(this->packets->size()<=GBN_WINDOW_SIZE){
        auto packet = makePkt(nextseqnum,message);
        this->packets->push_back(packet);// get the send message
        this->isReceived->push_back(UNKNOWN);
        pUtils->printPacket("Sender sends message:", *packet);
        pns->startTimer(SENDER,Configuration::TIME_OUT, packet->seqnum);//每一个分组都需要记时
        this->nextseqnum++;
        this->nextseqnum %= MAX_SEQ;// mod 2^k
        this->waitingState = packets->size()>=GBN_WINDOW_SIZE;//判断是否处于等待ACK状态
        pns->sendToNetworkLayer(RECEIVER,*packet);
        packet = nullptr;
        return true;
    }
    else{
        return false;
    }
}

void SRSender::receive(const Packet &ackPkt) {
    cout << "收到ack = " << ackPkt.acknum << endl;
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    if (checkSum == ackPkt.checksum) {
        if (((nextseqnum > base) &&
        (ackPkt.acknum >= base) &&
        (ackPkt.acknum < nextseqnum))
        || ((nextseqnum < base) &&
        ((ackPkt.acknum < nextseqnum) ||
        (ackPkt.acknum >= base)))) {
            // 落在当前窗口
            // 停止计时
//            cout << "落在当前窗口之内" << endl;
            pns->stopTimer(SENDER, ackPkt.acknum);//对每一个分组都在记时
            int index = 0;
            if (ackPkt.acknum >= base) {
                // 正常情况
                index = ackPkt.acknum - base;
            } else {
                // 在边界上
                index = MAX_SEQ - base + ackPkt.acknum;
                // 假设 base = 2 ^ k -1，并且收到ack = 1，那么应该是第3个包, 即2 ^ k -1, 0, 1
            }
            // 更改对应的包的状态
            if (ackPkt.acknum == base) {
                cout << "SR 发送方收到ack = " << ackPkt.acknum << "窗口发生变化: 由 " << endl;
                printWindow();
                cout << "变为: " << endl;
            }
            (*isReceived)[index] = RECEIVED;
//            cout << "收到第" << index << "个ack" << endl;
            if (ackPkt.acknum == base) {
                // 如果等于base的话
//                cout << "等于base" << endl;

                while(!isReceived->empty() && isReceived->at(0) == RECEIVED) {
                    // 不断去除已经标记为收到的包
                    isReceived->erase(isReceived->begin());
                    packets->erase(packets->begin());
//                    cout << "清除一个标记为收到的包" << endl;
                    base++;
                }
                // 移动窗口开始位置
                base %= MAX_SEQ;
                printWindow();
//                cout << "此时base更新为 " << base << endl;
                // 理论上base 应该始终小于等于nextseqnum
//                assert(base <= nextseqnum);
            }
        } else {
//            cout << "没有落在当前窗口 ack = " << ackPkt.acknum << " base = " << base << " nextseqnum = " << nextseqnum << endl;
        }
    }
    waitingState = packets->size() >= GBN_WINDOW_SIZE;
}

bool SRSender::getWaitingState() {
    return this->waitingState;
}

void SRSender::timeoutHandler(int seqNum) {
    // 需要通过seqNum找到对应的序号
    int index = 0;
    if (seqNum >= base) {
        // 如果是正常情况超时
        index = seqNum - base;
    } else {
        // 如果是在边界上
        index = MAX_SEQ - base + seqNum;
        // 假设此时 base = 2 ^ k - 1, seqNum = 1, 那么就是第三个包超时了
    }
    // 找到对应的包并发送，并且重启计时器
    // 理论上找到的包和序号应该对应的上
    pns->sendToNetworkLayer(RECEIVER, *packets->at(index));
    pns->startTimer(SENDER, Configuration::TIME_OUT,seqNum);
}

void SRSender::printWindow() {
    cout << "> '-' 表示该包已经收到 < " << endl;
    if (base < nextseqnum) {
        cout << "[ ";
        for (int i = base; i < nextseqnum;i++) {
            if (this->isReceived->at(i - base) == RECEIVED) {
                cout << "-" << i << "-, ";
            } else {
                cout << i << ", ";
            }
        }
        cout << "]" << endl;
    } else if (base > nextseqnum) {
        cout << "[ ";
        for (int i = base; i < MAX_SEQ; i++) {
            if (this->isReceived->at(i - base) == RECEIVED) {
                cout << "-" << i << "-, ";
            } else {
                cout << i << ", ";
            }
        }
        for (int i = 0; i < nextseqnum; i++) {
            if (this->isReceived->at(i + MAX_SEQ - base) == RECEIVED) {
                cout << "-" << i << "-, ";
            } else {
                cout << i << ", ";
            }
        }
        cout << "]" << endl;
    } else {
        cout << "[ ]" << endl;
    }
}


