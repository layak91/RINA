//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <DAFEnrollmentNotifierListeners.h>

DAFEnrollmentNotifierListeners::DAFEnrollmentNotifierListeners(DAFEnrollmentNotifierBase* nenb) : enb(nenb)
{
}

DAFEnrollmentNotifierListeners::~DAFEnrollmentNotifierListeners() {
    enb = NULL;
}

void LisDAFEnrollmentNotifierRcvCACE::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "ReceiveCACEData initiated by " << src->getFullPath()
       << " and processed by " << enb->getFullPath() << endl;
    CDAPMessage* cimsg = dynamic_cast<CDAPMessage*>(obj);
    if (cimsg) {
        enb->receiveMessage(cimsg);
    }
    else
        EV << "DafEnrollmentNotifierListener received unknown object!" << endl;
}

void LisDAFEnrollmentNotifierRcvEnrollCACE::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "Send CACE from Enrollment" << endl;
    CDAPMessage* cimsg = dynamic_cast<CDAPMessage*>(obj);
    if (cimsg) {
        enb->sendCACE(cimsg);
    }
    else
        EV << "DafEnrollmentNotifierListener received unknown object!" << endl;
}

void LisDAFEnrollmentNotifierStaEnrolReq::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "StartEnrollmentRequest initiated by " << src->getFullPath()
       << " and processed by " << enb->getFullPath() << endl;
    DAFEnrollmentObj* enroll = dynamic_cast<DAFEnrollmentObj*>(obj);
    if (enroll)
        enb->sendStartEnrollmentRequest(enroll);
    else
        EV << "DafEnrollmentNotifierListener received unknown object!" << endl;

}

void LisDAFEnrollmentNotifierStaEnrolRes::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "StartEnrollmentResponse initiated by " << src->getFullPath()
       << " and processed by " << enb->getFullPath() << endl;
    DAFEnrollmentObj* enroll = dynamic_cast<DAFEnrollmentObj*>(obj);
    if (enroll)
        enb->sendStartEnrollmentResponse(enroll);
    else
        EV << "DafEnrollmentNotifierListener received unknown object!" << endl;

}

void LisDAFEnrollmentNotifierStoEnrolReq::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "StopEnrollmentRequest initiated by " << src->getFullPath()
       << " and processed by " << enb->getFullPath() << endl;
    DAFEnrollmentObj* enroll = dynamic_cast<DAFEnrollmentObj*>(obj);
    if (enroll)
        enb->sendStopEnrollmentRequest(enroll);
    else
        EV << "DafEnrollmentNotifierListener received unknown object!" << endl;

}

void LisDAFEnrollmentNotifierStoEnrolRes::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "StopEnrollmentResponse initiated by " << src->getFullPath()
       << " and processed by " << enb->getFullPath() << endl;
    DAFEnrollmentObj* enroll = dynamic_cast<DAFEnrollmentObj*>(obj);
    if (enroll)
        enb->sendStopEnrollmentResponse(enroll);
    else
        EV << "DafEnrollmentNotifierListener received unknown object!" << endl;

}

void LisDAFEnrollmentNotifierStaOperReq::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "StartOperationRequest initiated by " << src->getFullPath()
       << " and processed by " << enb->getFullPath() << endl;
    DAFOperationObj* oper = dynamic_cast<DAFOperationObj*>(obj);
    if (oper)
        enb->sendStartOperationRequest(oper);
    else
        EV << "DafEnrollmentNotifierListener received unknown object!" << endl;

}

void LisDAFEnrollmentNotifierStaOperRes::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "StartOperationResponse initiated by " << src->getFullPath()
       << " and processed by " << enb->getFullPath() << endl;
    DAFOperationObj* oper = dynamic_cast<DAFOperationObj*>(obj);
    if (oper)
        enb->sendStartOperationResponse(oper);
    else
        EV << "DafEnrollmentNotifierListener received unknown object!" << endl;

}
