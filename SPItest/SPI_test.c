/*
 * SPI_test.c
 *
 *  Created on: Sep 24, 2018
 *      Author: Cole
 */

#include <string.h>
#include <stdio.h>
#include "wiced.h"


#include "platform_constants.h"
#include "platform_peripheral.h"
#include "platform_map.h"
#include "platform_appscr4.h"

#include "wwd_assert.h"
#include "platform_pinmux.h"
#include "wiced_constants.h"

wiced_spi_device_t spi_device =
{
    .port               = WICED_SPI_1,
    .chip_select        = WICED_GPIO_22, //CS PIN IN 43097
    .speed              = 10000000,
    .mode               = ( SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_MSB_FIRST ),
    .bits               = 8
};

/*
 *      // Define SPI message segment
 *      // wiced_spi_message_segment_t spi_segment[NUM_SPI_MSG_SEGMENTS];
 *      // spi_message_segment structure consists of
 *      // {
 *      //     const void* tx_buffer;
 *      //     void*       rx_buffer;
 *      //     uint32_t    length; //Length of data to be sent
 *      // }
 *
 *      // Initialize all the spi_message segments
 *
 *      spi_segment[0].tx_buffer = tx_buffer;
 *      spi_segment[0].rx_buffer = rx_buffer;
 *      spi_segment[0].length = message_length; //Length of data in tx_buffer
 *
 *      //Continue to add spi_segment[1], spi_segment[2] ...
 *
 *      // Transfer the segments
 *      wiced_spi_transfer( &wiced_spi_device, spi_segment, number_of_segments);
 *
 *      // if wiced_spi_transfer returns WICED_SUCCESS, rx_buffer(If non-NULL) should be populated
 *
 */
void application_start(void)
{

    /* Initialize the WICED device */
    //wiced_init();
    wiced_core_init(); //doesnt init the WLAN stuff

    printf("\n");

    if( wiced_spi_init(&spi_device) != WICED_SUCCESS)
        WPRINT_APP_INFO(( "SPI device init fail\n" ));
    else
        WPRINT_APP_INFO(( "SPI device init success\n" ));

    uint16_t nsegments = 1;
    wiced_spi_message_segment_t message[1];
    uint8_t txbuf[7] , rxbuf[1000];

    txbuf[0] = 0x10;
    txbuf[1] = 0x00;
    txbuf[2] = 0x0A;
    txbuf[3] = 0x03;
    txbuf[4] = 'a';
    txbuf[5] = 't';
    txbuf[6] = 'I';
    message[0].tx_buffer = txbuf;
    message[0].rx_buffer = rxbuf;
    message[0].length = 7;


    if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS){
        WPRINT_APP_INFO(("SPI slave send failed at master side\n"));
    }
    else{
        WPRINT_APP_INFO(("SPI slave send success\n"));
    }



    WPRINT_APP_INFO(("Transfered: "));
    for(int i=0; i<7; i++)
    {
        WPRINT_APP_INFO( ("0x%x, ", *((uint8_t *)message[0].tx_buffer+i)) );
    }
    printf("\n");
    WPRINT_APP_INFO(("Transfered: "));
    for(int i=0; i<7; i++)
    {
            WPRINT_APP_INFO( ("%c, ", *((char *)message[0].tx_buffer+i)) );
    }
    printf("\n");

    for(int i=0; i<1000; i++)
    {
            WPRINT_APP_INFO( ("%c, ", *((char *)message[0].rx_buffer+i)) );
    }
    printf("\n");



    //WPRINT_APP_INFO( ("\nReceived    0x%x\n\n",*(( uint8_t *)message[0].rx_buffer ) ) );


    wiced_rtos_delay_microseconds(15);
    wiced_spi_deinit( &spi_device );
    wiced_rtos_delay_microseconds(15);
    WPRINT_APP_INFO(("END OF SPI\n"));
    wiced_rtos_delay_milliseconds(1000);
    wiced_core_deinit( );



}
