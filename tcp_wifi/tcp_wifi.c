#include "wiced.h"
#include "wifi_config_dct.h"
#include "wiced_low_power.h"

#define TCP_PACKET_MAX_DATA_LENGTH        30
#define TCP_CLIENT_INTERVAL               2
#define TCP_SERVER_PORT                   50007
#define TCP_CLIENT_CONNECT_TIMEOUT        3000
#define TCP_CLIENT_RECEIVE_TIMEOUT        1000
#define TCP_CONNECTION_NUMBER_OF_RETRIES  3
#define DEADLINE_NETWORKING_TO_COMPLETE   1000
/* Amount of time to wait prior to going into deep sleep: higher values mean more time out of deep sleep */
#define WIFI_SLEEP_TIME                   (600000 * MILLISECONDS)

/*server ip addr*/
#define TCP_SERVER_IP_ADDRESS MAKE_IPV4_ADDRESS(192,168,1,1)

void printIp(wiced_ip_address_t ipV4address);
void print_network_info( ); //debug function - prints DCT ap info.
void print_cur_net_stats(void);
wiced_result_t change_AP(int APnum);
wiced_result_t do_tcp(void);
static void print_wlan_log( void );
static void deep_sleep( void );


static wiced_bool_t network_suspended = WICED_FALSE;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( wakeup_count )        = 0;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( cold_boot_fast_time ) = 0;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( cold_boot_slow_time ) = 0;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( max_up_time )         = 0;


void application_start(void)
{
    wiced_result_t result;
    wiced_time_t    start_time;
    wiced_time_t    end_time;
    char hostname[] = "WASPMODULE_1";
    wiced_network_set_hostname  (hostname);
    wiced_bool_t   network_inited;

    wakeup_count++;
    if ( WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE( ) )
    {
        WPRINT_APP_INFO(( "Warm boot.\n" ));

        result = wiced_resume_after_deep_sleep( );
    }
    else
    {
        WPRINT_APP_INFO(( "Cold boot.\n" )); //aka first start up

        /* Initialise the device and WICED framework */
        result = wiced_init( );

        if ( result == WICED_SUCCESS )
        {
            result = wiced_wifi_enable_powersave();
        }
    }

    //we are warm booted - check to see if resume went ok
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(( "something went wrong\n" ));
        return;
    }

    WPRINT_APP_INFO(( "WICED inited: rtos_time_since_deep_sleep_enter=%lu\n", wiced_deep_sleep_ticks_since_enter( ) ));

    if ( !WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE( ) )
    {
        /* Bring up the network interface and connect to the Wi-Fi network */
        WPRINT_APP_INFO(( "Networking is about to bring up\n" ));
        result = wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    }
    else
    {
        /* Resume network interface */
        WPRINT_APP_INFO(( "Networking is about to resume\n" ));
        result = wiced_network_resume_after_deep_sleep(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    }
    //check if network is actually up
    if ( result == WICED_SUCCESS )
    {
        WPRINT_APP_INFO(( "Networking inited\n" ));
        network_inited = WICED_TRUE;
    }
    else
    {
        WPRINT_APP_INFO(( "Networking NOT inited: result %d\n", result ));
        network_inited = WICED_FALSE;
    }


    //print_network_info(); //prints wifi DCT entries. debug tool only


    /*
    // Bring up the network interface, also time that shit
    DEFUNCT BECUASE OF DEEP SLEEP NETWORKING HANDLERS
    wiced_time_get_time(&start_time);
    result = wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    wiced_time_get_time(&end_time);
    if(result == WICED_SUCCESS)
    {
        print_cur_net_stats();
    }
    else
    {
            return;
    }
    */

    /*main stuff - do what you need to here*/
    while (1)
    {
        do_tcp();
        wiced_time_get_time(&start_time);
        deep_sleep( );
        wiced_time_get_time(&end_time);
        WPRINT_APP_INFO(("complete in %lu seconds\n", (unsigned long )((end_time - start_time)/1000) ) );
        return;
    }

    //do_tcp();
    //do_tcp(); //for some reason the second call resets/crashes the module
    return;

    //everything after here is uncharted - doesnt work correctly
    //drop the network, edit dct, rejoin 5.0, repeat send
    WPRINT_APP_INFO( ("CHANGING NETWORK.\n") );
    result = change_AP(2);
    if(result == WICED_SUCCESS)
    {
        print_cur_net_stats();
    }
    else
    {
        WPRINT_APP_INFO( ("bad join.\n") );
        return;
    }

    do_tcp();



    WPRINT_APP_INFO( ("finished.... closing.\n") );


}

void printIp(wiced_ip_address_t ipV4address)
{
    WPRINT_APP_INFO(("%d.%d.%d.%d\r\n",
            (int)((ipV4address.ip.v4 >> 24) & 0xFF), (int)((ipV4address.ip.v4 >> 16) & 0xFF),
            (int)((ipV4address.ip.v4 >> 8) & 0xFF),  (int)(ipV4address.ip.v4 & 0xFF)));
}

