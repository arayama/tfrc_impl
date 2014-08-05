/* This header file includes data types, parameters and variables used
 * by the cliet program. */

#include <sys/time.h>
#include "tcp.h"
#include <signal.h>


#define CNTRLMSGSIZE 14
#define ACKMSGSIZE 36
#define DATAHEADERSIZE 24
#define MAXSEQ 1000
#define true 1
#define false 0
#define MAXINITTRY 10
#define t_mbi 64.0 // t_mbi  = 64 seconds in usec
#define MEG 1000000.0
#define WINDOWSIZE 4
#define TIMESTAMPWINDOW 10




typedef enum {CLIENT_INIT,CLIENT_START,CLIENT_STOP} client_status; // possible client state


struct controlMsg{
	char *controlmessage;
	unsigned short 	*msgLength;
	char *msgType;
	char *msgCode;
	unsigned int *cxID;
	unsigned int *sequenceNum;
	unsigned short *sendMsgSize;
};

struct dataMsg{
	char* datamessage;
	unsigned short *msgLength;
	char *msgType;
	char *msgCode;
	unsigned int *cxID;
	unsigned int *sequenceNum;
	double *timeStamp;
	unsigned int *senderRttEst;
	char* msg;
	unsigned int dataMsgLen;
};

struct ackMsg{
	char *ackmessage;
	unsigned short *msgLength;
	char *msgType;
	char *msgCode;
	unsigned int *cxID;
	unsigned int *ackNum;
	double *timeStamp;
	unsigned int *t_Delay;
	unsigned int *lossEventRate;
	unsigned int *receiveRate;
	unsigned int *seqnumrecvd;
};

struct clientPrms{

        /****SOCKET STUFF******/
		int sock;                        /* Socket descriptor */
		struct sockaddr_in ServAddr; /* Echo server address */
		struct sockaddr_in ClientAddr;
		unsigned int ClientAddrLen;
		unsigned int ServAddrLen;
		unsigned short ServPort;     /* Echo server port */
		unsigned short ClientPort;
		char *servIP;                    /* IP address of server */
		struct hostent *thehost;	     /* Hostent from gethostbyname() */
		int connectionID;

        /****TFRC Parameters******/
		double X_trans; // transmit rate in bytes/s
		double tld; //  time since last doubled
		double X_calc;
		double X_recv; //  rate seen by receiver
		int s_msgSize ; // packetsize in bytes
		double R_rtt; // RTT in microseconds
		double R_sample; // interim calculation of RTT
		float p; // loss event rate
		float simulatedLossRate;
		double maxAllowedThroughput; //  user set max allowed 
		double timebetnPackets; // packet scheduling time duration
		double t_RTO; //  tcp retransmission timeout default = 4 * rtt in microsec
		double t_recvdata; //  time stamp contanined in the ACK 
        	double t_now; // current time
       	 	double t_delay; // t_delay contained in ACK
        	unsigned short b; //  number of packets ack'ed by TCP ACK, default to 1
        	double latestPktTimestamp;


        	/***** FINAL  RESULT DISPLAY PARAMETERS*********/
	
        	double numSent;
        	double numReceived;
        	double sessionTime;
        	double avgThroughput;
       	 	double avgLossEvents;
        	double lossEventCounter;
        	double twoSecCounter;
        	struct sigaction displaytimer;
		double numDropped;
			
        
        	/*****TEMP VARIABLES****/
		unsigned int cntrlTimeout; //  timeout value for control message , default 10 sec
		unsigned int cntrlTimeoutCounter; //  counts till 10 successive timeouts for control message and terminates
		double noFeedbackTimer; // start of no feedback interval
		unsigned int lastAckreceived; //  stores the last perfectly received ack number
		unsigned int sequencenum;
		int feedbackRecvd; //  a flag to hint of a feedbackreceived
		int alarmtimeout; //  flag to indicate timer runout
		int sendSTOP; // to be used when Ctrl +  C is issued
		int expectedACK ; // seq num of expected ack
		int window;
		double timestore[TIMESTAMPWINDOW]; // for storing last 10 Time stamps.

};



int makecntrlmsg(unsigned short msgSize);
void initializedatamsg(unsigned short msgSize, unsigned int seqnum);
void setupackmsg();
void initializeparameters();
void newsendingrate();


void DieWithError(char *errorMessage);


