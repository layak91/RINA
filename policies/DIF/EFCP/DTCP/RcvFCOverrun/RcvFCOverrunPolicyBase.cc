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
 * @file RcvFCOverrunPolicyBase.cc
 * @author Marcel Marek (imarek@fit.vutbr.cz)
 * @date Jun 22, 2015
 * @brief
 * @detail
 */

#include "RcvFCOverrunPolicyBase.h"
#include "DTP.h"
RcvFCOverrunPolicyBase::RcvFCOverrunPolicyBase()
{


}

RcvFCOverrunPolicyBase::~RcvFCOverrunPolicyBase()
{

}

void RcvFCOverrunPolicyBase::defaultAction(DTPState* dtpState, DTCPState* dtcpState)
{

  //TODO A! Implement the default action, the text below is from SndFCOverrun policy
  DTP* dtp = (DTP*)getModuleByPath((std::string(".^.") + std::string(MOD_DTP)).c_str());
  /* Default */
//  dtcpState->pushBackToClosedWinQ((DataTransferPDU*) dtpState->getCurrentPdu());
//  //Block further Write API calls on this port-id
//  dtp->notifyStopSending();
  /* End default */
}
