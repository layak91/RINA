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
 * @file DAFEnrollmentListeners.h
 * @author Kamil Jerabek (xjerab18@stud.fit.vutbr.cz)
 * @date Apr 1, 2015
 * @brief DAFEnrollment and CACE listeners
 * @detail
 */


#ifndef DAFENROLLMENTLISTENERS_H_
#define DAFENROLLMENTLISTENERS_H_

#include "DAFEnrollment.h"

class DAFEnrollment;
class DAFEnrollmentListeners : public cListener {
public:
    DAFEnrollmentListeners(DAFEnrollment* nenrollment);
    virtual ~DAFEnrollmentListeners();
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj) {
               EV << "Signal to DAFEnrollment initiated by " << src->getFullPath() << endl;
        }
protected:
    DAFEnrollment* enrollment;
};


class LisDAFEnrollmentAllResPosi : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentAllResPosi(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};


class LisDAFEnrollmentGetFlowFromFaiCreResPosi : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentGetFlowFromFaiCreResPosi(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};


class LisDAFEnrollmentStartEnrollReq : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentStartEnrollReq(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};

class LisDAFEnrollmentStartEnrollRes : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentStartEnrollRes(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};

class LisDAFEnrollmentStopEnrollReq : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentStopEnrollReq(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};

class LisDAFEnrollmentStopEnrollRes : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentStopEnrollRes(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};

class LisDAFEnrollmentStopOperationReq : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentStopOperationReq(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};

class LisDAFEnrollmentStartOperationRes : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentStartOperationRes(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};

class LisDAFEnrollmentConResPosi : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentConResPosi(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};

class LisDAFEnrollmentConResNega : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentConResNega(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};

class LisDAFEnrollmentConReq : public DAFEnrollmentListeners {
public:
    LisDAFEnrollmentConReq(DAFEnrollment* nenrollment): DAFEnrollmentListeners(nenrollment){};
    virtual void receiveSignal(cComponent *src, simsignal_t id,  cObject *obj);
};


#endif /* ENROLLMENTLISTENERS_H_ */
