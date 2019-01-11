#include "wiced.h"

//#define HIBERNATION_PERIOD_MS 10000  //10 seconds
#define HIBERNATION_PERIOD_MS 60000  //1 min
//#define HIBERNATION_PERIOD_MS 300000 //5 min
//#define HIBERNATION_PERIOD_MS 600000 //10 mins

#define HIBERNATION_PERIOD_MIN HIBERNATION_PERIOD_MS/60000

#define PLATFORM_HIB_ENABLE

void send_to_hibernate(uint32_t freq, uint32_t max_ticks);
void boring_bringup(void);

void application_start( )
{
    //begin boot sequence
    uint32_t          freq;
    uint32_t          max_ticks;

    printf("\n\nPreparing to init WICED\n");

    freq      = platform_hibernation_get_clock_freq( );
    max_ticks = platform_hibernation_get_max_ticks( );

    if ( platform_hibernation_is_returned_from( ) )
    {
        uint32_t spent_ticks = platform_hibernation_get_ticks_spent( );
        uint32_t spent_ms    = 1000 * spent_ticks / freq;
        uint32_t spent_min   = spent_ms / 60000;

        printf("Returned from hibernation: spent there %u ticks, %u ms, %u min\n\n", (unsigned)spent_ticks, (unsigned)spent_ms, (unsigned)spent_min);
        boring_bringup();
    }
    else
    {
        printf( "Normal boot\n\n" );
        boring_bringup();
    }

    //end boot sequence

    send_to_hibernate(freq,max_ticks);

}

void send_to_hibernate(uint32_t freq, uint32_t max_ticks)
{
    uint32_t          ticks_to_wakeup;
    platform_result_t result = PLATFORM_BADARG;

    printf( "Entering hibernate state for %u ms | %d min\n", (unsigned)HIBERNATION_PERIOD_MS, HIBERNATION_PERIOD_MIN);

    ticks_to_wakeup = freq * HIBERNATION_PERIOD_MS / 1000;
    if ( ticks_to_wakeup > max_ticks )
    {
        printf( "Scheduled ticks number %u is too big\n", (unsigned)ticks_to_wakeup );
    }
    else
    {
        result = platform_hibernation_start( ticks_to_wakeup );
    }

    while ( 1 )
    {
        printf( "Hibernation failure: %d\n", (int)result );
        host_rtos_delay_milliseconds( 1000 );
        result = platform_hibernation_start( ticks_to_wakeup ); //just in case?
    }
}

void boring_bringup(void)
{
    wiced_init();
    host_rtos_delay_milliseconds( 5000 );
    printf("Bringing up the network config\n");
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);
}
