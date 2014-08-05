#include "clientinfo.h"
#include <math.h>

// this file contains all the global variables

struct controlMsg cntrl;
struct dataMsg data;
struct ackMsg ack;
struct clientPrms tfrc_client;

client_status cstate;
int CNTCStop;
double usec2,usec1,usec3,usec4;
