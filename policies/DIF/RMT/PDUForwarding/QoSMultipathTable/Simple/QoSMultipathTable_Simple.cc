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

#include "QoSMultipathTable_Simple.h"
#include <sstream>
#include <algorithm>
#include <vector>


Register_Class(QoSMultipathTable_Simple::QoSMultipathTable_Simple);

namespace QoSMultipathTable_Simple {

using namespace std;
using namespace QoSMultipathTable;


cEntry::cEntry(): p(nullptr), t(0) {}


void QoSMultipathTable_Simple::onMainPolicyInit() {
    cleanCache_t = par("cleanCache_t").doubleValue();
    if(cleanCache_t > 0) {
        scheduleAt(simTime() + cleanCache_t, new cMessage("Clean Forwarding Cache"));
    }
}

vector<RMTPort * > QoSMultipathTable_Simple::lookup(const PDU * pdu){
    RMTPort * next = nullptr;
    string dstAddr = pdu->getDstAddr().getIpcAddress().getName();

    cEntry * e = &cache[dstAddr][pdu->getConnId().getDstCepId()];

    next = e->p;
    if(next == nullptr) {
        next = portLookup(dstAddr, pdu->getConnId().getQoSId());
        e->p = next;//Port inserted in cache
        e->reqBW = QoS_BWreq[pdu->getConnId().getQoSId()];
        string aux = pdu->getConnId().getQoSId();
        BWControl[next].bw += QoS_BWreq[pdu->getConnId().getQoSId()];
    }

    else//just for debug
    {

    }
    vector<RMTPort *> ret;
    if(next != nullptr) {
        ret.push_back(next);
        e->t = simTime();
    } else {
        BWControl[(cache[dstAddr][pdu->getConnId().getDstCepId()]).p].bw -= (cache[dstAddr][pdu->getConnId().getDstCepId()]).reqBW;
        cache[dstAddr].erase(pdu->getConnId().getDstCepId());
        if(cache[dstAddr].empty()) {
            cache.erase(dstAddr);
        }
    }


    return ret;
}

//Not Used
vector<RMTPort * > QoSMultipathTable_Simple::lookup(const Address &dst, const std::string& qos){
    RMTPort * next = nullptr;
    vector<RMTPort *> ret;

    string dstAddr = dst.getIpcAddress().getName();

    if(dstAddr == "") { return ret; }

    next = portLookup(dstAddr, qos);

    if(next != nullptr) { ret.push_back(next); }


    return ret;
}

RMTPort * QoSMultipathTable_Simple::portLookup(const string& dst, const string& qos) {
    if(dst == "") {
        return nullptr;
    }
    int reqBW = QoS_BWreq[qos];
    vector<entryT> * entries = & table[dst];

    long totalBW = 0;
    for (auto it : *entries)
    {
       totalBW += BWControl[it.p].bw;
    }

    vector<entryT> possibles;

    for( entryT & e : *entries) {
        UsedBW* BW = &BWControl[(e.p)];
        if((e.BW - BW->bw) >= reqBW) {
            possibles.push_back(e);
        }
    }

    if(possibles.empty()) {
        if(totalBW >= reqBW){
            return rerouteFlows(*entries, dst, reqBW);
        }
        else{
            return nullptr;
        }
    }

    int maxBW=0;
    entryT * exit = nullptr;
    for( entryT & e : possibles) {
        UsedBW* BW = &BWControl[(e.p)];
        if(maxBW < e.BW - BW->bw) {
            maxBW = e.BW - BW->bw;
            exit = &e;
        }
    }
    if (exit != nullptr)
    {
        return exit->p;
    }
    else
    {
        error("Entries found but none returned");
        return nullptr;
    }
}

RMTPort * QoSMultipathTable_Simple::rerouteFlows(const vector<entryT>& ports, const string& dst, const int& bw){

    vector<entryT> AvBWports; //ports with Aviable BandWith


    for(auto it : ports)
    {
        entryT e(it.p,BWControl[it.p].bw);
        AvBWports.push_back(e);
    }

    sort(AvBWports.begin(), AvBWports.end(), compare);

    for(auto p: AvBWports){
        RerouteInfo info(AvBWports);

        for(auto it : cache[dst]){
            if(it.second.p==p.p)
            {
                //if flow can be rerouted
                entryT * auxPort = new entryT(NULL, 0);
                for (auto it2 : info.ports){
                    if ((it2.second > it.second.reqBW) && (it2.second > auxPort->BW)){
                        auxPort->p = it2.first;
                        auxPort->BW = it2.second;
                    }
                }
                info.addMov(p.p, auxPort->p, it.first, it.second.reqBW);
                info.ports[p.p]+=it.second.reqBW;
                info.ports[auxPort->p]-=it.second.reqBW;

                if(info.ports[p.p] >= bw){
                    AplyReroute(info, dst);
                    return p.p;

                }
            }
        }
        info.movements.clear();
        info.ports.clear();
    }
    return nullptr;

}

void QoSMultipathTable_Simple::AplyReroute(const RerouteInfo &info, const string& dst){

    for (auto it : info.movements){
        cache[dst][it.flow].p=it.dst;
        BWControl[it.org].bw += it.reqBW;
        BWControl[it.dst].bw -= it.reqBW;
    }
}

bool QoSMultipathTable_Simple::compare(const entryT &i, const entryT &j){
    return(i.BW > j.BW);//Decresing order
}

void QoSMultipathTable_Simple::onSetPort(RMTPort * p, const int bw) {
    if(bw > 0) {
        return;
    }

    for(auto it = cache.begin(); it!= cache.end();) {
        for(auto it2 = it->second.begin(); it2!= it->second.end();) {
            if(it2->second.p == p) {
                auto tIt = it2;
                it2++;
                it->second.erase(tIt);
            } else { it2++; }
        }
        if(it->second.empty()) {
            auto tIt = it;
            it++;
            cache.erase(tIt);
        } else { it++; }
    }
    BWControl.erase(p);
}

void QoSMultipathTable_Simple::preReplacePort(RMTPort * oldP, RMTPort * newP, const int bw) {
    onSetPort(oldP, 0);

    for(auto & dst : table) {
        for(auto & e : dst.second) {
            if(e.p == oldP) { e.p = newP; }
        }
    }
}

void QoSMultipathTable_Simple::handleMessage(cMessage * msg) {
    simtime_t limT = simTime()-2*cleanCache_t;
    for(auto it = cache.begin(); it!= cache.end();) {
        for(auto it2 = it->second.begin(); it2!= it->second.end();) {
            if(it2->second.t < limT) {
                auto tIt = it2;
                it2++;
                BWControl[tIt->second.p].bw -= tIt->second.reqBW;
                it->second.erase(tIt);
            } else { it2++; }
        }
        if(it->second.empty()) {
            auto tIt = it;
            it++;
            cache.erase(tIt);
        } else { it++; }
    }

    scheduleAt(simTime() + cleanCache_t, msg);
}

//Insert/Remove an entry
void QoSMultipathTable_Simple::addReplace(const string &addr, vector<entryT> ports) {
    if(addr == "") {
        return;
    }

    if(ports.empty()) {
        for(auto it = cache[addr].begin(); it!= cache[addr].end();) {
                BWControl[it->second.p].bw -= it->second.reqBW;
        }
        cache.erase(addr);
        table.erase(addr);
        return;
    }

    for(entryT & e : table[addr]) {
        bool found = false;
        for(entryT & n : ports) {
            if(e.p == n.p) {
                found = true;
            }
        }
        if(!found) {
            for(auto it2 = cache[addr].begin(); it2!= cache[addr].end();) {
                if(it2->second.p == e.p) {
                    auto tIt = it2;
                    it2++;
                    BWControl[tIt->second.p].bw -= tIt->second.reqBW;
                    cache[addr].erase(tIt);
                } else {
                    it2++;
                }
            }
        }
    }
    if(cache[addr].empty()) {
        cache.erase(addr);
    }

    table[addr] = ports;
}


// Returns a representation of the Forwarding Knowledge
string QoSMultipathTable_Simple::toString(){
    std::ostringstream os;
    os << this->getFullName()<<endl;
    for(const auto & dst : table) {
        os << "\t" << dst.first << "  ->  " << endl;
        for(auto e : dst.second) {
            os << "\t\t" << e.BW << " : "<< e.p->getFullPath() << endl;
        }
    }

    return os.str();
}

void QoSMultipathTable_Simple::finish(){
    if(par("printAtEnd").boolValue()){
        EV << "-----------------" << endl;
        EV << "Forwarding table::" << endl;
        EV << toString() <<endl;
        EV << "-----------------" << endl;
    }
}

}
