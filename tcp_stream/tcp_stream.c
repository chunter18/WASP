#include "wiced.h"

#define TCP_PACKET_MAX_DATA_LENGTH        30 //bytes?
#define TCP_CLIENT_INTERVAL               4
#define TCP_SERVER_PORT                   50007
#define TCP_CLIENT_CONNECT_TIMEOUT        500 //maybe ms?
#define TCP_CLIENT_RECEIVE_TIMEOUT        300 //maybe ms?
#define TCP_CONNECTION_NUMBER_OF_RETRIES  3
#define STREAM_DATA_BUFFER_LENGTH         1000

/* Change the server IP address to match the TCP echo server address */
#define TCP_SERVER_IP_ADDRESS MAKE_IPV4_ADDRESS(192,168,1,164)

void create_pseudo_spi(uint32_t numsamples);
static wiced_result_t tcp_client();

static wiced_time_t start_time;
static wiced_tcp_socket_t  tcp_client_socket;
static wiced_timed_event_t tcp_client_event;
wiced_tcp_stream_t* tcp_stream_wr;
char string_data[STREAM_DATA_BUFFER_LENGTH];

void application_start(void)
{
    wiced_result_t result;

    /* Initialize the WICED device */
    wiced_init();
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);

    result = wiced_time_get_time(&start_time);
    if(result != WICED_SUCCESS)
        return;

    //proceed with test.
    create_pseudo_spi(8);

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

    sprintf(string_data, "%s", "Hello from WICED" );
    /*Fill the buffer string_data with the required contents that you wish to send. The data can be filled as per the size of buffer*/
    for(int i_str=18; i_str< STREAM_DATA_BUFFER_LENGTH; i_str++)
    {
        sprintf(&string_data[i_str], "%d", i_str%9 );
    }

    /* Register a function to send TCP packets */
    wiced_rtos_register_timed_event( &tcp_client_event, WICED_NETWORKING_WORKER_THREAD, &tcp_client, TCP_CLIENT_INTERVAL * SECONDS, 0 );

    WPRINT_APP_INFO(("Connecting to the remote TCP server every %d seconds ...\n", TCP_CLIENT_INTERVAL));

}


void create_pseudo_spi(uint32_t numsamples)
{
    //create some example spi data samples
    int i;
    time_t t;
    wiced_time_t now;
    wiced_result_t result;
    //wiced_time_t time;
    int hrs, mins, secs, ms;

    /* Intializes random number generator */
    srand((unsigned) wiced_time_get_time(&now));

    /* Print 5 random numbers from 0 to 255 */
    for( i = 0 ; i < numsamples ; i++ )
    {
      printf("%d\n", rand() % 256);
    }

    result = wiced_time_get_time(&now);
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

wiced_result_t tcp_client( void* arg )
{
    wiced_result_t           result;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( server_ip_address, TCP_SERVER_IP_ADDRESS );
    int                      connection_retries;
    UNUSED_PARAMETER( arg );

    /* Connect to the remote TCP server, try several times */
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

    /*Pass the buffer/data to the stream function to send data*/
    if (wiced_tcp_stream_write( tcp_stream_wr, string_data, STREAM_DATA_BUFFER_LENGTH) != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("TCP stream send failed\n"));

        /* Close the connection */
        wiced_tcp_disconnect(tcp_stream_wr->socket);

        /*TODO: The freeing of string_data can be handled by the application*/
        return WICED_ERROR;
    }

    /* Close the connection */
    wiced_tcp_disconnect(tcp_stream_wr->socket);
    return WICED_SUCCESS;
}
