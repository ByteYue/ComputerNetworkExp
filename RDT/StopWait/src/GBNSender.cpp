//
// Created by yue on 10/22/20.
//
#include "Global.h"
#include "GBNSender.h"

GBNSender::GBNSender():nextseqnum(1),waitingState(false),base(1) {
    this->packets = new std::vector<Packet*>();
}

GBNSender::~GBNSender() noexcept {
    if(this->packets!= nullptr){
        delete this->packets;
        this->packets = nullptr;
    }
}

bool GBNSender::getWaitingState() {
    return this->waitingState;
}



/**
 * 生成发送packet
 * @param  nextseqnum: seqnum of this packet
 * */
Packet* makePkt(int nextseqnum, const Message &message){
    auto *packet = new Packet();
    packet->acknum = -1;
    packet->checksum = 0;
    packet->seqnum = nextseqnum;
    memcpy(packet->payload, message.data, sizeof(message.data));
    packet->checksum = pUtils->calculateCheckSum(*packet);
    return packet;
}

/**
 * 检测是否能发送应用层数据message
 * */
bool GBNSender::send(const Message &message) {

    if(this->packets->size()<GBN_WINDOW_SIZE){
        auto packet = makePkt(nextseqnum,message);
        this->packets->push_back(packet);// get the send message
        pUtils->printPacket("Sender sends message:", *packet);
        if(base==nextseqnum){
            pns->startTimer(SENDER,Configuration::TIME_OUT, packet->seqnum);
        }
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


/**
 * 接受receiver的信息
 *
 * */
void GBNSender::receive(const Packet &ackPkt) {
    int checksum=pUtils->calculateCheckSum(ackPkt);
    if(checksum == ackPkt.checksum){//no corrupt
        //如果acknum大于目前的seqnum,不考虑这个情况
        if(((nextseqnum > base) && //仍有未收到ACK
        (ackPkt.acknum >= base) &&
        (ackPkt.acknum < nextseqnum))
        ||
        ((nextseqnum < base) && //nextseqnume可能因为mod操作而小于base
        ((ackPkt.acknum < nextseqnum)||
        (ackPkt.acknum >= base))))
        {
            //窗口没有全部堵住
            this->waitingState= false;

            int pre= this->base;//stop需要使用
            cout << "GBN 发送方收到ack = " << ackPkt.acknum << "窗口发生变化: 由 " << endl;
            printWindow();
            cout << "变为: " << endl;
            this->base=ackPkt.acknum+1;//考虑到累计积累,所以认为返回的acknum以前的所有数据全部发送成功
            this->base%=MAX_SEQ;
            printWindow();

            //计算要清理多少空间
            int index = 0;
            if (ackPkt.acknum >= pre) {
                index = ackPkt.acknum - pre;
            } else {
                index = MAX_SEQ - pre + ackPkt.acknum;
            }
            // 假设此时是base = 2 ^ k - 1，然后收到的ack = 0, 那么, 结果应该要删除2个
            for (int i = 0; i <= index; i++) {
                // free every useless memory
                delete packets->at(i);
            }
            packets->erase(packets->begin(), packets->begin() + index + 1);
            if(this->base== this->nextseqnum)//empty window
            {
                pns->stopTimer(SENDER,pre);
            }
            else{
                pns->stopTimer(SENDER,pre);
                pns->startTimer(SENDER,Configuration::TIME_OUT, this->base);
            }
        }
    }
}

/**
 * 超时重传
 * */
void GBNSender::timeoutHandler(int seqNum) {
    // 按照理论来说应该始终是base超时
    pns->stopTimer(SENDER, seqNum);
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
    int limit = 0;
    if (nextseqnum > base) {
        // 如果没有越过分界点
        limit = nextseqnum - base;
    } else if (nextseqnum < base) {
        // 如果越过了分界点
        limit = MAX_SEQ - base + nextseqnum;
        // 假设此时base = 2 ^ k - 1, nextsequm = 1, 那么应该重发2个包
    }

    for (int i = 0; i < limit; i++){
        // 遍历从base到nextseqnum-1的所有报文,进行重传
        Packet * packet = packets->at(i);
        pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", *packet);
        pns->sendToNetworkLayer(RECEIVER, *packet);
    }
}

void GBNSender::printWindow() {
    if (base < nextseqnum) {
        cout << "[ ";
        for (int i = base; i < nextseqnum;i++) {
            cout << i << ", ";
        }
        cout << "]" << endl;
    } else if (base > nextseqnum) {
        cout << "[ ";
        for (int i = base; i < MAX_SEQ; i++) {
            cout << i << ", ";
        }
        for (int i = 0; i < nextseqnum; i++) {
            cout << i << ", ";
        }
        cout << "]" << endl;
    } else {
        cout << "[ ]" << endl;
    }
}
