#include "wiced.h"
#include "../WICED/WWD/internal/wwd_sdpcm.h"
#include "network/wwd_buffer_interface.h"


#define CHECK_IOCTL_BUFFER( buff )  if ( buff == NULL ) {  wiced_assert("Allocation failed\n", 0 == 1); return WWD_BUFFER_ALLOC_FAIL; }

int check_die_temp(void);

void application_start( void )
{
        int temperature;

        wiced_init();
        temperature = check_die_temp();
        WPRINT_APP_INFO(("die temp is %d\n",temperature));

}

int check_die_temp(void)
{
    wiced_result_t result;
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    INT*  temp;

    INT* data = (INT*) wwd_sdpcm_get_iovar_buffer( &buffer,  sizeof(INT), "phy_tempsense" );
    CHECK_IOCTL_BUFFER( data );
    result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE );

    if ( result != WICED_SUCCESS )
        return -1;

    data = (INT*) host_buffer_get_current_piece_data_pointer( response );
    *temp = *data;
    host_buffer_release( response, WWD_NETWORK_RX );

    return *temp;
}
