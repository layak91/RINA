//
// Copyright � 2014 PRISTINE Consortium (http://ict-pristine.eu)
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
/**
 * @file RIBd.cc
 * @author Vladimir Vesely (ivesely@fit.vutbr.cz)
 * @date Apr 30, 2014
 * @brief Kind of a Notification Board for DIF
 * @detail
 */

#ifndef RIBDAEMON_H_
#define RIBDAEMON_H_

//Standard libraries
#include <omnetpp.h>
//RINASim libraries
#include "RIBdBase.h"
#include "Flow.h"
#include "CDAPMessage_m.h"
#include "ExternConsts.h"
#include "RIBdListeners.h"
#include "RINASignals.h"

class RIBd : public RIBdBase {
  public:
    virtual void sendCreateRequestFlow(cObject* obj);
    virtual void receiveData(cObject* obj);

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    void initCdapBindings();
    void initSignalsAndListeners();

    //Signals
    simsignal_t sigRIBDAllocReq;

    //Listeners
    LisRIBDRcvData* lisRIBDRcvData;
    LisRIBDCreReq* lisRIBDCreReq;

    void signalizeSendData(CDAPMessage* msg);

};

#endif /* RIBDAEMON_H_ */