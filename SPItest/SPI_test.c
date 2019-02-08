#include "wiced.h"

#include "platform_constants.h"
#include "platform_peripheral.h"
#include "platform_map.h"
#include "platform_appscr4.h"

#include "wwd_assert.h"
#include "platform_pinmux.h"
#include "wiced_constants.h"

#include "platform.h"

#define PLATFORM_HIB_ENABLE

//adding spi bb slowdown
#define SPI_BB_TRANSFER_DELAY_SUPPORT
//clock rate is 320 MHz by default on cyw43907

#define VREF 5
//#define VREF 3.3 //WASP

#define mV_per_g 4 //for 5v ref to adxl
//#define mV_per_g 2.64 //for 3.3v ref to adxl

// (4mv/5v) = (2.64mv/3.3v)


//static double step = VREF/(0xFF);


/*
 * WICE_GPIO_34 == PIN_I2S_SCLK1
 */

//this is from platform.c
const platform_gpio_t platform_gpio_pinsX[] =
{
    [WICED_GPIO_1          ]    = { PIN_GPIO_0 },
    [WICED_GPIO_2          ]    = { PIN_GPIO_1 },
    [WICED_GPIO_3          ]    = { PIN_GPIO_7 },
    [WICED_GPIO_4          ]    = { PIN_GPIO_8 },
    [WICED_GPIO_5          ]    = { PIN_GPIO_9 },
    [WICED_GPIO_6          ]    = { PIN_GPIO_10 },
    [WICED_GPIO_7          ]    = { PIN_GPIO_11 },
    [WICED_GPIO_8          ]    = { PIN_GPIO_12 },
    [WICED_GPIO_9          ]    = { PIN_GPIO_13 },
    [WICED_GPIO_10         ]    = { PIN_GPIO_14 },
    [WICED_GPIO_11         ]    = { PIN_GPIO_15 },
    [WICED_GPIO_12         ]    = { PIN_GPIO_16 },
    [WICED_GPIO_13         ]    = { PIN_PWM_0 },
    [WICED_GPIO_14         ]    = { PIN_PWM_1 },
    [WICED_GPIO_15         ]    = { PIN_PWM_2 },
    [WICED_GPIO_16         ]    = { PIN_PWM_3 },
    [WICED_GPIO_17         ]    = { PIN_PWM_4 },
    [WICED_GPIO_18         ]    = { PIN_PWM_5 },
    [WICED_GPIO_19         ]    = { PIN_SPI_0_MISO },
    [WICED_GPIO_20         ]    = { PIN_SPI_0_CLK },
    [WICED_GPIO_21         ]    = { PIN_SPI_0_MOSI },
    [WICED_GPIO_22         ]    = { PIN_SPI_0_CS },
    [WICED_GPIO_23         ]    = { PIN_GPIO_2 },
    [WICED_GPIO_24         ]    = { PIN_GPIO_3 },
    [WICED_GPIO_25         ]    = { PIN_GPIO_4 },
    [WICED_GPIO_26         ]    = { PIN_GPIO_5 },
    [WICED_GPIO_27         ]    = { PIN_GPIO_6 },
    [WICED_GPIO_28         ]    = { PIN_I2S_MCLK0 },
    [WICED_GPIO_29         ]    = { PIN_I2S_SCLK0 },
    [WICED_GPIO_30         ]    = { PIN_I2S_LRCLK0 },
    [WICED_GPIO_31         ]    = { PIN_I2S_SDATAI0 },
    [WICED_GPIO_32         ]    = { PIN_I2S_SDATAO0 },
    [WICED_GPIO_33         ]    = { PIN_I2S_MCLK1 },
    [WICED_GPIO_34         ]    = { PIN_I2S_SCLK1 },
    [WICED_GPIO_35         ]    = { PIN_I2S_LRCLK1 },
    [WICED_GPIO_36         ]    = { PIN_I2S_SDATAI1 },
    [WICED_GPIO_37         ]    = { PIN_I2S_SDATAO1 },
    [WICED_GPIO_38         ]    = { PIN_SPI_1_CLK },
    [WICED_GPIO_39         ]    = { PIN_SPI_1_MISO },
    [WICED_GPIO_40         ]    = { PIN_SPI_1_MOSI },
    [WICED_GPIO_41         ]    = { PIN_SPI_1_CS },
    [WICED_GPIO_42         ]    = { PIN_SDIO_CLK },
    [WICED_GPIO_43         ]    = { PIN_SDIO_CMD },
    [WICED_GPIO_44         ]    = { PIN_SDIO_DATA_0 },
    [WICED_GPIO_45         ]    = { PIN_SDIO_DATA_1 },
    [WICED_GPIO_46         ]    = { PIN_SDIO_DATA_2 },
    [WICED_GPIO_47         ]    = { PIN_SDIO_DATA_3 },
    [WICED_GPIO_48         ]    = { PIN_I2C0_SDATA },
    [WICED_GPIO_49         ]    = { PIN_I2C0_CLK },
    [WICED_GPIO_50         ]    = { PIN_I2C1_SDATA },
    [WICED_GPIO_51         ]    = { PIN_I2C1_CLK },


    [WICED_PERIPHERAL_PIN_1]    = { PIN_RF_SW_CTRL_6 },
    [WICED_PERIPHERAL_PIN_2]    = { PIN_RF_SW_CTRL_7 },
    [WICED_PERIPHERAL_PIN_3]    = { PIN_UART0_RXD },
    [WICED_PERIPHERAL_PIN_4]    = { PIN_UART0_TXD },
    [WICED_PERIPHERAL_PIN_5]    = { PIN_UART0_CTS },
    [WICED_PERIPHERAL_PIN_6]    = { PIN_UART0_RTS },
    [WICED_PERIPHERAL_PIN_7]    = { PIN_RF_SW_CTRL_8 },
    [WICED_PERIPHERAL_PIN_8]    = { PIN_RF_SW_CTRL_9 },
};