void print_network_info( )
{
    platform_dct_wifi_config_t*  wifi_config;

    // Get a copy of the WIFT config from the DCT into RAM
    wiced_dct_read_lock((void**) &wifi_config, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));

    // Print info
    WPRINT_APP_INFO(("SSID = %s\n\r",wifi_config->stored_ap_list[2].details.SSID.value));
    WPRINT_APP_INFO(("Security = %d\n\r",wifi_config->stored_ap_list[2].details.security));
    WPRINT_APP_INFO(("Passphrase = %s\n\r",wifi_config->stored_ap_list[2].security_key));

    // Free RAM buffer
    wiced_dct_read_unlock(wifi_config, WICED_FALSE);
}

wiced_result_t change_AP(int APnum)
{
    /*
     * this function changes the network association
     * it does so by suspending the network, changing the order of the
     * stored_ap_list in the DCT, writing the changes, then finally
     * re-calling network_up.
     *
     * the function assumes that all of the required APs are already listed
     * int the wifi_config_dct.h file, and that the number passed in APnum
     * is either 2 or 3 - this is the ap from the header file to switch to the
     * first entry.
     */

    if((APnum == 2) || (APnum == 3))
    {
        WPRINT_APP_INFO(("joining network %d\n", APnum));
        APnum = APnum-1; //proper index for array
    }
    else
    {
        WPRINT_APP_INFO(("BAD ARG\n"));
        return WICED_ERROR; //bad arg
    }

    wiced_result_t res;
    wwd_result_t wwdres;
    platform_dct_wifi_config_t*  wifi_config;
    //temps to hold old AP_0 values
    char ssid0_temp[8] = {'\0'};
    char passphrase0_temp[16] = {'\0'};
    //wiced_security_t security;


    wwdres = wwd_wifi_leave(WICED_STA_INTERFACE);

    if(wwdres == WWD_SUCCESS)
    {
        WPRINT_APP_INFO(("left ap\n"));
    }
    else
    {
        WPRINT_APP_INFO(("couldnt leave ap\n"));
        WPRINT_APP_INFO(("code %d\n", wwdres));
        return WICED_ERROR;
    }

    //not sure if this is necessary
    res = wiced_network_down(WICED_STA_INTERFACE);
    if(res == WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("network down\n"));
    }
    else
    {
        WPRINT_APP_INFO(("network didnt come down\n"));
        WPRINT_APP_INFO(("code %d\n", res));
        //return WICED_ERROR;
    }



    // Get a copy of the WIFT config from the DCT into RAM
    wiced_dct_read_lock((void**) &wifi_config, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));

    //store a temp copy of ap_list 0
    strcpy(ssid0_temp, (char *) wifi_config->stored_ap_list[0].details.SSID.value);
    strcpy(passphrase0_temp, (char *) wifi_config->stored_ap_list[0].security_key);
    //dont need to fuck with the security type as long as all three are the same


    //copy APnum entry into first entry
    strcpy((char *) wifi_config->stored_ap_list[0].details.SSID.value, (char *) wifi_config->stored_ap_list[APnum].details.SSID.value);
    wifi_config->stored_ap_list[0].details.SSID.length = strlen((char *) wifi_config->stored_ap_list[APnum].details.SSID.value);
    strcpy((char *) wifi_config->stored_ap_list[0].security_key, (char *) wifi_config->stored_ap_list[APnum].security_key);
    wifi_config->stored_ap_list[0].security_key_length = strlen((char *) wifi_config->stored_ap_list[APnum].security_key);
    //wifi_config->stored_ap_list[0].details.security = security;
    //MAKE SURE SECURITY IS THE SAME IN THE DCT!!!

    //fill in the APnum entry with temp vals
    strcpy((char *) wifi_config->stored_ap_list[APnum].details.SSID.value, ssid0_temp);
    wifi_config->stored_ap_list[APnum].details.SSID.length = strlen(ssid0_temp);
    strcpy((char *) wifi_config->stored_ap_list[APnum].security_key, passphrase0_temp);
    wifi_config->stored_ap_list[APnum].security_key_length = strlen(passphrase0_temp);
    //wifi_config->stored_ap_list[0].details.security = security;

    // Write updated parameters to the DCT
    res = wiced_dct_write((const void *) wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));
    if(res == WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("DCT write SUCCEEDED\n\r"));
    }
    else
    {
        WPRINT_APP_INFO(("DCT write FAILED\n\r"));
    }

    //WPRINT_APP_INFO(("%s\n", (char *) wifi_config->stored_ap_list[0].details.SSID.value));
    //WPRINT_APP_INFO(("%s\n", (char *) wifi_config->stored_ap_list[0].security_key));

    // Free RAM buffer
    wiced_dct_read_unlock(wifi_config, WICED_TRUE);

    wiced_result_t result;
    res = wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);

    return res;
}

