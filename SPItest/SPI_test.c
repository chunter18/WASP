#include "wiced.h"

#include "platform_constants.h"
#include "platform_peripheral.h"
#include "platform_map.h"
#include "platform_appscr4.h"

#include "wwd_assert.h"
#include "platform_pinmux.h"
#include "wiced_constants.h"

#include "platform.h"

#define VREF 5
//#define VREF 3.3 //WASP

#define mV_per_g 4 //for 5v ref to adxl
#define mV_per_g 2.64 //for 3.3v ref to adxl

// (4mv/5v) = (2.64mv/3.3v)


//static double step = VREF/(0xFF);


/*
 * WICE_GPIO_34 == PIN_I2S_SCLK1
 */

wiced_spi_device_t spi_device =
{
    .port               = WICED_SPI_1,
    .chip_select        = WICED_GPIO_22, //CS PIN IN 43097
    .speed              = 1000000,     //1mhz still for bb
    .mode               = ( SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_MSB_FIRST ),
    .bits               = 8
};


static inline void adc_sample();

void application_start(void)
{

    /* Initialize the WICED device */
    //wiced_init();
    printf("\n\n");
    wiced_core_init();

    wiced_gpio_init(WICED_GPIO_34, OUTPUT_PUSH_PULL);
    wiced_gpio_output_low(WICED_GPIO_34);

    if( wiced_spi_init(&spi_device) != WICED_SUCCESS)
        WPRINT_APP_INFO(( "SPI device init fail\n" ));
    else
        WPRINT_APP_INFO(( "SPI device init success\n" ));

    wiced_rtos_delay_microseconds(15);


    WPRINT_APP_INFO(( "1\n" ));
    int16_t nsegments = 2;
    wiced_spi_message_segment_t message[1];
    uint8_t txbuf[2] , rxbuf[2];
/*
    //toggle cnv for  a convert
    //wiced_gpio_output_high(WICED_GPIO_34);
    //wiced_gpio_output_low(WICED_GPIO_34);

    //txbuf[0] = 0xFF; //this is what we send for conversion
    //txbuf[1] = 0xFF;

    WPRINT_APP_INFO(( "2\n" ));
    txbuf[0] = 0x14;
    txbuf[1] = 0xE4; //masked version
    message[0].tx_buffer = txbuf;
    message[0].rx_buffer = rxbuf;
    message[0].length = 2;

    WPRINT_APP_INFO(( "3\n" ));
    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_gpio_output_low(WICED_GPIO_34);
    WPRINT_APP_INFO(( "X\n" ));
    if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("SPI slave send failed at master side\n"));
    }
    else
    {
        WPRINT_APP_INFO(("SPI slave send success\n"));
    }
    WPRINT_APP_INFO(( "4\n" ));


    txbuf[0] = 0x54; //read reg command
    txbuf[1] = 0xFF;
    message[0].tx_buffer = txbuf;
    message[0].rx_buffer = rxbuf;

    WPRINT_APP_INFO(( "5\n" ));

    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_gpio_output_low(WICED_GPIO_34);

    if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("SPI slave send failed at master side\n"));
    }
    else
    {
        WPRINT_APP_INFO(("SPI slave send success\n"));
    }

    WPRINT_APP_INFO( ("\nReceived  0x%x", rxbuf[0]));
    WPRINT_APP_INFO( ("\nReceived  0x%x", rxbuf[1]));
    WPRINT_APP_INFO( ("\n"));
*/
    txbuf[0] = 0xFF; //this is what we send for conversion
    txbuf[1] = 0xFF;
    //rxbuf[0] = 0x00;
    //rxbuf[1] = 0x00;

    message[0].tx_buffer = txbuf;
    message[0].rx_buffer = rxbuf;
    message[0].length = 2;

    //wiced_gpio_output_high(WICED_GPIO_34);
    //wiced_gpio_output_low(WICED_GPIO_34);

    platform_gpio_output_high( &platform_gpio_pins[WICED_GPIO_34] );
    platform_gpio_output_low( &platform_gpio_pins[WICED_GPIO_34] );
    //do we need to check if its a success? will it really ever not be?
    if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("SPI slave send failed at master side\n"));
    }
    else
    {
        WPRINT_APP_INFO(("SPI slave send success\n"));
        wiced_gpio_output_high(WICED_GPIO_34);
    }

    WPRINT_APP_INFO( ("\nReceived  0x%x", rxbuf[0]));
    WPRINT_APP_INFO( ("\nReceived  0x%x", rxbuf[1]));
    WPRINT_APP_INFO( ("\n"));

    //voltage step size is vref/2^16 (16 bit adc)

    uint16_t sample = ((uint16_t)rxbuf[0] << 8) | rxbuf[1];
    long double step = 5.0/65536.0;
    long double V = step*sample;
    WPRINT_APP_INFO( ("\nSTEP = %Lf, sample = %d, Voltage = %Lf\n", step, sample, V));



    wiced_rtos_delay_microseconds(15);
    wiced_spi_deinit( &spi_device );
    wiced_rtos_delay_microseconds(15);
    WPRINT_APP_INFO(("END OF SPI\n\n\n"));
    wiced_core_deinit( );

}

static inline void adc_sample()
{
    int16_t nsegments = 1;
    wiced_spi_message_segment_t message[1];
    uint8_t txbuf[2] , rxbuf[2];

    txbuf[0] = 0xFF; //this is what we send for conversion
    txbuf[1] = 0xFF;
    //rxbuf[0] = 0x00;
    //rxbuf[1] = 0x00;

    message[0].tx_buffer = txbuf;
    message[0].rx_buffer = rxbuf;
    message[0].length = 2;

    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_gpio_output_low(WICED_GPIO_34);
    //do we need to check if its a success? will it really ever not be?
    if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("SPI slave send failed at master side\n"));
    }
    else
    {
        WPRINT_APP_INFO(("SPI slave send success\n"));
        wiced_gpio_output_high(WICED_GPIO_34);
    }

    WPRINT_APP_INFO( ("\nReceived  0x%x", rxbuf[0]));
    WPRINT_APP_INFO( ("\nReceived  0x%x", rxbuf[1]));
    WPRINT_APP_INFO( ("\n"));

    //voltage step size is vref/2^16 (16 bit adc)

    uint16_t sample = ((uint16_t)rxbuf[0] << 8) | rxbuf[1];

    //we want to preserve accuracy - double doesnt get the low decimals
    //so - we will add som math to get them back in
    //step should be vref/2^16 but we will make it vref*10^6/2^16

    //oh and this math should probably be offloaded to server side
    long double step = 5000000.0/65536.0;
    long double scaled_v = step*sample;
    long double v = scaled_v /1000000;
    WPRINT_APP_INFO( ("\nSTEP = %Lf, sample = %d, Voltage = %Lf\n", step, sample, V));

    double g = 0; //will be 0 for sample of 32768 - exactly halfway between
    //convert to g value

    if(sample >= 32768)
    {
        sample = sample - 32768;
        g = 2.5 + sample*mV_per_g;
    }
    if(sample <= 32768)
    {
        g = 2.5 - sample*mV_per_g;
    }

    WPRINT_APP_INFO( ("\ng = %lf",g));
}
