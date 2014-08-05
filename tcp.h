//tcp.h
//Functions to implement TFRC over UDP Sockets


/*********CODE VALUES**********/
#define START 1
#define STOP 2
#define OK 3
/********MESSAGE TYPES*********/
#define CONTROL 1
#define DATA		2
#define ACK			3
/**********CONSTANTS***********/
#define MSGMAX 4096
#define CONTROL_LENGTH 14
#define ACK_LENGTH 36
#define DATA_HEADER_SIZE 24
/***FLOW CONTROL PARAMETERS****/
#define N_MAX 8
typedef struct client_t{
	char ip[INET6_ADDRSTRLEN];
	unsigned short port;
	int id;
	int seq;
	short int length;
} client;

client* tcp_accept(int sock);
char *NET_get_ip(struct sockaddr_in *addr);
unsigned short NET_get_port(struct sockaddr_in *addr);
double htond(double host);
double ntohd(double network);
void pack_ack(char buf[], int id, int seq, double drop, double rx, int seq_recvd);
void shift_s_values(int s[]);
double calc_s_hat(int s[]);
double get_time();
