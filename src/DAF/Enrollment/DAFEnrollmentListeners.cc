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

/**
 * @file DAFEnrollmentListeners.cc
 * @author Kamil Jerabek (xjerab18@stud.fit.vutbr.cz)
 * @date Apr 1, 2015
 * @brief DAFEnrollment and CACE listeners
 * @detail
 */


#include <DAFEnrollmentListeners.h>

DAFEnrollmentListeners::DAFEnrollmentListeners(DAFEnrollment* nenrollment): enrollment(nenrollment) {
}

DAFEnrollmentListeners::~DAFEnrollmentListeners() {
    enrollment = NULL;
}

void LisDAFEnrollmentAllResPosi::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "AllocationResponsePositive initiated by " << src->getFullPath() << " and processed by " << enrollment->getFullPath() << endl;
    /*Flow* flow = dynamic_cast<Flow*>(obj);
    if (flow) {
        if (!flow->isManagementFlow()){
            return;
        }

        enrollment->startCACE(flow);
    }
    else
        EV << "DAFEnrollmentListener reeived unknown object!" << endl;
    */


    // TODO: refactor this stuff
    Flow* flow = dynamic_cast<Flow*>(obj);
    APNIPair* apnip = new APNIPair(flow->getSrcApni(),flow->getDstApni());
    if (flow) {
        if (enrollment->test) {
            enrollment->test = false;
            enrollment->createBindings(*flow);
        }

        enrollment->startCACE(apnip);
    }
    else {
        EV << "DAFEnrollmentListener received unknown object!" << endl;
    }

}

void LisDAFEnrollmentGetFlowFromFaiCreResPosi::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    EV << "GetFlowFromFaiCreResPosi initiated by " << src->getFullPath() << " and processed by " << enrollment->getFullPath() << endl;
    Flow* flow = dynamic_cast<Flow*>(obj);
    if (flow){
        if (!flow->isManagementFlow()){
            return;
        }
        enrollment->insertStateTableEntry(flow);
    }
    else
        EV << "DAFEnrollmentListener received unknown object!" << endl;
}

void LisDAFEnrollmentStartEnrollReq::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    CDAPMessage* msg = dynamic_cast<CDAPMessage*>(obj);
    if (msg)
        enrollment->receiveStartEnrollmentRequest(msg);
    else
        EV << "Received not a CDAPMessage!" << endl;
}

void LisDAFEnrollmentStartEnrollRes::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    CDAPMessage* msg = dynamic_cast<CDAPMessage*>(obj);
    if (msg)
        enrollment->receiveStartEnrollmentResponse(msg);
    else
        EV << "Received not a CDAPMessage!" << endl;
}

void LisDAFEnrollmentStopEnrollReq::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    CDAPMessage* msg = dynamic_cast<CDAPMessage*>(obj);
    if (msg)
        enrollment->receiveStopEnrollmentRequest(msg);
    else
        EV << "Received not a CDAPMessage!" << endl;
}

void LisDAFEnrollmentStopEnrollRes::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    CDAPMessage* msg = dynamic_cast<CDAPMessage*>(obj);
    if (msg)
        enrollment->receiveStopEnrollmentResponse(msg);
    else
        EV << "Received not a CDAPMessage!" << endl;
}

void LisDAFEnrollmentStopOperationReq::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    CDAPMessage* msg = dynamic_cast<CDAPMessage*>(obj);
    if (msg)
        enrollment->receiveStartOperationRequest(msg);
    else
        EV << "Received not a CDAPMessage!" << endl;
}

void LisDAFEnrollmentStartOperationRes::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    CDAPMessage* msg = dynamic_cast<CDAPMessage*>(obj);
    if (msg)
        enrollment->receiveStartOperationResponse(msg);
    else
        EV << "Received not a CDAPMessage!" << endl;
}

void LisDAFEnrollmentConResPosi::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    CDAPMessage* msg = dynamic_cast<CDAPMessage*>(obj);
    if (msg)
        enrollment->receivePositiveConnectResponse(msg);
    else
        EV << "Received not a CDAPMessage!" << endl;
}

void LisDAFEnrollmentConResNega::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    CDAPMessage* msg = dynamic_cast<CDAPMessage*>(obj);
    if (msg)
        enrollment->receiveNegativeConnectResponse(msg);
    else
        EV << "Received not a CDAPMessage!" << endl;
}

void LisDAFEnrollmentConReq::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj) {
    CDAPMessage* msg = dynamic_cast<CDAPMessage*>(obj);
    if (msg)
        enrollment->receiveConnectRequest(msg);
    else
        EV << "Received not a CDAPMessage!" << endl;
}

void LisDAFEnrollmentRequest::receiveSignal(cComponent* src, simsignal_t id,
        long obj) {
    //TODO: add checks
    enrollment->checkEnrolled();
}