wiced_result_t do_tcp(void)
{
    static wiced_tcp_socket_t  tcp_client_socket;
    wiced_packet_t*          packet;
    wiced_packet_t*          rx_packet;
    char*                    tx_data;
    char*                    rx_data;
    uint16_t                 rx_data_length;
    uint16_t                 available_data_length;
    int                      connection_retries;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( server_ip_address, TCP_SERVER_IP_ADDRESS );
    wiced_result_t result;

    /* Create a TCP socket */
    if ( wiced_tcp_create_socket( &tcp_client_socket, WICED_STA_INTERFACE) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("TCP socket creation failed\n") );
    }

    /* Bind to the socket */
    wiced_tcp_bind( &tcp_client_socket, TCP_SERVER_PORT );



    connection_retries = 0;
    do
    {
        result = wiced_tcp_connect( &tcp_client_socket, &server_ip_address, TCP_SERVER_PORT, TCP_CLIENT_CONNECT_TIMEOUT );
        connection_retries++;
    }
    while( ( result != WICED_SUCCESS ) && ( connection_retries < TCP_CONNECTION_NUMBER_OF_RETRIES ) );
    if( result != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("Unable to connect to the server! Halt.\n"));
    }

    /* Create the TCP packet. Memory for the tx_data is automatically allocated */
    if (wiced_packet_create_tcp(&tcp_client_socket, TCP_PACKET_MAX_DATA_LENGTH, &packet, (uint8_t**)&tx_data, &available_data_length) != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("TCP packet creation failed\n"));
        return WICED_ERROR;
    }

    /* Write the message into tx_data"  */
    sprintf(tx_data, "%s", "Hello from WICED\n");

    /* Set the end of the data portion */
    wiced_packet_set_data_end(packet, (uint8_t*)tx_data + strlen(tx_data));

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

    /* Null terminate the received string */
    rx_data[rx_data_length] = '\x0';
    WPRINT_APP_INFO(("%s", rx_data));

    /* Delete the packet and terminate the connection */
    wiced_packet_delete(rx_packet);
    wiced_tcp_disconnect(&tcp_client_socket);

    return WICED_SUCCESS;
}

static void print_wlan_log( void )
{
    static char buffer[200];
    (void)wwd_wifi_read_wlan_log( buffer, sizeof( buffer ) );
}

void print_cur_net_stats(void)
{
    wiced_ip_address_t ipAddress;
    wiced_mac_t mac;

    WPRINT_APP_INFO(("PRINTING INFO...\n"));
    wiced_ip_get_ipv4_address(WICED_STA_INTERFACE, &ipAddress);
    WPRINT_APP_INFO(("IP addr: "));
    printIp(ipAddress);

    wiced_ip_get_netmask(WICED_STA_INTERFACE, &ipAddress);
    WPRINT_APP_INFO(("Netmask: "));
    printIp(ipAddress);

    /* Gateway */
    wiced_ip_get_gateway_address(WICED_STA_INTERFACE, &ipAddress);
    WPRINT_APP_INFO(("Gateway: "));
    printIp(ipAddress);

    wiced_wifi_get_mac_address(&mac);
    WPRINT_APP_INFO(("MAC Address: "));
    WPRINT_APP_INFO(("%X:%X:%X:%X:%X:%X\r\n",
                   mac.octet[0], mac.octet[1], mac.octet[2],
                   mac.octet[3], mac.octet[4], mac.octet[5]));

}

static void deep_sleep( void )
{
    int i;

    /* Wait till packets be sent */
    for ( i = 0; i < DEADLINE_NETWORKING_TO_COMPLETE; ++i )
    {
        if ( wiced_deep_sleep_is_networking_idle(WICED_STA_INTERFACE ) )
        {
            break;
        }

        wiced_rtos_delay_milliseconds( 1 );
    }

    /* Suspend network timers */
    if ( !network_suspended )
    {
        if ( wiced_network_suspend( ) == WICED_SUCCESS )
        {
            network_suspended = WICED_TRUE;
        }
    }

    /*
     * Wakeup system monitor thread so it can:
     *     - kick watchdog
     *     - go to sleep for maximum period so deep-sleep be longer
     */
    wiced_wakeup_system_monitor_thread( );

    /* Enable power save */
    wiced_platform_mcu_enable_powersave( );

    /* Deep-sleep for a while */
    wiced_rtos_delay_milliseconds( WIFI_SLEEP_TIME );

    /* Disable power save */
    wiced_platform_mcu_disable_powersave( );

    /* Resume network timers */
    if ( network_suspended )
    {
        if ( wiced_network_resume( ) == WICED_SUCCESS )
        {
            network_suspended = WICED_FALSE;
        }
    }
}