wiced_spi_device_t spi_device =
{
    .port               = WICED_SPI_1,
    .chip_select        = WICED_GPIO_22, //CS PIN IN 43097
    .speed              = 500000,  /*500khz*/   //1mhz base for bb, if #define is ther can change it here
    .mode               = ( SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_MSB_FIRST ),
    .bits               = 8
};


static inline void adc_sample();
void delay(void);

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
    txbuf[1] = 0xF4; //masked version
    message[0].tx_buffer = txbuf;
    message[0].rx_buffer = rxbuf;
    message[0].length = 2;
    WPRINT_APP_INFO(( "3\n" ));

    WPRINT_APP_INFO(( "4\n" ));
    if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("SPI slave send failed at master side\n"));
    }
    else
    {
        WPRINT_APP_INFO(("SPI slave send success\n"));
    }
    WPRINT_APP_INFO(( "5\n" ));
*/

    /*
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
///*
    WPRINT_APP_INFO(( "6\n" ));
    txbuf[0] = 0xFF; //this is what we send for conversion
    txbuf[1] = 0xFF;
    //rxbuf[0] = 0x00;
    //rxbuf[1] = 0x00;

    message[0].tx_buffer = txbuf;
    message[0].rx_buffer = rxbuf;
    message[0].length = 2;
    WPRINT_APP_INFO(( "7\n" ));

    //wiced_gpio_output_high(WICED_GPIO_34);
    //wiced_gpio_output_low(WICED_GPIO_34);

    //drive high
    //PLATFORM_CHIPCOMMON->gpio.output |= (   1 << 31 );
    //wiced_rtos_delay_microseconds(15);
    // Drive the GPIO pin output low
    //PLATFORM_CHIPCOMMON->gpio.output &= (~( 1 << 31 ));
    //platform_gpio_output_high(&platform_gpio_pinsX[WICED_GPIO_34]);
    //wiced_rtos_delay_microseconds(1);
    //platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_34]);
    //do we need to check if its a success? will it really ever not be?
    WPRINT_APP_INFO(( "8\n" ));
    //PLATFORM_CHIPCOMMON->gpio.output |= (   1 << 31 );
    //PLATFORM_CHIPCOMMON->gpio.output |= (   1 << 31 );
    //PLATFORM_CHIPCOMMON->gpio.output &= (~( 1 << 31 ));

    if (wiced_spi_transfer(&spi_device , message, nsegments)!=WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("SPI slave send failed at master side\n"));
    }
    else
    {
        WPRINT_APP_INFO(("SPI slave send success\n"));
    }

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
    //WPRINT_APP_INFO( ("\nReceived  0x%x", rxbuf[2]));
    WPRINT_APP_INFO( ("\n"));

    //voltage step size is vref/2^16 (16 bit adc)

    uint16_t sample = ((uint16_t)rxbuf[0] << 8) | rxbuf[1];
    long double step = 5000000.0/65536.0;
    long double scaled_v = step*sample;
    long double V = scaled_v /1000000;
    WPRINT_APP_INFO( ("\nSTEP = %Lf, sample = %d, Voltage = %Lf\n", step, sample, V));


    wiced_rtos_delay_microseconds(15);
    wiced_spi_deinit( &spi_device );
    wiced_rtos_delay_microseconds(15);
    WPRINT_APP_INFO(("END OF SPI\n\n\n"));
    wiced_core_deinit( );
    wiced_rtos_delay_milliseconds(100000);
    platform_hibernation_start(320000);

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
    long double V = scaled_v /1000000;
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

void delay(void)
{
    return;
}
