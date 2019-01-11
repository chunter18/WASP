#include "wiced.h"

static wiced_bool_t   get_ping_destination( wiced_ip_address_t* ip_address );
static wiced_result_t send_ping ( void );
static void           print_wlan_log( void );

typedef struct
{
    wiced_ssid_t            ssid;
    wiced_security_t        sec_type;
    uint8_t*                passwd;
    uint8_t                 passwd_len;
    host_semaphore_type_t   sem;
    wwd_interface_t         interface;
} AP_INFO;



void application_start( )
{
    wiced_init( );
    host_semaphore_type_t sem;
    //wiced_rtos_init_semaphore(&sem);
    wwd_result_t res;
    wiced_time_t    start_time;
    wiced_time_t    end_time;

    char ssidname1[] = "WASP_2.4";
    char key1[] = "BbBR977KrhAnG33Q";
    uint8_t key1_len = sizeof(key1);
    wiced_ssid_t ssid1 = {sizeof(ssidname1), ssidname1};

    char ssidname2[] = "apt1308-5G";
    char key2[] = "DDTC2019!";
    uint8_t key2_len= sizeof(key2);
    wiced_ssid_t ssid2 = {sizeof(ssidname2), ssidname2};


    WPRINT_APP_INFO(("starting up\n"));
    wiced_time_get_time(&start_time);
    res = wwd_wifi_join(&ssid1, WICED_SECURITY_WPA2_MIXED_PSK,key1,key1_len,&sem,WWD_STA_INTERFACE);
    wiced_time_get_time(&end_time);
    if(res == WWD_SUCCESS)
    {
        WPRINT_APP_INFO(("complete in %lu milliseconds\n", (unsigned long )(end_time - start_time) ) );
        WPRINT_APP_INFO(("pinging\n"));
        send_ping();
        print_wlan_log( );
    }
    else
    {
        WPRINT_APP_INFO(("network result var: %d\n",res));
    }


    WPRINT_APP_INFO(("waiting\n"));
    wiced_rtos_delay_milliseconds(2000);

    wiced_time_get_time(&start_time);
    res = wwd_wifi_leave(WWD_STA_INTERFACE);
    wiced_time_get_time(&end_time);
    if(res == WWD_SUCCESS)
    {
        WPRINT_APP_INFO(("successfully left network 1\n"));
        WPRINT_APP_INFO(("complete in %lu milliseconds\n", (unsigned long )(end_time - start_time) ) );
    }
    else
    {
        WPRINT_APP_INFO(( "leave fail - result %d\n", res ));
    }

    wiced_rtos_delay_milliseconds(2000);

    WPRINT_APP_INFO(("joining ap number 2\n"));
    wiced_time_get_time(&start_time);
    res = wwd_wifi_join(&ssid2, WICED_SECURITY_WPA2_MIXED_PSK,key2,key2_len,&sem,WWD_STA_INTERFACE);
    wiced_time_get_time(&end_time);
    if(res == WWD_SUCCESS)
    {
        WPRINT_APP_INFO(("complete in %lu milliseconds\n", (unsigned long )(end_time - start_time) ) );
        WPRINT_APP_INFO(("pinging\n"));
        send_ping();
        print_wlan_log( );
    }
    else
    {
        WPRINT_APP_INFO(("network result var: %d\n",res));
    }
    //wiced_rtos_deinit_semaphore(&sem);

}

static wiced_result_t send_ping( void )
{
    const uint32_t     ping_timeout = 1000;
    uint32_t           elapsed_ms;
    wiced_result_t     status;
    wiced_ip_address_t ping_target_ip;

    SET_IPV4_ADDRESS(ping_target_ip, MAKE_IPV4_ADDRESS(216, 58,   195,  68));

    status = wiced_ping(WWD_STA_INTERFACE, &ping_target_ip, ping_timeout, &elapsed_ms );

    if ( status == WICED_SUCCESS )
    {
        WPRINT_APP_INFO(( "Ping Reply %lums\n", elapsed_ms ));
    }
    else if ( status == WICED_TIMEOUT )
    {
        WPRINT_APP_INFO(( "Ping timeout\n" ));
    }
    else
    {
        WPRINT_APP_INFO(( "Ping error: %d\n", (int)status ));
    }

    return WICED_SUCCESS;
}

static wiced_bool_t get_ping_destination( wiced_ip_address_t* ip_address )
{
    return ( wiced_ip_get_gateway_address(WWD_STA_INTERFACE, ip_address ) == WICED_SUCCESS ) ? WICED_TRUE : WICED_FALSE;
}

static void print_wlan_log( void )
{
    static char buffer[200];
    (void)wwd_wifi_read_wlan_log( buffer, sizeof( buffer ) );
}
