#include "wiced.h"
#include "WWD/include/platform/wwd_platform_interface.h"
#include "tpl.h"

#define TCP_SERVER_PORT                   50007
#define TCP_CLIENT_CONNECT_TIMEOUT        500
#define TCP_CLIENT_RECEIVE_TIMEOUT        300
#define TCP_CONNECTION_NUMBER_OF_RETRIES  30
#define TCP_PACKET_MAX_DATA_LENGTH        30 //why

#define TCP_SERVER_IP_ADDRESS MAKE_IPV4_ADDRESS(192,168,1,219)

static wiced_tcp_socket_t  tcp_client_socket;

typedef struct
{
    uint8_t mac[6];
    uint8_t battery_level;
    uint16_t test_codes;
}init_packet_t;

typedef struct
{
        unsigned int fetch_update_command : 1;
        unsigned int send_to_hibernate : 1;
        unsigned int switch_network : 1;
        unsigned int test_begin : 1;
        unsigned int init_self_test_proc : 1;
        unsigned int wireless : 27; //placeholder
}init_response_t; //response message to client device

static platform_dct_wifi_config_t wifi_config_dct_local;

static wiced_result_t tcp_client(init_packet_t send_packet);
wiced_mac_t get_mac();
wiced_result_t do_update(void);
wiced_result_t ex2(void);

void application_start(void)
{
    wiced_mac_t hostmac;
    /* Initialise the device and WICED framework */
    wiced_init( );

    /* Bring up the network interface */
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);

    /* Create a TCP socket */
    if ( wiced_tcp_create_socket( &tcp_client_socket, WICED_STA_INTERFACE ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("TCP socket creation failed\n") );
    }

    /* Bind to the socket */
    wiced_tcp_bind( &tcp_client_socket, TCP_SERVER_PORT );

    init_packet_t init;
    hostmac = get_mac();
    memcpy(init.mac, hostmac.octet, sizeof(init.mac)); //works
    init.battery_level = 87;
    init.test_codes = 0;
    tcp_client(init);
}


wiced_result_t tcp_client(init_packet_t send_packet)
{
    wiced_result_t           result;
    wiced_packet_t*          packet;
    wiced_packet_t*          rx_packet;
    char*                    tx_data;
    char*                    rx_data;
    uint16_t                 rx_data_length;
    uint16_t                 available_data_length;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( server_ip_address, TCP_SERVER_IP_ADDRESS );
    int                      connection_retries;

    /* Connect to the remote TCP server, try several times */
    connection_retries = 0;
    do
    {
        result = wiced_tcp_connect( &tcp_client_socket, &server_ip_address, TCP_SERVER_PORT, TCP_CLIENT_CONNECT_TIMEOUT );
        connection_retries++;
        wiced_rtos_delay_microseconds(15);
    }
    while( ( result != WICED_SUCCESS ) && ( connection_retries < TCP_CONNECTION_NUMBER_OF_RETRIES ) );
    if( result != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("Unable to connect to the server! Halt.\n"));
    }

    /* Create the TCP packet. Memory for the tx_data is automatically allocated */
    if (wiced_packet_create_tcp(&tcp_client_socket, 30, &packet, (uint8_t**)&tx_data, &available_data_length) != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("TCP packet creation failed\n"));
        return WICED_ERROR;
    }

    size_t macsize = sizeof(send_packet.mac);
    size_t battsize = sizeof(send_packet.battery_level);
    size_t tstsize = sizeof(send_packet.test_codes);
    size_t total = macsize+battsize+tstsize;

    /* Write the message into tx_data"  */
    memcpy(tx_data, (send_packet.mac), macsize);
    //sprintf(tx_data, "%s", "Hello from WICED");

    //expiriment - add on to end
    memcpy(tx_data+macsize, &(send_packet.battery_level), battsize);
    //works!!

    memcpy(tx_data+macsize+battsize, &(send_packet.test_codes), tstsize);

    /* Set the end of the data portion */
    wiced_packet_set_data_end(packet, (uint8_t*)tx_data + total);

    /* Send the TCP packet */
    if (wiced_tcp_send_packet(&tcp_client_socket, packet) != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("TCP packet send failed\n"));

        /* Delete packet, since the send failed */
        wiced_packet_delete(packet);

        /* Close the connection */
        wiced_tcp_disconnect(&tcp_client_socket);
        return WICED_ERROR;
    }

    /* Receive a response from the server and print it out to the serial console */
    result = wiced_tcp_receive(&tcp_client_socket, &rx_packet, TCP_CLIENT_RECEIVE_TIMEOUT);
    if( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("TCP packet reception failed\n"));

        /* Delete packet, since the receive failed */
        wiced_packet_delete(rx_packet);

        /* Close the connection */
        wiced_tcp_disconnect(&tcp_client_socket);
        return WICED_ERROR;
    }

    /* Get the contents of the received packet */
    wiced_packet_get_data(rx_packet, 0, (uint8_t**)&rx_data, &rx_data_length, &available_data_length);

    if (rx_data_length != available_data_length)
    {
        WPRINT_APP_INFO(("Fragmented packets not supported\n"));

        /* Delete packet, since the receive failed */
        wiced_packet_delete(rx_packet);

        /* Close the connection */
        wiced_tcp_disconnect(&tcp_client_socket);
        return WICED_ERROR;
    }

    init_response_t resp;
    memcpy(&resp, rx_data, sizeof(resp));

    WPRINT_APP_INFO(("Values Recieved"));
    WPRINT_APP_INFO(("%s", rx_data));

    /* Delete the packet and terminate the connection */
    wiced_packet_delete(rx_packet);
    wiced_tcp_disconnect(&tcp_client_socket);

    return WICED_SUCCESS;

}

wiced_mac_t get_mac()
{
    wiced_mac_t                     original_mac_address;
    platform_dct_wifi_config_t*     dct_wifi_config          = NULL;

    /* get the wi-fi config section for modifying, any memory allocation required would be done inside wiced_dct_read_lock() */
   wiced_dct_read_lock( (void**) &dct_wifi_config, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof( *dct_wifi_config ) );

   /* save to local structure to restore original values */
   wifi_config_dct_local = *dct_wifi_config;

   /* Print original MAC addresses */
   original_mac_address = dct_wifi_config->mac_address;
   WPRINT_APP_INFO( ( "Original mac_address: ") );
   print_mac_address( (wiced_mac_t*) &original_mac_address );
   WPRINT_APP_INFO( ( "\n") );

   return original_mac_address;
}

wiced_result_t do_update(void)
{
    WPRINT_APP_INFO( ( "Update\n") );
    return WICED_SUCCESS;
}

wiced_result_t ex2(void)
{
    WPRINT_APP_INFO( ( "Got Here\n") );
    return WICED_SUCCESS;
}

