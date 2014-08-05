

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <stdio.h>


#include "clientresources.h"



int makecntrlmsg(unsigned short msgSize)
{
	// setup the control message
	cntrl.controlmessage = (char *) calloc(CNTRLMSGSIZE,sizeof(char));
	cntrl.msgLength = (unsigned short *) cntrl.controlmessage;
	cntrl.msgType   = (char *) &(cntrl.controlmessage[2]);
	cntrl.msgCode   = (char *) &(cntrl.controlmessage[3]);
	cntrl.cxID      = (unsigned int *) &(cntrl.controlmessage[4]);
	cntrl.sequenceNum = (unsigned int*) &(cntrl.controlmessage[8]);
	cntrl.sendMsgSize = (unsigned short*) &(cntrl.controlmessage[12]);

	*(cntrl.msgLength) = htons(CNTRLMSGSIZE);
	*(cntrl.msgType) = CONTROL;
	*(cntrl.msgCode) = START;
	*(cntrl.cxID) = htonl(OK); //  by default , unused
	*cntrl.sequenceNum = htonl(rand()%MAXSEQ); //  TODO:  limit the max to avoid overflow
	*(cntrl.sendMsgSize) = htons(msgSize);
	return ntohl(*(cntrl.sequenceNum));
}

void initializedatamsg(unsigned short msgSize, unsigned int seqnum)
{
	data.dataMsgLen = (msgSize+DATAHEADERSIZE);
	data.datamessage = (char *) calloc(data.dataMsgLen,sizeof(char));
	data.msgLength = (unsigned short*) data.datamessage;
	data.msgType   = (char *) &(data.datamessage[2]);
	data.msgCode   = (char *) &(data.datamessage[3]);
	data.cxID      = (unsigned int *) &(data.datamessage[4]);
	data.sequenceNum = (unsigned int*) &(data.datamessage[8]);
	data.timeStamp = (double*) &(data.datamessage[12]);
	data.senderRttEst = (unsigned int*) &(data.datamessage[20]);
	data.msg = (char*) &(data.datamessage[24]);

	*data.msgLength = htons(data.dataMsgLen);
	*data.msgType = DATA;
	*data.msgCode = OK;
	*data.cxID = htonl(OK); //  by default , unused
	*data.sequenceNum = htonl(seqnum) ;

	 /* timestamp and sendrtt request not added yet */
}

void setupackmsg()
{
	ack.ackmessage = (char *) calloc(ACKMSGSIZE,sizeof(char));
	ack.msgLength = (unsigned short*) ack.ackmessage;
	ack.msgType   = (char *) &(ack.ackmessage[2]);
	ack.msgCode   = (char *) &(ack.ackmessage[3]);
	ack.cxID      = (unsigned int *) &(ack.ackmessage[4]);
	ack.ackNum = (unsigned int*) &(ack.ackmessage[8]);
	ack.timeStamp = (double*) &(ack.ackmessage[12]);
	ack.t_Delay = (unsigned int*) &(ack.ackmessage[20]);
	ack.lossEventRate = (unsigned int*) &(ack.ackmessage[24]);
	ack.receiveRate = (unsigned int*) &(ack.ackmessage[28]);
	ack.seqnumrecvd = (unsigned int*) &(ack.ackmessage[32]);
}

void initializeparameters()
{
    tfrc_client.cntrlTimeout = 1; // cntrl packets timeout = 10 sec
    tfrc_client.cntrlTimeoutCounter = 0; // at most 10 timeout retries for initial sync

    tfrc_client.X_trans = tfrc_client.s_msgSize*8.0; // msgsize bytes * 8 bits per second
    tfrc_client.tld = 0;
    tfrc_client.t_RTO = 2 * MEG; // 2 seconds in usec
    tfrc_client.b = 1; //  one ack per packet
    tfrc_client.timebetnPackets = tfrc_client.s_msgSize *8.0 / tfrc_client.X_trans; // his is the initial time betn packets
    //tfrc_client.t_now = &tfrc_client.T_NOW;
    tfrc_client.t_now = get_time();
    tfrc_client.R_rtt = 0.0; 
    
    tfrc_client.window = 0; 
    
    tfrc_client.numSent=0;
    tfrc_client.numReceived=0;
    tfrc_client.lossEventCounter=0;
    tfrc_client.numDropped = 0;
    
    
    
}

void newsendingrate()
{
    double fq;
    
    if(tfrc_client.p>0)
	{
		fq = tfrc_client.R_rtt*sqrt(2*tfrc_client.b*tfrc_client.p/3) + (tfrc_client.t_RTO*3*sqrt(3*tfrc_client.b*tfrc_client.p/8)*tfrc_client.p*(1+32*tfrc_client.p*tfrc_client.p));	
			
        tfrc_client.X_calc = tfrc_client.s_msgSize *8.0 * MEG / fq; // X_calc should be in [ bps]
        tfrc_client.X_trans = fmax(fmin(fmin(tfrc_client.X_calc,2*tfrc_client.X_recv),tfrc_client.maxAllowedThroughput),tfrc_client.s_msgSize*8.0/t_mbi);
    }
    else
        if(tfrc_client.t_now-tfrc_client.tld >= tfrc_client.R_rtt)
        {
            tfrc_client.X_trans = fmax(fmin(fmin(2*tfrc_client.X_trans,2*tfrc_client.X_recv),tfrc_client.maxAllowedThroughput),tfrc_client.s_msgSize*8.0/tfrc_client.R_rtt);
            tfrc_client.tld = tfrc_client.t_now;
        }
}


void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
