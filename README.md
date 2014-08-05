author : Anjan Rayamajhi and Derek Johnson

Implementation of TCP Friendly Rate Control Algorithm based on RFC 3448 

This archive contains two applications server and client

The Source code for the server is in server.c it depeneds on tcp.c and tcp.h for functions needed for the implementation of TFRC
The source code for the client is in tfrc_client.c it depends on tcpc.c/h clientinfo.h clientresources.c/h

both applications can be made with the command "make"

The server can be invoked by issuing the command
./server <Port #>
	Port #: is a positive integer

The client can be invoked by issuing the command
./tfrc_client <host address> <host port #> <Packet Size> <Session ID> <Loss Rate> <Maximum Throughput>
	host address: Is the ip address of the host in either dotted decimal or domain name format
	host port #: is a positive integer that identifies the port the server is using
	Packet size: positive integer that specifies the packet size in bytes
	Session ID: positive integer
	Loss Rate: floating point number where 0<= Loss Rate <= 1
	Maximum Throughput: positive integer that caps the system throughput. In bits/sec
