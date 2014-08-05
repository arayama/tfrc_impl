
all: server tfrc_client

server: server.o tcp.o
	gcc -Wall server.o tcp.o -o server -lm

tfrc_client:tfrc_client.o clientresources.o tcp.o
	gcc -Wall -o tfrc_client tfrc_client.o clientresources.o tcp.o -lpthread -lm

tfrc_client.o: tfrc_client.c
	gcc -Wall -o tfrc_client.o -c tfrc_client.c -lpthread -lm

clientresources.o: clientresources.c
	gcc -Wall -c clientresources.c -lm

server.o: server.c
	gcc -Wall -c server.c -lm

tcp.o: tcp.c
	gcc -Wall -c tcp.c -lm
	
clean:
	rm -f *.o *~ server tfrc_client core

backup: clean
	rm -f *.tar.gz
	tar -cvf MS4.tar *
	gzip MS4.tar
