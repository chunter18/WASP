#include "wiced.h"
#include "sntp.h"
#include <inttypes.h>

/*
 * 40 kHz - sample every 25us
 * 50 kHz - sample every 20us
 * 75 kHz - sample every 13.3 us
 * 80 kHz - sample every 12.5 us
 * 100kHz - sample every 10us
 * 200kHz - sample every 5us
 *
 */

/*
 * 1ms sample numbers
 * 40khz - 40 samples
 * 50khz - 50 samples
 * 75khz - 75 samples
 * 80khz - 80 samples
 * and so on...
 */

#define TCP_PACKET_MAX_DATA_LENGTH        30 //bytes?
#define TCP_CLIENT_INTERVAL               4
#define TCP_SERVER_PORT                   50007
#define TCP_CLIENT_CONNECT_TIMEOUT        500 //maybe ms?
#define TCP_CLIENT_RECEIVE_TIMEOUT        300 //maybe ms?
#define TCP_CONNECTION_NUMBER_OF_RETRIES  3
#define STREAM_DATA_BUFFER_LENGTH         1000

//what is a good sync time? ill choose 1 min
#define TIME_SYNC_PERIOD    (60 * SECONDS)

/* Change the server IP address to match the TCP echo server address */
#define TCP_SERVER_IP_ADDRESS MAKE_IPV4_ADDRESS(192,168,1,164)

void create_pseudo_spi(char *buffer);
static wiced_result_t tcp_client();

static wiced_time_t start_time;
static wiced_tcp_socket_t  tcp_client_socket;
static wiced_timed_event_t tcp_client_event;
wiced_tcp_stream_t* tcp_stream_wr;

//4 buffers for round robin streaming?
char data0[STREAM_DATA_BUFFER_LENGTH];
char data1[STREAM_DATA_BUFFER_LENGTH];
char data2[STREAM_DATA_BUFFER_LENGTH];
char data3[STREAM_DATA_BUFFER_LENGTH];

typedef struct{
    uint16_t                samples[100]; //assumes a 100khz sampling rate, change this to be dynamic
    wiced_iso8601_time_t    time_start;
    uint64_t                nano_time_start;
    //wiced_iso8601_time_t    time_end;
    //uint64_t                nano_time_end;
}ms_sample_set_t; //228 bytes without end times

typedef struct{
    void *worker_thread;
}wiced_worker_thread_t;

void serialize_data(ms_sample_set_t *x, char* buffer);

void application_start(void)
{
    wiced_result_t result;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( server_ip_address, TCP_SERVER_IP_ADDRESS );
    int                      connection_retries;
    wiced_iso8601_time_t     iso8601_time;
    uint64_t                 cpu_time;
    wiced_rtc_time_t         time;
    platform_result_t        rtc;


    /* Initialize the WICED device */
    wiced_init();
    platform_init_nanosecond_clock();
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);

    result = wiced_time_get_time(&start_time);
    if(result != WICED_SUCCESS)
        return;

    /* trying out 802.11as on wiced
     * RESULT - ALWAYS GET 0! LOOKS LIKE WE CANT USE THIS FEATURE

    wiced_time_enable_8021as();
    uint32_t master_secs;
    uint32_t master_nanosecs;
    uint32_t local_secs;
    uint32_t local_nanosecs;
    wiced_time_read_8021as(&master_secs, &master_nanosecs, &local_secs, &local_nanosecs);
    WPRINT_APP_INFO( ("%" PRIu32 "%" PRIu32 "%" PRIu32 "%" PRIu32 "\n", master_secs, master_nanosecs, local_secs, local_nanosecs) );
    //print result : 0 0 0 0
    return;
    */

    //nanosecond clock?
    cpu_time = wiced_get_nanosecond_clock_value();
    WPRINT_APP_INFO(("cpu_time: %f\n", (double)cpu_time));

    //rtc - is it supported?
    rtc = platform_rtc_init();
    if (rtc == PLATFORM_SUCCESS)
    {
        WPRINT_APP_INFO(("RTC supported - 1s clock accuracy\n"));
        wiced_platform_get_rtc_time(&time);
    }
    else
        WPRINT_APP_INFO(("RTC not supported\n"));


    //wiced_time_get_iso8601_time( &iso8601_time );
    //WPRINT_APP_INFO( ("Current time is: %s\n", (char*)&iso8601_time) );
    //time before sync is always jan 1 1970 + a few seconds


    sntp_start_auto_time_sync( TIME_SYNC_PERIOD );
    wiced_time_get_iso8601_time( &iso8601_time );
    WPRINT_APP_INFO( ("Current time is: %s\n", (char*)&iso8601_time) );
    cpu_time = wiced_get_nanosecond_clock_value();
    WPRINT_APP_INFO(("cpu_time: %f\n", (double)cpu_time));


    return;
    //proceed with test.

    /* Create a TCP socket */
    if ( wiced_tcp_create_socket( &tcp_client_socket, WICED_STA_INTERFACE ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("TCP socket creation failed\n") );
    }

    /* Bind to the socket */
    wiced_tcp_bind( &tcp_client_socket, TCP_SERVER_PORT );

    /*Initialize tcp_stream*/
    tcp_stream_wr = (wiced_tcp_stream_t*)malloc(sizeof(wiced_tcp_stream_t));
    if (wiced_tcp_stream_init(tcp_stream_wr, &tcp_client_socket)!= WICED_SUCCESS)
        WPRINT_APP_INFO(("TCP stream init failed \n"));

    //CONNECT TO SERVER ONCE
    connection_retries = 0;
    do
    {
        result = wiced_tcp_connect( tcp_stream_wr->socket, &server_ip_address, TCP_SERVER_PORT, TCP_CLIENT_CONNECT_TIMEOUT );
        WPRINT_APP_INFO(("Connection retries: %d\n",connection_retries ));
        connection_retries++;
    }
    while( ( result != WICED_SUCCESS ) && ( connection_retries < TCP_CONNECTION_NUMBER_OF_RETRIES ) );
    if(result != WICED_SUCCESS)
        WPRINT_APP_INFO(("Unable to connect to the server! Halt.\n"));

    while(1) //infinite until we break
    {
        //TODO - start a timer, break after like 30 seconds

        /*Fill the buffer string_data with the required contents that you wish to send. The data can be filled as per the size of buffer*/

        //method 1 of filling buffer
        //sprintf(string_data, "%s", "Hello from WICED" );

        //method 2 of filling buffer
        /*
        for(int i_str=18; i_str< STREAM_DATA_BUFFER_LENGTH; i_str++)
        {
            sprintf(&string_data[i_str], "%d", i_str%9 );
        }
        */

        //get some stupid data
        char buffer[8]; //holds 8 bytes
        create_pseudo_spi(&buffer);
        //timestamp it

        /*
        // Register a function to send TCP packets
        //this connects and disconnects. I want to hava continuously open connection
        //wiced_rtos_register_timed_event( &tcp_client_event, WICED_NETWORKING_WORKER_THREAD, &tcp_client, TCP_CLIENT_INTERVAL * SECONDS, 0 );
        //WPRINT_APP_INFO(("Connecting to the remote TCP server every %d seconds ...\n", TCP_CLIENT_INTERVAL));
        */


        /*Pass the buffer/data to the stream function to send data*/
        if (wiced_tcp_stream_write( tcp_stream_wr, buffer, STREAM_DATA_BUFFER_LENGTH) != WICED_SUCCESS)
        {
            WPRINT_APP_INFO(("TCP stream send failed\n"));

            /* Close the connection */
            wiced_tcp_disconnect(tcp_stream_wr->socket);

            /*TODO: The freeing of string_data can be handled by the application*/
            return WICED_ERROR;
        }
    }//end while loop

    //closing up
    wiced_tcp_disconnect(tcp_stream_wr->socket);
}


