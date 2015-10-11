//
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
 * @file RIBd.cc
 * @author Vladimir Vesely (ivesely@fit.vutbr.cz)
 * @date Apr 30, 2014
 * @brief Kind of a Notification Board for DIF
 * @detail
 */

#include "RIBd.h"

const char* MSG_CONGEST         = "Congestion";

const char* MSG_ROUTINGUPDATE   = "RoutingUpdate";

const char* PAR_USEFANOTIF          = "useFANotifier";
const char* PAR_USEENROLLNOTIF      = "useEnrollmentNotifier";
//const char* MSG_ENROLLMENT      = "Enrollment";

Define_Module(RIBd);

void RIBd::initialize() {
    //Init signals and listeners
    initSignalsAndListeners();

    //Init MyAddress
    initMyAddress();

    //Pointers
    initPointers();
}

void RIBd::initPointers() {
    FANotif = dynamic_cast<FANotifierBase*>( getModuleByPath("^.faNotifier") );
    if (FANotif) {
        useFANotifier = true;
        this->par(PAR_USEFANOTIF) = true;
    }
    EnrollNotif = dynamic_cast<EnrollmentNotifierBase*>( getModuleByPath("^.enrollmentNotifier") );
    if (EnrollNotif) {
        useEnrollmentNotifier = true;
        this->par(PAR_USEENROLLNOTIF) = true;
    }

}

void RIBd::handleMessage(cMessage *msg) {

}

void RIBd::receiveData(CDAPMessage* msg) {
    Enter_Method("receiveData()");
    //std::string xxx = FANotif->getFullPath();

    //EnrollmentNotifier processing
    if (useEnrollmentNotifier && EnrollNotif->isMessageProcessable(msg)) {
        EnrollNotif->receiveMessage(msg);
    }
    //FANotifier processing
    else if (useFANotifier && FANotif->isMessageProcessable(msg)) {
        FANotif->receiveMessage(msg);
    }
    //TODO: Vesely - Hardcoded message processing should be removed!
    //M_WRITE_Request
    else if (dynamic_cast<CDAP_M_Write*>(msg)) {
        processMWrite(msg);
    }
    //M_START_Request
    else if (dynamic_cast<CDAP_M_Start*>(msg)) {
        processMStart(msg);
    }
    //delete msg;
}

void RIBd::initSignalsAndListeners() {
    cModule* catcher1 = this->getParentModule();
    cModule* catcher2 = this->getModuleByPath("^.^");
    //cModule* catcher3 = this->getModuleByPath("^.^.^");

    //Signals that this module is emitting
    sigRIBDSendData      = registerSignal(SIG_RIBD_DataSend);

    // sigRIBDFwdUpdateRecv = registerSignal(SIG_RIBD_ForwardingUpdateReceived);
    sigRIBDRoutingUpdateRecv = registerSignal(SIG_RIBD_RoutingUpdateReceived);
    sigRIBDCongNotif     = registerSignal(SIG_RIBD_CongestionNotification);


    //Signals that this module is processing
    lisRIBDRcvData = new LisRIBDRcvData(this);
    catcher1->subscribe(SIG_CDAP_DateReceive, lisRIBDRcvData);

    lisRIBDRoutingUpdate = new LisRIBDRoutingUpdate(this);
    catcher2->subscribe(SIG_RIBD_RoutingUpdate, lisRIBDRoutingUpdate);

    lisRIBDCongNotif = new LisRIBDCongesNotif(this);
    catcher2->subscribe(SIG_RA_InvokeSlowdown, lisRIBDCongNotif);
}

void RIBd::signalizeSendData(CDAPMessage* msg) {
    Enter_Method("signalizeSendData()");

    //Check dstAddress
    if (msg->getDstAddr() == Address::UNSPECIFIED_ADDRESS) {
        EV << "Destination address cannot be UNSPECIFIED!" << endl;
        return;
    }

    msg->setBitLength(msg->getBitLength() + msg->getHeaderBitLength());
    //Pass message to CDAP
    EV << "Emits SendData signal for message " << msg->getName() << endl;
    emit(sigRIBDSendData, msg);
}

