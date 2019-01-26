/*
Cole Hunter
Server for WASP project
senior design - 2019
Team: Tyler Hack and Daniel Webber
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h> //is this the right one
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct //not sure if this is somewhere. here for ease of use/def 
{
	char year[4];        /**< Year         */
	char dash1;          /**< Dash1        */
	char month[2];       /**< Month        */
	char dash2;          /**< Dash2        */
	char day[2];         /**< Day          */
	char T;              /**< T            */
	char hour[2];        /**< Hour         */
	char colon1;         /**< Colon1       */
	char minute[2];      /**< Minute       */
	char colon2;         /**< Colon2       */
	char second[2];      /**< Second       */
	char decimal;        /**< Decimal      */
	char sub_second[6];  /**< Sub-second   */
	char Z;              /**< UTC timezone */
}iso8601_time_t;

typedef struct
{
	uint32_t packet_count;
	iso8601_time_t time_start;
	uint64_t nano_time_start;
	uint16_t samples[236];
}WASP_packet_t; //511 bytes

typedef struct
{
	uint64_t control_byte;
	//layout - 
	//1 bit for update available
	//1 bit for go back to sleep yes/norm
	//1 bit for change network
	//1 bit for test begin
	//1 bit for start a self test
	//force wireless modes - qos or something 
}control_packet_t;
//maybe this should be a tcp packet - need to make sure it gets there

//coming back - error codes from self test if fail
// coming back - did self test procedures fail
//coming back - self test info 
//battery info

//need to build a ip to mac LUT in control section

int main (int argc, char *argv[])
{


	//general variables
	int sock, nBytes;
	WASP_packet_t buf;
	WASP_packet_t* buffer = &buf;

	control_packet_t resp; //dont need a response yet
	control_packet_t* response = &resp;
	struct sockaddr_in serverAddr, clientAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	int i;

	//standard socket intialization
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons ((short)50007);
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	memset ((char *)serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));  
	addr_size = sizeof (struct sockaddr_in);

	//create UDP socket
	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf ("socket error\n");
		return -1;
	}

	//bind socket to our IP
	if (bind (sock, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) != 0)
	{
		printf ("bind error\n");
		return -1;
	}

	//do the work

	FILE *outfile;
	//outfile = fopen("/mnt/usb/TEST/test.txt", "wb"); //openwrt with edited fstab
	outfile = fopen("/media/pi/DE96-17E5/test.txt", "wb"); //instead of usb change it to valid dir name 
	
	int len = 0;
	uint32_t count;
	iso8601_time_t tstart;
	uint64_t ntstart;
	uint16_t samps[236];
	char newline = '\n';
	char addr_str[INET_ADDRSTRLEN];
	printf("Listening on port 50007\n");
	while(1)
	{
		//recvfrom is a blocking call - it wont return until a packet comes in 
		nBytes = recvfrom (sock, buffer, sizeof(WASP_packet_t), 0, (struct sockaddr *)&clientAddr, &addr_size);	
		inet_ntop(AF_INET, &clientAddr, addr_str, INET_ADDRSTRLEN); //get printable address

		//grab the goods - only do this if we cant just write it all to the file from the buffer
		count = (buffer->packet_count);
		tstart = (buffer->time_start);
		ntstart = (buffer->nano_time_start);
		memcpy(samps, (buffer->samples), sizeof((buffer->samples))); //put packet data in buffer all at once

		/*
		//fwrite works on binary files - cant read as text.
		fwrite(&count, sizeof(uint64_t), 1, outfile);
		fwrite(&tstart, sizeof(iso8601_time_t), 1, outfile);
		fwrite(&ntstart, sizeof(uint64_t), 1, outfile);
		fwrite(samps, sizeof(uint16_t), 236, outfile);
		fwrite(&newline, sizeof(char), 1, outfile);
		//fwrite(*buffer, sizeof(WASP_packet_t), 1, outfile);
		*/

		fprintf(outfile, "count=%d      "PRIu64, count);
		fprintf(outfile, "count=%d	"PRIu64, count);
		fprintf(outfile, "tstart=%s     ", (char*)&tstart);
		fprintf(outfile, "ntstart=%d    \n"PRIu64, ntstart);
		//fprintf(outfile, "samples=%d      "PRIu64, count);
		
		//send something back - this is in place for the control section. 
		//logic to be implemented later.
		(response->control_byte) = 0;
		sendto (sock, response, sizeof(control_packet_t), 0, (struct sockaddr *)&clientAddr, addr_size);
		
		printf("nbytes = %d\n", nBytes);
		printf("client address = %s\n", addr_str);
		printf("nano time = %f\n", (float)ntstart);
		printf("time = %s\n", (char*)&tstart);
		printf("count = %d\n", count);

		len++;
		if(len == 2)
			break;
	}

	//close up
	fclose(outfile);
	return 0;
}