void create_pseudo_spi(char *buffer)
{
    //create some example spi data samples
    int i;
    wiced_time_t now;

    /* Intializes random number generator */
    srand((unsigned) wiced_time_get_time(&now));

    /* Print 8 random bytes from 0 to 255 */
    for( i = 0 ; i < 8 ; i++ )
    {
      buffer[i] = rand() % 256;
      printf("%d\n", buffer[i]);
    }

    //result = wiced_time_get_time(&now);
    /*
    time  = now - start_time;

    ms   = time % 1000;
    time /= 1000;

    secs = time % 60;
    time /= 60;

    mins = time % 60;
    hrs  = (time / 60) % 24;
    printf("%d:%d:%d.%d\n", hrs, mins, secs, ms);
    */
}


void serialize_data(ms_sample_set_t *x, char* buffer)
{
    //buffer needs to be 228 bytes for struct without end times

    //maybe this isnt the best idea
    int i;
    //sprintf(buffer[i_str], "%d", i_str%9 )
    //x->nano_time_start;
    //x->time_start;
    //x->samples[i];

}






//unused!!! this is from the example snip.
/*
wiced_result_t tcp_client( void* arg )
{
    wiced_result_t           result;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( server_ip_address, TCP_SERVER_IP_ADDRESS );
    int                      connection_retries;
    UNUSED_PARAMETER( arg );

    // Connect to the remote TCP server, try several times
    connection_retries = 0;
    do
    {
        result = wiced_tcp_connect( tcp_stream_wr->socket, &server_ip_address, TCP_SERVER_PORT, TCP_CLIENT_CONNECT_TIMEOUT );
        WPRINT_APP_INFO(("Connection retries: %d\n",connection_retries ));
        connection_retries++;
    }
    while( ( result != WICED_SUCCESS ) && ( connection_retries < TCP_CONNECTION_NUMBER_OF_RETRIES ) );
    if(result != WICED_SUCCESS)
        WPRINT_APP_INFO(("Unable to connect to the server! Halt.\n"));

    //Pass the buffer/data to the stream function to send data
    if (wiced_tcp_stream_write( tcp_stream_wr, string_data, STREAM_DATA_BUFFER_LENGTH) != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("TCP stream send failed\n"));

        // Close the connection
        wiced_tcp_disconnect(tcp_stream_wr->socket);

        //TODO: The freeing of string_data can be handled by the application
        return WICED_ERROR;
    }

    // Close the connection
    //we dont need this for our purposes right??
    //wiced_tcp_disconnect(tcp_stream_wr->socket);
    return WICED_SUCCESS;
}
*/
