/**
 * file:	server.c
 * Authors:	Derek Johnson & Anjan Rayamajhi
 * Created:	March 21, 2014
 * Revisions:
 */


#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <string.h>     // for memset()
#include <netinet/in.h> // for in_addr
#include <sys/socket.h> // for socket(), connect(), sendto(), and recvfrom()
#include <arpa/inet.h>  // for sockaddr_in and inet_addr()
#include <stdlib.h>     // for atoi() and exit()
#include <unistd.h>     // for close()


#include "tcp.h"

#define INTERVAL 1

void cleanup(int ignored);
void handle_alarm(int ignored);

client* clnt = NULL;									// Client info structure
int sock;                 						// Socket
int packetsRecvd = 0;									// Total # of packets recvd
double tBytesRecvd = 0.0;							// Total # of bytes recvd
int lost = 0;													// Total packets lost
int acks = 0;													// Counts the # of acks sent
double loss_rate_sum = 0;							// Used for calculating the average loss rate
int recvd = 0;												// Counts # of packets properly recv'd
int start_num = 0;										// Starting sequence #
double start, end;
double rx_rate = 10000;								// In bits per sec
double bits_recvd = 0;								// Used to calculate recv_rate


// Main function and loop for the server to recieve and keep track of
// statistics for each client.
int main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr; 				// Local address
	struct sockaddr_in clnt_addr;					// Client Address info
	unsigned int addr_len;								// Length of clnt_addr
	char buf[MSGMAX];             				// Buffer for echo string
	unsigned short serv_port;     				// Server port
	int numBytesRecvd;             				// Size of received message
	short int *length = (short int*)buf;	// Pointer to message LENGTH field
	char *type = &(buf[2]);								// Pointer to message TYPE field
	char *code = &(buf[3]);								// Pointer to message CODE field
	int *id = (int*)(&(buf[4]));					// Pointer to message CxID field
	int *seq = (int*) (&(buf[8]));				// Pointer to message SEQ# field
	int seq_num;													// Holds the host byte order seq #
	short *size = (short*)(&(buf[12]));   // Ptr to size field in start msg
	//double *time = (double*)(&(buf[12]));	// Pointer to TIMESTAMP field
	int count = 0;												// Counter for Loss Event
	int event = 0;												// Counts the # of loss events to have occurred
	double loss_rate;											// Estimated loss event rate
	double diff;													// Time measurements
	double s_hat = 1000000;								// Average loss interval parameter s_hat
	double s_hat_new = 0;									// Average loss interval parameter s_hat_new
	//double s_hat_used = 1;								// Average loss interval
	int s[9] = {0};												// Holds all of the s(i)'s
	int in_event = 0;											// 1 if a loss event is occurring and 0 otherwise
	double sec1, sec2;
	int first_event = 0;
	int k;
	int last_seq_recvd;


	clnt = (client *)malloc(sizeof(client));
	addr_len = (unsigned int)sizeof(clnt_addr);
	signal(SIGINT, cleanup);
	signal(SIGALRM, handle_alarm);
	


	// Test for correct number of parameters
	if (argc != 2)
	{
		fprintf(stderr,"Usage: %s <Port>\n", argv[0]);
		exit(1);
	}

	// First arg:  local port
	serv_port = atoi(argv[1]);
	printf("Server Port:%d\n",serv_port);
	printf("Size of double:%d\n", (int)sizeof(double));
	printf("Time:%g\n", get_time());    

	// Create socket for sending/receiving datagrams
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		printf("Failure on socket call , errno:\n");

	// Construct local address structure
	memset(&serv_addr, 0, sizeof(serv_addr));      // Zero out structure
	serv_addr.sin_family = AF_INET;                // Internet address family
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
	serv_addr.sin_port = htons(serv_port);         // Local port

	// Bind to the local address
	if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Failure on bind, errno:\n");
	}
	
	//clnt = tcp_accept(sock);
	
	
	/********ACCEPT TCP CONNECTION***********/
	do
	{
		numBytesRecvd = recvfrom(sock, buf, CONTROL_LENGTH, 0, (struct sockaddr *) &clnt_addr, &addr_len);
		if(numBytesRecvd < 0)
		{
			fprintf(stderr, "ERROR: Accepting Connection\n");
			exit(1);
		}
		printf("length:%d\n", ntohs(*length));
		printf("type:%d\n", (int)*type);
		printf("code:%d\n", (int)*code);
		printf("Cxid:%d\n", ntohl(*id));
		printf("Seq#:%d\n", ntohl(*seq));
		printf("size:%d\n", ntohs(*size));
		
	}
	while(*type != CONTROL && *code != START);
	strcpy(clnt->ip, NET_get_ip(&clnt_addr));
	clnt->port = NET_get_port(&clnt_addr);
	clnt->id = ntohl(*id);
	clnt->seq = ntohl(*seq);
	clnt->length = ntohs(*size) + DATA_HEADER_SIZE;
	printf("Client length %d\n", clnt->length);
	start_num = clnt->seq + 1;
	*length = ntohs(CONTROL_LENGTH);
	*type = CONTROL;
	*code = OK;
	*id = ntohl(clnt->id);
	*seq = ntohl(clnt->seq);
	last_seq_recvd = clnt->seq;
	
	sendto(sock, buf, CONTROL_LENGTH, 0, (struct sockaddr *) &(clnt_addr), sizeof(clnt_addr));
	sec1 = get_time();
	printf("START rcvd\n\n");
	/********FINISHED ACCEPTING CLIENT*****/
	
	start = get_time();
	alarm(INTERVAL);
	while(1)
	{
		numBytesRecvd = recvfrom(sock, buf, MSGMAX, 0, (struct sockaddr *) &clnt_addr, &addr_len);
		sec2 = get_time();
		/*
		printf("length:%d\n", ntohs(*length));
		printf("type:%d\n", (int)*type);
		printf("code:%d\n", (int)*code);
		printf("Cxid:%d\n", ntohl(*id));
		printf("Seq#:%d\n", ntohl(*seq));
		*/
		if(numBytesRecvd < 0)
		{
			fprintf(stderr, "ERROR: Receiving Data\n");
			exit(1);
		}
		if(strcmp(NET_get_ip(&clnt_addr), clnt->ip) != 0 || ntohl(*id) != clnt->id || NET_get_port(&clnt_addr) != clnt->port)
		{
			printf("Not the client\n\n");
			continue;
		}
		
		/**************RESPOND TO STOP****************/
		if(*type == CONTROL && *code == STOP)
		{
			/*send OK message*/
			end = sec2;
			printf("Got STOP\n\n");
			*length = htons(CONTROL_LENGTH);
			*type = CONTROL;
			*code = OK;
			*id = htonl(clnt->id);
			*seq = htonl(seq_num);
			sendto(sock, buf, CONTROL_LENGTH, 0, (struct sockaddr *) &clnt_addr, sizeof(clnt_addr));
			/*Exit loop*/
			break;
		}
		/*************RESPOND TO DUPLICATE START*********************/
		if(*type == CONTROL && *code == START)
		{
			*length = ntohs(CONTROL_LENGTH);
			*type = CONTROL;
			*code = OK;
			*id = ntohl(clnt->id);
			*seq = ntohl(clnt->seq);	
			sendto(sock, buf, CONTROL_LENGTH, 0, (struct sockaddr *) &(clnt_addr), sizeof(clnt_addr));
			printf("START recvd\n\n");
			continue;
		}
		
		if(numBytesRecvd != clnt->length)
			continue;
		
		packetsRecvd++;
		tBytesRecvd = tBytesRecvd + numBytesRecvd;
		bits_recvd += numBytesRecvd*8;
		seq_num = ntohl(*seq);
		//printf("-------------------------\n");
		//printf("Expected Seq #: %d\n", clnt->seq);
		//printf("Last Seq #: %d\n", last_seq_recvd);
		//printf("Seq# recvd: %d\n", seq_num);
		//printf("# Lost: %d\n", lost);
		/***************Handle Packet Loss******************/
		if(*type == DATA && seq_num != (clnt->seq + 1))
		{
			//printf("DATA: Unexpected seq#\n\n");
			
			//Count loss for the first time
			if(seq_num > last_seq_recvd)
			{
				lost+= (seq_num - last_seq_recvd - 1);
			}
			else if(seq_num < last_seq_recvd)
			{
				//lost+= (seq_num - last_seq_recvd - 1);
				lost--;
			}
			//Detect loss event	
			if(count < 3)
			{
				count++;
				if(count == 3)
				{
					event++;
					in_event = 1;
					first_event++;
				}
			}
			acks++;
			diff = sec2 - sec1;
			rx_rate = numBytesRecvd*8.0/diff;
			

			last_seq_recvd = seq_num;
			loss_rate_sum += ((unsigned int)floor(loss_rate*1000))/1000.0;
			pack_ack(buf, clnt->id, clnt->seq, loss_rate, rx_rate, seq_num);
			sec1 = get_time();
			sendto(sock, buf, ACK_LENGTH, 0, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr));
		}
		/************************Correct Packet************************/
		else if(*type == DATA && seq_num == (clnt->seq + 1))
		{
			/*Send ACK and update clnt->seq*/
			//printf("DATA: Correct seq#\n\n");
			clnt->seq = seq_num;
			
			
			last_seq_recvd = seq_num;
			recvd++;
			s[0]++;
			/*Update Average loss interval*/
			if(in_event)
			{
				//printf("In event\n");
				//getchar();
				if(first_event == 1)
				{
					for(k = 1; k <9; k++)
						s[k] = s[0];
					first_event++;
					//printf("s[0]: %d\n", s[0]);
				}
				in_event = 0;
				shift_s_values(s);
				s_hat = calc_s_hat(&(s[1]));
				//printf("s_hat: %g\n", s_hat);
			}
			else
			{
				if(seq_num < last_seq_recvd)
				{
					lost+= (seq_num - last_seq_recvd);
				}
			}
			s_hat_new = calc_s_hat(s);
			if(s_hat_new > s_hat)
				s_hat = s_hat_new;
			loss_rate = (double)(1.0/s_hat);
			loss_rate_sum += ((unsigned int)floor(loss_rate*1000))/1000.0;
			diff = sec2 -sec1;
			rx_rate = (numBytesRecvd*8.0)/diff;
			acks++;
			//printf("Loss Rate: %g rx_rate: %u\n", floor(loss_rate*1000)/1000.0, (unsigned int)floor(rx_rate*1000));
			pack_ack(buf, clnt->id, clnt->seq, loss_rate, rx_rate, seq_num);
			sec1 = get_time();
			sendto(sock, buf, ACK_LENGTH, 0, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr));
			count = 0;
		}
		else
		{
			/*Most likely ACK clnt->seq*/
			//pack_ack(buf, clnt->id, clnt->seq, loss_rate, rx_rate, seq_num);
			//sendto(sock, buf, ACK_LENGTH, 0, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr));
			printf("ERROR!!!!!!\n");
			printf("length:%d\n", ntohs(*length));
			printf("type:%d\n", (int)*type);
			printf("code:%d\n", (int)*code);
			printf("Cxid:%d\n", ntohl(*id));
			printf("Seq#:%d\n", ntohl(*seq));
			exit(1);
		}
	}
	cleanup(1);
	
	return 0;
}

void cleanup(int ignored)
{
	printf("\n");
	end = get_time();
	free(clnt);
	close(sock);
	printf("Packets Recvd: %d Bytes Recvd: %lg\n", packetsRecvd, tBytesRecvd);
	printf("Throughtput: %g\n", tBytesRecvd*8/(end-start));
	printf("Acks: %d\n", acks);
	//printf("Packets dropped: %d\n", lost);
	//printf("Packet loss rate: %g\n", lost*1.0/(packetsRecvd+lost));
	printf("Average loss event rate: %g\n", loss_rate_sum*1.0/acks);
	exit(0);
	return;
}

void handle_alarm(int ignored)
{
	rx_rate = bits_recvd/INTERVAL;
	if(rx_rate < 10000)
	{
		rx_rate = 10000;
	}
	bits_recvd = 0;
	alarm(INTERVAL);
	return;
}

	
	
