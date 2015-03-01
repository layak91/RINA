//
// Copyright © 2014 - 2015 PRISTINE Consortium (http://ict-pristine.eu)
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

#ifndef __RINA_RMTPORT_H_
#define __RINA_RMTPORT_H_

#include <omnetpp.h>
#include "RINASignals.h"
#include "Flow.h"
#include "CDAPMessage_m.h"
#include "RMTQueue.h"
#include "QueueIDGenBase.h"

class RMTPort : public cSimpleModule
{
    /* tight coupling for management purposes */
    friend class RA;
    friend class RMTModuleAllocator;

  public:

    /**
     * Returns the port state (ready to receive data/busy).
     *
     * @return port state
     */
    bool isReady();

    /**
     * Marks the port as ready to receive data.
     */
    void setReady();

    /**
     * Marks the port as busy (e.g. when sending data through it).
     */
    void setBusy();

    /**
     * Marks the port as blocked (e.g. when (N-1)-EFCPI isn't keeping up).
     */
    void blockOutput();

    /**
     * Unmarks the port as blocked (e.g. when (N-1)-EFCPI is keeping up again).
     */
    void unblockOutput();

    /**
     * Returns the (N-1)-flow this port is assigned to.
     *
     * @return (N-1)-flow object
     */
    const Flow* getFlow() const;

    /**
     * Returns the port's management queue.
     * Note: this will go away soon when we start dedicating entire (N-1)-flows
     *       for management purposes
     *
     * @param type direction of data
     * @return management queue
     */
    RMTQueue* getManagementQueue(RMTQueueType type) const;

    /**
     * Returns the first queue available on this port.
     * Note: this excludes the temporary management queues
     *
     * @param type direction of data
     * @return queue
     */
    RMTQueue* getFirstQueue(RMTQueueType type) const;

    /**
     * Returns the longest queue attached to this port.
     *
     * @param type direction of data
     * @return queue
     */
    RMTQueue* getLongestQueue(RMTQueueType type) const;

    /**
     * Returns the queue that matches given ID.
     *
     * @param type direction of data
     * @param queueId queue ID
     * @return queue
     */
    RMTQueue* getQueueById(RMTQueueType type, const char* queueId) const;

    /**
     * Returns a list of available input queues.
     *
     * @return set of queues
     */
    const RMTQueues& getInputQueues() const;

    /**
     * Returns a list of available output queues.
     *
     * @return set of queues
     */
    const RMTQueues& getOutputQueues() const;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);

  private:
    bool ready;
    bool blocked;
    Flow* flow;
    std::string dstDisplayString;
    double postServeDelay;

    QueueIDGenBase* queueIdGen;

    std::set<cGate*> northOutputGates;
    std::set<cGate*> northInputGates;
    cGate* southInputGate;
    cGate* southOutputGate;
    cChannel* outputChannel;

    RMTQueues outputQueues;
    RMTQueues inputQueues;

    void postInitialize();
    void setFlow(Flow* flow);
    void addInputQueue(RMTQueue* queue, cGate* portGate);
    void addOutputQueue(RMTQueue* queue, cGate* portGate);
    cGate* getSouthInputGate() const;
    cGate* getSouthOutputGate() const;

    void setReadyDelayed();
    void redrawGUI();

    simsignal_t sigRMTPortReady;
    simsignal_t sigStatRMTPortUp;
    simsignal_t sigStatRMTPortDown;
};

typedef std::vector<RMTPort*> RMTPorts;

#endif
