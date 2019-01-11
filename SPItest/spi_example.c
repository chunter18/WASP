/*
 * This is an application to setup a SPI master to communicate with a SPI slave
 * with the following configuration
 *
 *      CPOL =0
 *      CPHA =0
 *      data bit width = 8
 *      Clock frequency = 1MHz
 *
 * When the application is Built and downloaded, the configured SPI master will send
 * data stored in the transfer buffer, to the slave with clk frequency = 1Mhz.
 * The clock frequency could be switched using button press(WICED_SPI_1). Toggle the button to switch
 * through the frequencies set in the array speeds[].
 *
 * Open the UART Terminal to follow the current transactions and the data received from
 * the slave.
 */

#include "wiced.h"


#include "platform_constants.h"
#include "platform_peripheral.h"
#include "platform_map.h"
#include "platform_appscr4.h"

#include "wwd_assert.h"
#include "platform_pinmux.h"
#include "wiced_constants.h"


#define SPI_CLOCK_SPEED_HZ             (1000000)
#define SPI_BIT_WIDTH                  ( 8 )
#define SPI_MODE                       ( SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_MSB_FIRST ) //vnak



wiced_spi_device_t spi_device =
{
    .port        = WICED_SPI_1,
    .chip_select = WICED_GPIO_22, //CS PIN IN 43097
    .speed       = SPI_CLOCK_SPEED_HZ,
    .mode        = SPI_MODE,
    .bits        = SPI_BIT_WIDTH
};
const uint16_t nsegments = 1;

uint32_t speeds[] = { 1000000, 3000000, 5000000, 10000000, 15000000, 20000000};


/******************************************************
 *               Function Definitions
 ******************************************************/

void application_start( void )
{
    uint16_t nsegments = 1;

    uint32_t a = 0;
    wiced_bool_t toggle = WICED_TRUE;
    uint8_t clk_index = 0;
    wiced_spi_message_segment_t message[1];
    uint8_t txbuf[2] , rxbuf[1];
    wiced_core_init();
    while(clk_index < sizeof(speeds)/sizeof(uint32_t) )
    {
        toggle = wiced_gpio_input_get(WICED_BUTTON1) ? WICED_TRUE : WICED_FALSE;

        if(toggle==WICED_FALSE) { clk_index++; }
        WPRINT_APP_INFO(("\nThe clock currently used is %lu \n" , speeds[clk_index]));

        if(wiced_spi_init (&spi_device) != WICED_SUCCESS)
        {
            WPRINT_APP_INFO(("SPI init failed \n")); }
        txbuf[0] = 0x55;
        txbuf[1] = 0x5F;
        message[0].tx_buffer = txbuf;
        message[0].rx_buffer = rxbuf;
        message[0].length = 2;

        if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS){
            WPRINT_APP_INFO(("\nSPI slave send failed at master side\n"));
        }
        else{
            WPRINT_APP_INFO(("\nSPI slave send success\n"));
        }


        WPRINT_APP_INFO( ("\nTransferred 0x%x\n", *((uint8_t *)message[0].tx_buffer)) );
        //WPRINT_APP_INFO( ("Transferred 0x%x\n", *(((uint8_t *)message[0].tx_buffer))+1) );



        WPRINT_APP_INFO( ("\nReceived    0x%x\n\n",*(( uint8_t *)message[0].rx_buffer ) ) );

        wiced_rtos_delay_microseconds(15);
        wiced_spi_deinit( &spi_device );
        wiced_rtos_delay_microseconds(15);

        WPRINT_APP_INFO(("END OF SPI %lu\n" , a++));

        wiced_rtos_delay_milliseconds(1000);
        toggle = WICED_TRUE;

   }
    wiced_core_deinit();


}
