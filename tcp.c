//tcp.c

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "tcp.h"

// Gets a human readable string representation of the IP
// Parameters:
//   addr - sockaddr containing information about the socket including the ip
char *NET_get_ip(struct sockaddr_in *addr)
{
	char ip[INET6_ADDRSTRLEN];

	if(inet_ntop(AF_INET, &(addr->sin_addr), ip, INET6_ADDRSTRLEN) != NULL)
		return strdup(ip);
	return NULL;
}

// Gets a human readable unsigned short of the PORT
// Parameters:
//   addr - sockaddr containing information about the socket including the port
unsigned short NET_get_port(struct sockaddr_in *addr)
{
	unsigned short port;

	port = htons(addr->sin_port);
	return port;
}


//Packs the header field for an ACKNOWLEDGEMENT packet
//Does all conversions necessary and gets the timestamp
void pack_ack(char buf[], int id, int seq, double drop, double rx, int seq_recvd)
{
	short *l = (short *)buf;
	char *type = &(buf[2]);
	char *code = &(buf[3]);
	int *i = (int *)&(buf[4]);
	int *s = (int *)&(buf[8]);
	double *t = (double *)&(buf[12]);
	int *td = (int *)&(buf[20]);
	int *d = (int *)&(buf[24]);
	int *r = (int *)&(buf[28]);
	int *n = (int *)&(buf[32]);
	*td = 0;
	double time;

	*l = htons(ACK_LENGTH);
	//printf("Length\n");
	//fflush(stdin);
	*type = ACK;
	//printf("TYPE\n");
	//fflush(stdin);
	*code = OK;
	//printf("CODE\n");
	//fflush(stdin);
	*i = htonl(id);
	//printf("ID\n");
	//fflush(stdin);
	*s = htonl(seq);
	//printf("SEQ\n");
	//fflush(stdin);
	time = (get_time())*1000000;
	//printf("Time: %g\n",time);
	//fflush(stdin);
	*t = htond(time);
	//printf("Time\n");
	//fflush(stdin);
	*d = htonl((unsigned int)floor(drop*1000));
	//printf("Drop\n");
	//fflush(stdin);
	*r = htonl((unsigned int)floor((rx*1000)));
	//printf("RX\n");
	//fflush(stdin);
	*n = htonl(seq_recvd);
	return;
}

//Converts 8 bytes from host byte order to network byte order
double htond(double host)
{
	static int is_big = -1;			//1 if Big Endian 0 if Little Endian
	double network;							//Holds network byte order representation
	double host1 = host;
	char *n = (char *)&network + sizeof(double)-1;
	char *h = (char *)&host1;
	unsigned int x = 1;
	char *y = (char *)&x;
	char z = 1;
	int i;

	if(is_big < 0)
	{
		if(*y == z)
		{
			is_big = 0;
		}
		else
		{
			is_big = 1;
		}
	}

	if(is_big)
	{
		return host;
	}

	for(i=0;i<sizeof(double);i++)
	{
		*n = *h;
		n--;
		h++;
	}
	return network;
}

//Converts 8 bytes from network byte order to host byte order
double ntohd(double network)
{
	static int is_big = -1;			//1 if Big Endian 0 if Little Endian
	double host;								//Holds host byte order representation
	char *n = (char *)&network + sizeof(double) -1;
	char *h = (char *)&host;
	unsigned int x = 1;
	char *y = (char *)&x;
	char z = 1;
	int i;

	if(is_big < 0)
	{
		if(*y == z)
		{
			is_big = 0;
		}
		else
		{
			is_big = 1;
		}
	}

	if(is_big)
	{
		return network;
	}

	for(i=0;i<sizeof(double);i++)
	{
		*h = *n;
		n--;
		h++;
	}
	return host;
}

/*Shifts loss interval values*/
void shift_s_values(int s[])
{
	int i;
	for(i = N_MAX - 1; i>0; i--)
	{
		s[i] = s[i-1];
	}
	s[0] = 0;
	return;
}

/*used for calculating Average loss rate*/
double calc_s_hat(int s[])
{
	int i;
	static double w[N_MAX] = {0};
	double s_hat = 0;
	static double w_sum = 0;
	int n = N_MAX;
	
	if(w[0] == 0)
	{
		for(i = 0; i <n/2; i++)
		{
			w[i] = 1;
		}
		for(i = n/2; i <n; i++)
		{
			w[i] = 1 - (i+1 - n/2.0)/(n/2.0 + 1);
		}
		for(i = 0; i < n; i++)
		{
			w_sum += w[i];
		}
		printf("w_sum: %g\n", w_sum);
	}
	
	for(i = 0; i < n; i++)
	{
		s_hat += w[i]*s[i];
		//printf("s_intermediate: %g\n", s_hat);
	}
	
	s_hat = s_hat/w_sum;
	return s_hat;
}


/*Returns time in seconds with precision down to microsecond*/
double get_time()
{
	struct timeval curTime;
	(void) gettimeofday (&curTime, (struct timezone *) NULL);
	return (((((double) curTime.tv_sec) * 1000000.0)
	         + (double) curTime.tv_usec) / 1000000.0); // do we need to divide by 1000000 for usec
}
