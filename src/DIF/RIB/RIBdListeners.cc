// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Brno University of Technology, PRISTINE project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "RIBdListeners.h"
#include "IntRoutingUpdate.h"


RIBdListeners::RIBdListeners(RIBdBase* nribd) : ribd(nribd)
{
}

RIBdListeners::~RIBdListeners() {
    ribd = NULL;
}

void LisRIBDRcvData::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "ReceiveData initiated by " << src->getFullPath()
       << " and processed by " << ribd->getFullPath() << endl;
    CDAPMessage* cimsg = dynamic_cast<CDAPMessage*>(obj);
    if (cimsg) {
        ribd->receiveData(cimsg);
    }
    else
        EV << "RIBdListener received unknown object!" << endl;
}

void LisRIBDRoutingUpdate::receiveSignal(cComponent* src, simsignal_t id, cObject* obj)
{
    EV << "LisRIBDRoutingUpdate initiated by " << src->getFullPath()
       << " and processed by " << ribd->getFullPath() << endl;

    IntRoutingUpdate * info = dynamic_cast<IntRoutingUpdate *>(obj);

    if (info)
    {
        ribd->receiveRoutingUpdateFromRouting(info);
    }
    else
    {
        EV << "ForwardingInfoUpdate listener received unknown object!" << endl;
    }
}

void LisRIBDCongesNotif::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "LisRIBDCongesNotif initiated by " << src->getFullPath()
       << " and processed by " << ribd->getFullPath() << endl;

    PDU* pdu = dynamic_cast<PDU*>(obj);
    if (pdu)
       ribd->sendCongestionNotification(pdu);
    else
       EV << "RIBdListener received unknown object!" << endl;
}

