/*
 * SPI_test.c
 *
 *  Created on:
 *      Author: Cole
 */


//at 1MHz - a byte sample every 8us
//16 us for a 16bit sample

//MOSI

//set mosi high at the beginning and end and see if anything changes?

#include "wiced.h"

#include "platform_constants.h"
#include "platform_peripheral.h"
#include "platform_map.h"
#include "platform_appscr4.h"

#include "wwd_assert.h"
#include "platform_pinmux.h"
#include "wiced_constants.h"

uint32_t speeds[] = { 1000000, 3000000, 5000000, 10000000, 15000000, 20000000}; //all the speeds avail
//                     1MHz     3MHz     5MHz     10MHz      15MHz     20MHz

#define SPI_CLOCK_SPEED_HZ             (1000000) //1MHz
#define SPI_BIT_WIDTH                  ( 8 )
#define SPI_MODE                       ( SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_MSB_FIRST ) //vnak
//???

wiced_spi_device_t spi_device =
{
    .port               = WICED_SPI_1,
    .chip_select        = WICED_GPIO_22, //CS PIN IN 43097
    .speed              = 1000000,     //1mhz
    .mode               = ( SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_MSB_FIRST ),
    .bits               = 8
};

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
    uint8_t txbuf[6] , rxbuf[6];

    txbuf[0] = 0xAA;
    txbuf[1] = 0x00;
    txbuf[2] = 0xFF;
    txbuf[3] = 0x00;
    txbuf[4] = 'Z';
    txbuf[5] = 0x00;
    message[0].tx_buffer = txbuf;
    message[0].rx_buffer = rxbuf;
    message[0].length = 6;


    if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("SPI slave send failed at master side\n"));
    }
    else
    {
        WPRINT_APP_INFO(("SPI slave send success\n"));
    }

    WPRINT_APP_INFO( ("\nTransferred 0x%x\n", *((uint8_t *)message[0].tx_buffer)) );
    WPRINT_APP_INFO( ("Transferred 0x%x\n", *(((uint8_t *)message[0].tx_buffer))+1) );
    WPRINT_APP_INFO( ("Transferred 0x%x\n", *(((uint8_t *)message[0].tx_buffer))+2) );
    WPRINT_APP_INFO( ("Transferred 0x%x\n", *(((uint8_t *)message[0].tx_buffer))+3) );
    WPRINT_APP_INFO( ("Transferred 0x%x\n", *(((uint8_t *)message[0].tx_buffer))+4) );
    WPRINT_APP_INFO( ("Transferred 0x%x\n", *(((uint8_t *)message[0].tx_buffer))+5) );


    WPRINT_APP_INFO( ("\nReceived    0x%x\n\n",*(( uint8_t *)message[0].rx_buffer ) ) ); //0th


    wiced_rtos_delay_microseconds(15);
    wiced_spi_deinit( &spi_device );
    wiced_rtos_delay_microseconds(15);
    WPRINT_APP_INFO(("END OF SPI\n"));
    wiced_core_deinit( );
}