void RIBd::sendCongestionNotification(PDU* pdu) {
    Enter_Method("sendCongestionNotification()");

    //Prepare M_START ConDescr
    CDAP_M_Start* mstarcon = new CDAP_M_Start(MSG_CONGEST);
    CongestionDescriptor* conDesc = new CongestionDescriptor(pdu->getConnId().getDstCepId(), pdu->getConnId().getSrcCepId(), pdu->getConnId().getQoSId());
    //Prepare object
    std::ostringstream os;
    os << conDesc->getCongesDescrName();
    object_t condesobj;
    condesobj.objectClass = conDesc->getClassName();
    condesobj.objectName = os.str();
    condesobj.objectVal = conDesc;
    //TODO: Vesely - Assign appropriate values
    condesobj.objectInstance = VAL_DEFINSTANCE;
    mstarcon->setObject(condesobj);

    //Generate InvokeId
    mstarcon->setInvokeID(DONTCARE_INVOKEID);

    //Append src/dst address for RMT "routing"
    mstarcon->setSrcAddr(pdu->getDstAddr());
    mstarcon->setDstAddr(pdu->getSrcAddr());

    //Send it
    signalizeSendData(mstarcon);
}

void RIBd::receiveRoutingUpdateFromRouting(IntRoutingUpdate * info)
{
    EV << getFullPath() << " Forwarding update to send to " << info->getDestination();

    /* Emits the CDAP message. */

    CDAP_M_Write * cdapm = new CDAP_M_Write(MSG_ROUTINGUPDATE);
    std::ostringstream os;
    object_t flowobj;

    /* Prepare the object to send. */

    os << "RoutingUpdateTo" << info->getDestination();

    flowobj.objectClass = info->getClassName();
    flowobj.objectName  = os.str();
    flowobj.objectVal   = info;
    //TODO: Vesely - Assign appropriate values
    flowobj.objectInstance = VAL_DEFINSTANCE;

    cdapm->setObject(flowobj);

    //TODO: Vesely - Work more on InvokeId

    /* This message will be sent to... */
    cdapm->setDstAddr(info->getDestination());

    /* Finally order to send the data... */
    signalizeSendData(cdapm);
}

void RIBd::signalizeCongestionNotification(CongestionDescriptor* congDesc) {
    EV << "Emits CongestionNotification" << endl;
    emit(sigRIBDCongNotif, congDesc);
}

void RIBd::processMStart(CDAPMessage* msg) {
    CDAP_M_Start* msg1 = check_and_cast<CDAP_M_Start*>(msg);

    EV << "Received M_Start";
    object_t object = msg1->getObject();
    EV << " with object '" << object.objectClass << "'" << endl;

    //CongestionNotification CongestDescr
    if (dynamic_cast<CongestionDescriptor*>(object.objectVal)) {
        CongestionDescriptor* congdesc = (check_and_cast<CongestionDescriptor*>(object.objectVal))->dup();
        congdesc->getConnectionId().swapCepIds();
        EV << "\n===========\n" << congdesc->getConnectionId().info();
        signalizeCongestionNotification(congdesc);
    }
}

void RIBd::processMWrite(CDAPMessage* msg)
{
    CDAP_M_Write * msg1 = check_and_cast<CDAP_M_Write *>(msg);

    EV << "Received M_Write";
    object_t object = msg1->getObject();
    EV << " with object '" << object.objectClass << "'" << endl;

    if (dynamic_cast<IntRoutingUpdate *>(object.objectVal))
    {
        IntRoutingUpdate * update = (check_and_cast<IntRoutingUpdate *>(object.objectVal));

        /* Signal that an update obj has been received. */
        emit(sigRIBDRoutingUpdateRecv, update);
    }
}

