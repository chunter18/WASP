#include "wiced.h"
#include "wiced_low_power.h"
#include "wwd_buffer_interface.h"

#define WIFI_SLEEP_TIME                      (10000 * MILLISECONDS)
#define POWERSAVE_RETURN_TO_SLEEP_DELAY      10
#define USE_POWERSAVE_POLL
#define NETWORK_INTERFACE                    WICED_STA_INTERFACE
#define PING_NUMBER                          3
#define PING_CONDITION                       ( ( wakeup_count % 3 ) == 0 )
#define DEADLINE_NETWORKING_TO_COMPLETE      1000
/* Amount of time to wait prior to going into deep sleep: higher values mean more time out of deep sleep */
#define PING_DEEP_SLEEP_WAIT_TIME            100

static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( wakeup_count )        = 0;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( cold_boot_fast_time ) = 0;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( cold_boot_slow_time ) = 0;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( max_up_time )         = 0;

void application_start(void)
{
    wiced_result_t result;
    wiced_time_t   cur_rtos_time;
    uint32_t       cur_slow_time;
    uint32_t       cur_fast_time;
    uint32_t       warm_boot_fast_time;
    uint32_t       warm_boot_slow_time;
    uint32_t       rtos_time_since_deep_sleep_enter;
    wiced_bool_t   network_inited;

    //bring up the network before initial powerdown process
    result = wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER_RESTORE, NULL );
    if ( result == WICED_SUCCESS )
    {
        WPRINT_APP_INFO(( "Networking inited\n" ));
        //do some stuff to check connection and that
    }

    wakeup_count++;

    wiced_time_get_time( &cur_rtos_time );
    rtos_time_since_deep_sleep_enter = wiced_deep_sleep_ticks_since_enter( );
    cur_slow_time                    = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );
    cur_fast_time                    = platform_tick_get_time( PLATFORM_TICK_GET_FAST_TIME_STAMP );

    WPRINT_APP_INFO(( "Application started: cur_rtos_time=%u cur_slow_time=%lu rtos_time_since_deep_sleep_enter=%lu wakeup_count=%lu\n",
        (unsigned)cur_rtos_time, cur_slow_time, rtos_time_since_deep_sleep_enter, wakeup_count ));

}
