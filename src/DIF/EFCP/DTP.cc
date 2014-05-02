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

#include "DTP.h"

DTP::DTP()
{
  // TODO Auto-generated constructor stub

}
DTP::~DTP()
{
  // TODO Auto-generated destructor stub
}

void DTP::setConnId(const ConnectionId& connId)
{
  this->connId = connId;
}

void DTP::handleMessage(cMessage *msg)
{

}

bool DTP::write(int portId, unsigned char* buffer, int len)
{

  cancelEvent(this->senderInactivity);

  this->delimit(buffer, len);
  /* Now the data from buffer are copied to SDUs so we can free the memory */
  free(buffer);

  this->generatePDUs();

  /* Iterate over generated PDUs and decide if we can send them */
  this->trySendGenPDUs();

  return true;
}

/**
 * Delimits @param len bytes of buffer into User-data parts and put them on generated PDU
 * @param buffer pointer to incoming data buffer
 * @param len size of incoming data
 * @return number of created SDUs
 */
int DTP::delimit(unsigned char *buffer, unsigned int len)
{

  unsigned int offset = 0, size = 0, counter = 0;

  do
  {
    if (len - offset > state.getMaxFlowSduSize())
    {
      size = state.getMaxFlowSduSize();
    }
    else
    {
      size = len;
    }
    SDU *sdu = new SDU();
    sdu->setUserData(&buffer[offset], size);

    sduQ.push_back(sdu);

    offset += size;
    counter++;

  } while (offset < len);

  //TODO A1 If len is zero then create empty SDU?
//  if(len == 0){
//    SDU sdu;
//    sduQ.push(sdu);
//    counter = 1;
//  }
  return counter;
}

/**
 * This method takes all SDUs from sduQ and generates PDUs by adding appropriate header content
 */
void DTP::generatePDUs()
{

  DataTransferPDU* dataPDU = new DataTransferPDU();

  dataPDU->setConnId((const ConnectionId) (*connId.dup()));
  //setDestAddr... APN
  //setSrcAddr ... APN

  //invoke SDU protection so we don't have to bother with it afterwards
  for(std::vector<SDU*>::iterator it = sduQ.begin(); it != sduQ.end(); ++it){
    sduProtection(*it);
  }

  SDU *sdu = NULL;
  DataTransferPDU* genPDU = dataPDU->dup();
  bool fragment = false;
  int delimitFlags = 0;
  do
  {
    //This method fetches next SDU if current one has been put to some PDU(s) (offset = size)
    this->getSDUFromQ(sdu);

    unsigned int copySize = 0;
    /* TODO We should take into account also SDUDelimiterFlags etc */
    unsigned int pduAvailSize = state.getMaxFlowPduSize() - genPDU->getPduLen();
    /* if the rest of the SDU is bigger than empty space in PDU then fill-up PDU */
    copySize = (sdu->getRestSize() >= pduAvailSize) ? pduAvailSize : sdu->getRestSize();


    if (genPDU->getPduLen() == PDU_HEADER_LEN)    {
      //PDU is empty
      //set noLength flag
      delimitFlags |= 0x04;
      if (sdu->getRestSize() > pduAvailSize){
        //(rest of) SDU is bigger than PDU
        //set noLength flag
        delimitFlags |= 0x04;
        if (sdu->getSize() > sdu->getRestSize()){
          //not first segment, something has been read from SDU
          //and since it won't fit cannot be last
          //this is middle segment
          delimitFlags |= 0x00;
        }else{
            //this is first segment of next SDU
            delimitFlags |= 0x01;
        }
      }else{
        //(rest of) SDU is smaller than available space in PDU
        if (sdu->getSize() > sdu->getRestSize()){
          //last fragment of previous SDU
          delimitFlags |= 0x02;
        }else{
          //this is complete SDU
          delimitFlags |= 0x03;
        }
      }

    }else{
      //clear noLength flag
      delimitFlags &= 0xFB;

      if(sdu->getRestSize() > pduAvailSize){
        //(rest of) SDU won't fit into rest of PDU
        if(sdu->getSize() > sdu->getRestSize()){
          //this is not first segment
          if(copySize == sdu->getRestSize()){
            //this is last segment
            delimitFlags |= 0x02;
          }else{
            //this is middle segment
            //not permitted
            throw cRuntimeError("This type of PDU delimiting is not permitted!");
          }
        }else{
          //adding first segment
          delimitFlags |= 0x01;
        }
      }else{
        //(rest of) SDU will fit into rest of PDU
        //complete sdu
        if((delimitFlags && 0x03) == 3){
          //since i'm adding complete SDU, this flag is possible only when adding complete SDU to one (or more)complete SDU
          delimitFlags &= 0xFC;
        }
      }
    }

    //add whole SDU or fragment to PDU
    genPDU->addUserData(sdu->getUserData(copySize), copySize, &fragment);
//    fragment = true;

    if(sdu->getRestSize() == 0){
      delete sduQ.front();
      sduQ.erase(sduQ.begin());
    }

    //if genPDU is full or SDU queue is empty, 'close' PDU and put it to generated PDUs and create new PDU
    if (genPDU->getPduLen() >= this->state.getMaxFlowPduSize() || sduQ.empty())
    {
      //TODO A2 what else to do before sending?
      //what about formating SDUDelimitersFlags in userData?
      genPDU->putDelimitFlags(delimitFlags, fragment);


      //put genPDU to generatedPDUs
      generatedPDUs.push_back(genPDU);

      if (!sduQ.empty())
      {
        genPDU = dataPDU->dup();
        fragment = false;
        //set DRF(false) -> not needed (false is default)
        genPDU->setSeqNum(this->state.getNextSeqNumToSend());
      }
    }

  }while (!sduQ.empty());
}


void DTP::trySendGenPDUs(){

  std::vector<PDU*> postablePDUs;
  std::vector<PDU*>::iterator it;
  for(it = generatedPDUs.begin(); it != generatedPDUs.end(); ++it){

    if(state.isDtcpPresent()){
      if(state.isWinBased() || state.isRateBased()){
//        if((*it)->getSeqNum() <= state.getRi)
      }

    }
  }
}




/**
 * This method calls specified function to perform SDU protection.
 * SDU size will probably change because of the added CRC or whatnot.
 * @param sdu is the SDU being protected eg added CRC or otherwise
 */
void DTP::sduProtection(SDU *sdu){


}
/**
 * This method
 * @para
 */

void DTP::getSDUFromQ(SDU *sdu)
{
  if(sdu == NULL){
    sdu = sduQ.front();
    return;
  }else{
    if (sdu->getRestSize() > 0){
      return;
    }else{
      sdu = sduQ.front();
    }
  }
}


