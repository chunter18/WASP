/*
Cole Hunter
WASP tcp server portion
WASP senior design project 2019
Partners: Daniel Webber and Tyler HAck
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <errno.h> 
#include <string.h>
#include <sys/types.h>

typedef struct
{
	uint8_t mac[6];
	uint8_t battery_level;
	uint16_t test_codes;
}init_packet_t; //initial registration message sent as first message to server

typedef struct
{
	unsigned int fetch_update_command : 1;
	unsigned int send_to_hibernate : 1;
	unsigned int switch_network : 1;
	unsigned int test_begin : 1;
	unsigned int init_self_test_proc : 1;
	unsigned int wireless : 27; //placeholder
}init_response_t; //response message to client device


int main(void)
{
	//setbuf(stdout, NULL);
	printf("listening on port 50007\n\n");

	int n;	//num stuff read by read()
	int listenfd = 0, connfd = 0; //sockets. will need to be made into an array
	struct sockaddr_in serv_addr; //also need to be arrays
	struct sockaddr_in client_addr; //arrays
	int client_len;
	char addr_buf[INET_ADDRSTRLEN];
	char buff[sizeof(init_packet_t)];		//buffer for mac recieve
	char sendbuffer[sizeof(init_response_t)];
	uint32_t sendbuff; //gonna try this out

	init_packet_t init;
	init_response_t response; 

	//beggining of mac->ip LUT
	char macs[300][6];	//this sets the max num devices to 300
	char ips[300][14];
	//strcpy(a[0], "blah"); this is how theyn get set

	//set up - fill buffer and server address mem with all zeros
	memset (&serv_addr, '0', sizeof (serv_addr));
	memset (buff, '0', sizeof (buff));


	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	serv_addr.sin_port = htons (50007); //can change to 50008 for tcp if needed in combination with udp

	//create socket, bind, and listen
	listenfd = socket (AF_INET, SOCK_STREAM, 0);	//listening socket - tcp (SOCK_STREAM)
	bind (listenfd, (struct sockaddr*)&serv_addr, sizeof (serv_addr)); 
	listen (listenfd, 10);	//actually listen on address

	client_len = sizeof(client_addr); //for ip - not working yet
	// accept and interact
	connfd = accept (listenfd, (struct sockaddr*)&client_addr, &client_len);  //create connection socket when host connects.

	//receive first packet 
	//read (connfd, init, sizeof (init_packet_t));
	read (connfd, buff, sizeof (buff));
	memcpy(init.mac, buff, sizeof(init.mac));
	memcpy(&(init.battery_level), buff+sizeof(init.mac), sizeof(init.battery_level));
	memcpy(&(init.test_codes), buff+sizeof(init.mac)+sizeof(init.battery_level), sizeof(init.test_codes));
	inet_ntop(AF_INET, &client_addr, addr_buf, sizeof(addr_buf));


	//send our response packet
	response.fetch_update_command = 1;
	response.send_to_hibernate = 1;
	response.switch_network = 0;
	response.test_begin = 1;
	response.init_self_test_proc = 1;
	response.wireless = 852;
	//memcpy(sendbuffer) //would be doing this if it was a regular packet
	memcpy(&sendbuff, &response, sizeof(response));

	write(connfd, &sendbuff, sizeof(sendbuff));  //use & cuse sendbuff not a char buffer
	close(connfd);	//close connection socket once write has finished

	return 0;
}

/*
//printf("client addr = %s\n", addr_buf);
        printf("mac addr = %x:%x:%x:%x:%x:%x\n", init.mac[0],init.mac[1],\
                         init.mac[2],init.mac[3],init.mac[4],init.mac[5]);
        //printf("mac addr = %s", init.mac);
        printf("battery level is %d\n", init.battery_level);
        printf("error codes -> 0x%x\n", init.test_codes);
*/
