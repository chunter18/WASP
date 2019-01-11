//Switches between 2 wifi networks

//results at very bottom of the code

#include "wiced.h"
#include "resources.h"
//#include "wiced_log.h"

#define TWOG_SSID   "NETGEAR-Guest"
#define TWOG_PASS   "BbBR977KrhAnG33Q"

#define FIVEG_SSID  "NETGEAR-5G-Guest"
#define FIVEG_PASS  "GSf2dSaqJYFf9Akc"

#define PING_TIMEOUT        900

static platform_dct_wifi_config_t wifi_config_dct_local;

static void read_write_dct(char * apSSID, char * sec_key);
static wiced_result_t send_ping();
wiced_result_t switch_networks(int networknum);
void timing_analysis_code(void);

void application_start(void)
{

    /* Initialise the WICED device */
    wiced_init();

    //run timing analysis - used for first tests
    //timing_analysis_code();

    //test switch api?
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);
    send_ping();
    switch_networks(2);
    send_ping();
    switch_networks(1);
    send_ping();


}


wiced_result_t switch_networks(int networknum)
{
    wiced_result_t wiced_res;

    wwd_wifi_disassoc();
    wiced_res = wiced_network_down(WWD_STA_INTERFACE);
    if(wiced_res != WICED_SUCCESS)
        return wiced_res;

    switch(networknum)
    {
        case 1:
            read_write_dct(TWOG_SSID,TWOG_PASS);
            break;
        case 2:
            read_write_dct(FIVEG_SSID,FIVEG_PASS);
            break;
        case 3:
            break;
    }

    wiced_res = wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);
    return wiced_res;
}

static void read_write_dct(char * apSSID, char * sec_key)
{
    wiced_mac_t                     original_mac_address;
    platform_dct_wifi_config_t*     dct_wifi_config          = NULL;
    platform_dct_wifi_config_t*     dct_wifi_config_modified = NULL;

        /* for testing "Only write DCT data if it is different " */
    uint32_t                        time_before_writing;
    uint32_t                        time_after_writing;

    /* get the wi-fi config section for modifying, any memory allocation required would be done inside wiced_dct_read_lock() */
    wiced_dct_read_lock( (void**) &dct_wifi_config, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof( *dct_wifi_config ) );

    /* save to local structure to restore original values */
    wifi_config_dct_local = *dct_wifi_config;

    /* Print original MAC addresses */
    original_mac_address = dct_wifi_config->mac_address;
    WPRINT_APP_INFO( ( "Original mac_address: ") );
    print_mac_address( (wiced_mac_t*) &original_mac_address );
    WPRINT_APP_INFO( ( "\r\n") );

    /* Modify the MAC address */
    //dct_wifi_config->mac_address = modified_mac_address;

//if (strnicmp((char *)dct_wifi_config->stored_ap_list[0].details.SSID.value,apSSID,sizeof(apSSID))!=0)
//{
    memset((char *)dct_wifi_config->stored_ap_list[0].details.SSID.value, 0,
            sizeof(dct_wifi_config->stored_ap_list[0].details.SSID.value));
    dct_wifi_config->stored_ap_list[0].details.SSID.length = strlen( apSSID );
    strlcpy((char *)dct_wifi_config->stored_ap_list[0].details.SSID.value,
           apSSID,
            sizeof(dct_wifi_config->stored_ap_list[0].details.SSID.value));

    memset((char *)dct_wifi_config->stored_ap_list[0].security_key, 0,
            sizeof(dct_wifi_config->stored_ap_list[0].security_key));
    dct_wifi_config->stored_ap_list[0].security_key_length = strlen(sec_key );
    strlcpy((char *)dct_wifi_config->stored_ap_list[0].security_key,
            sec_key,
            sizeof(dct_wifi_config->stored_ap_list[0].security_key));

    dct_wifi_config->stored_ap_list[0].details.security = WICED_SECURITY_WPA2_MIXED_PSK;

    wiced_time_get_time(&time_before_writing);
    wiced_dct_write( (const void*) dct_wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );
    wiced_time_get_time(&time_after_writing);
    WPRINT_APP_INFO( ( "  > Write data : %ldms\r\n", time_after_writing - time_before_writing) );

    /* write data a second time to show that DCT write doesn't write if it is the same data */
    wiced_time_get_time(&time_before_writing);
    wiced_dct_write( (const void*) dct_wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );
    wiced_time_get_time(&time_after_writing);
    WPRINT_APP_INFO( ( "      Re-write : %ldms\r\n", time_after_writing - time_before_writing) );
//}
    /* release the read lock */
    wiced_dct_read_unlock( dct_wifi_config, WICED_TRUE );

    /* Print modified MAC addresses */
    wiced_dct_read_lock( (void**) &dct_wifi_config_modified, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, 0, sizeof( *dct_wifi_config_modified ) );
    WPRINT_APP_INFO( ( "Modified mac_address: ") );
        /* release the read lock */
    wiced_dct_read_unlock( dct_wifi_config_modified, WICED_TRUE );
    print_mac_address( (wiced_mac_t*) &dct_wifi_config_modified->mac_address );
    WPRINT_APP_INFO( ( "\r\n") );


}

