//
// Created by yue on 10/22/20.
//
#include "Global.h"
#include "GBNSender.h"

GBNSender::GBNSender():expectSequenceNumberSend(0),waitingState(false) {

}

GBNSender::~GBNSender() noexcept {}

bool GBNSender::getWaitingState() {
    return this->waitingState;
}


