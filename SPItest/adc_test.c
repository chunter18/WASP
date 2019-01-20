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
    .speed              = 3000000,     //1mhz still for bb
    .mode               = ( SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_MSB_FIRST ),
    .bits               = 8
};


void application_start(void)
{

    /* Initialize the WICED device */
    //wiced_init();
    wiced_core_init();

    wiced_gpio_init(WICED_GPIO_34, OUTPUT_PUSH_PULL);
    wiced_gpio_output_low(WICED_GPIO_34);

    if( wiced_spi_init(&spi_device) != WICED_SUCCESS)
        WPRINT_APP_INFO(( "SPI device init fail\n" ));
    else
        WPRINT_APP_INFO(( "SPI device init success\n" ));

    wiced_rtos_delay_microseconds(15);

    int16_t nsegments = 1;
    wiced_spi_message_segment_t message[1];
    uint8_t txbuf[2] , rxbuf[2];

    //toggle cnv
    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_gpio_output_low(WICED_GPIO_34);

    txbuf[0] = 0xFF;
    txbuf[1] = 0xFF;
    message[0].tx_buffer = txbuf;
    message[0].rx_buffer = rxbuf;
    message[0].length = 2;

    if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("SPI slave send failed at master side\n"));
    }
    else
    {
        WPRINT_APP_INFO(("SPI slave send success\n"));
    }

    wiced_rtos_delay_microseconds(15);
    //wiced_spi_deinit( &spi_device );
    wiced_rtos_delay_microseconds(15);
    WPRINT_APP_INFO(("END OF SPI\n\n\n"));
    wiced_core_deinit( );

}