static wiced_result_t send_ping()
{
    uint32_t elapsed_ms;
    wiced_result_t status;
    static int ping_count = 0;

    //this is making it timeout - try 1.1.1.1?
    //wiced_ip_get_gateway_address( WICED_STA_INTERFACE, &ping_target_ip );

    static wiced_ip_address_t INITIALISER_IPV4_ADDRESS(ping_target_ip, MAKE_IPV4_ADDRESS(1,1,1,1));

    //try to print the addr - should be 16843009 in decimal
    WPRINT_APP_INFO(("ipaddr = %d\n", ping_target_ip));

    status = wiced_ping( WICED_STA_INTERFACE, &ping_target_ip, PING_TIMEOUT, &elapsed_ms );

    if ( status == WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("Ping #%d Reply : %lu ms\n", ping_count, (unsigned long)elapsed_ms ));
    }
    else if ( status == WICED_TIMEOUT )
    {
        WPRINT_APP_INFO(("Ping timeout\n"));
    }
    else
    {
        WPRINT_APP_INFO(("Ping error\n"));
    }
    return WICED_SUCCESS;
}

void timing_analysis_code(void)
{
    wwd_time_t time1, time2 ;
    uint32_t channel_t;

    WPRINT_APP_INFO(("\n\n\n\n\n"));

    //join the network
    time1= host_rtos_get_time();
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);
    time2=host_rtos_get_time();
    WPRINT_APP_INFO(("join time =  %d \n\n", time2-time1));

    //get some network info
    wwd_wifi_get_channel(WWD_STA_INTERFACE, &channel_t);
    WPRINT_APP_INFO(("wwd_wifi_get_channel = %d \n",channel_t));
    wwd_wifi_get_rate(WWD_STA_INTERFACE, &channel_t);
    WPRINT_APP_INFO(("wwd_wifi_get_rate = %d \n\n",channel_t));
    //wwd_wifi_get_bw(&channel_t);
    //WPRINT_APP_INFO(("wwd_wifi_get_bw = %d \n",channel_t));

    //send a ping to make sure its all working
    send_ping();

    //dissasociate with ap
    wiced_rtos_delay_milliseconds( 250 );
    time1= host_rtos_get_time();
    wwd_wifi_disassoc();
    time2=host_rtos_get_time();
    WPRINT_APP_INFO(("disassoc time =  %d \n\n", time2-time1));
    time1= host_rtos_get_time();
    wiced_network_down(WWD_STA_INTERFACE);
    time2=host_rtos_get_time();
    WPRINT_APP_INFO(("net down time =  %d \n\n", time2-time1));

    //change ap settings
    wiced_rtos_delay_milliseconds( 250 );
    time1= host_rtos_get_time();
    read_write_dct(FIVEG_SSID,FIVEG_PASS);
    time2=host_rtos_get_time();
    WPRINT_APP_INFO(("dct write time =  %d \n\n", time2-time1));


    WPRINT_APP_INFO( ( " Joining second AP \n" ) );
    time1= host_rtos_get_time();
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);
    time2=host_rtos_get_time();
    WPRINT_APP_INFO(("join time =  %d \n\n", time2-time1));

    //get some network info
    wwd_wifi_get_channel(WWD_STA_INTERFACE, &channel_t);
    WPRINT_APP_INFO(("wwd_wifi_get_channel = %d \n",channel_t));
    wwd_wifi_get_rate(WWD_STA_INTERFACE, &channel_t);
    WPRINT_APP_INFO(("wwd_wifi_get_rate = %d \n\n",channel_t));
    //wwd_wifi_get_bw(&channel_t);
    //WPRINT_APP_INFO(("wwd_wifi_get_bw = %d \n",channel_t));

    //send a ping to make sure its all working
    send_ping();
}


/*
Starting WICED vWiced_006.000.001.0005
Platform CYW943907AEVAL1F initialised
Started ThreadX v5.6
Initialising NetX_Duo v5.7_sp2
Creating Packet pools
WLAN MAC Address : A4:08:EA:D9:C5:E6
WLAN Firmware    : wl0: Oct 23 2017 03:40:42 version 7.15.168.101 (r674438) FWID 01-13cae12
WLAN CLM         : API: 12.2 Data: 9.10.74 Compiler: 1.31.3 ClmImport: 1.36.3 Creation: 2017-10-23 03:36:41





Joining : NETGEAR-Guest
Successfully joined : NETGEAR-Guest
Obtaining IPv4 address via DHCP
DHCP CLIENT hostname WICED IP
IPv4 network ready IP: 192.168.1.23
Setting IPv6 link-local address
IPv6 network ready IP: FE80:0000:0000:0000:A608:EAFF:FED9:C5E6
join time =  8114

wwd_wifi_get_channel = 7
wwd_wifi_get_rate = 130

ipaddr = 4
Ping #0 Reply : 121 ms
net down time =  5001

Original mac_address: 00:A0:50:2E:C4:F6
  > Write data : 280ms
      Re-write : 1ms
Modified mac_address: 00:A0:50:2E:C4:F6
dct write time =  294

 Joining second AP
Joining : NETGEAR-5G-Guest
Failed to join : NETGEAR-5G-Guest
Joining : NETGEAR-5G-Guest
Successfully joined : NETGEAR-5G-Guest
Obtaining IPv4 address via DHCP
DHCP CLIENT hostname WICED IP
IPv4 network ready IP: 192.168.1.23
Setting IPv6 link-local address
IPv6 network ready IP: FE80:0000:0000:0000:A608:EAFF:FED9:C5E6
join time =  14986

wwd_wifi_get_channel = 155
wwd_wifi_get_rate = 780

ipaddr = 4
Ping #0 Reply : 84 ms


LOOKS LIKE ABOUT 20s TO SWITCH NETWORKS - make it 30 for worst case
 */
