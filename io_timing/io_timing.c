#include "wiced.h"

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

void application_start(void)
{

    wiced_core_init();
    uint64_t        cpu_time_start;
    uint64_t        cpu_time_end;
    uint64_t        delta;
    int cc_gpio_bit = 31;

    WPRINT_APP_INFO(("\n\n"));
    wiced_gpio_init(WICED_GPIO_34, OUTPUT_PUSH_PULL);
    wiced_gpio_output_low(WICED_GPIO_34);
    platform_init_nanosecond_clock();

    //choosing gpio that will be our cnv pin
    //hopefully the WICE init will work for all of the ways we are doing this

    /* this if for finding the cc bits
    wiced_gpio_init(WICED_GPIO_1, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_2, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_3, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_4, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_5, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_6, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_7, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_8, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_9, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_10, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_11, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_12, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_13, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_14, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_15, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_16, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_17, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_18, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_19, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_20, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_21, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_22, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_23, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_24, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_25, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_26, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_27, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_28, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_29, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_30, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_31, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_32, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_33, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_34, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_35, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_36, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_37, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_38, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_39, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_40, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_41, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_42, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_43, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_44, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_45, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_46, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_47, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_48, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_49, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_50, OUTPUT_PUSH_PULL);
    wiced_gpio_init(WICED_GPIO_51, OUTPUT_PUSH_PULL);

    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_1]);
    WPRINT_APP_INFO(("GPIO #1 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_2]);
    WPRINT_APP_INFO(("GPIO #2 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_3]);
    WPRINT_APP_INFO(("GPIO #3 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_4]);
    WPRINT_APP_INFO(("GPIO #4 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_5]);
    WPRINT_APP_INFO(("GPIO #5 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_6]);
    WPRINT_APP_INFO(("GPIO #6 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_7]);
    WPRINT_APP_INFO(("GPIO #7 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_8]);
    WPRINT_APP_INFO(("GPIO #8 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_9]);
    WPRINT_APP_INFO(("GPIO #9 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_10]);
    WPRINT_APP_INFO(("GPIO #10 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_11]);
    WPRINT_APP_INFO(("GPIO #11 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_12]);
    WPRINT_APP_INFO(("GPIO #12 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_13]);
    WPRINT_APP_INFO(("GPIO #13 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_14]);
    WPRINT_APP_INFO(("GPIO #14 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_15]);
    WPRINT_APP_INFO(("GPIO #15 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_16]);
    WPRINT_APP_INFO(("GPIO #16 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_17]);
    WPRINT_APP_INFO(("GPIO #17 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_18]);
    WPRINT_APP_INFO(("GPIO #18 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_19]);
    WPRINT_APP_INFO(("GPIO #19 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_20]);
    WPRINT_APP_INFO(("GPIO #20 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_21]);
    WPRINT_APP_INFO(("GPIO #21 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_22]);
    WPRINT_APP_INFO(("GPIO #22 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_23]);
    WPRINT_APP_INFO(("GPIO #23 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_24]);
    WPRINT_APP_INFO(("GPIO #24 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_25]);
    WPRINT_APP_INFO(("GPIO #25 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_26]);
    WPRINT_APP_INFO(("GPIO #26 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_27]);
    WPRINT_APP_INFO(("GPIO #27 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_28]);
    WPRINT_APP_INFO(("GPIO #28 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_29]);
    WPRINT_APP_INFO(("GPIO #29 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_30]);
    WPRINT_APP_INFO(("GPIO #30 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_31]);
    WPRINT_APP_INFO(("GPIO #31 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_32]);
    WPRINT_APP_INFO(("GPIO #32 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_33]);
    WPRINT_APP_INFO(("GPIO #33 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_34]);
    WPRINT_APP_INFO(("GPIO #34 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_35]);
    WPRINT_APP_INFO(("GPIO #35 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_36]);
    WPRINT_APP_INFO(("GPIO #36 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_37]);
    WPRINT_APP_INFO(("GPIO #37 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_38]);
    WPRINT_APP_INFO(("GPIO #38 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_39]);
    WPRINT_APP_INFO(("GPIO #39 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_40]);
    WPRINT_APP_INFO(("GPIO #40 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_41]);
    WPRINT_APP_INFO(("GPIO #41 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_42]);
    WPRINT_APP_INFO(("GPIO #42 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_43]);
    WPRINT_APP_INFO(("GPIO #43 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_44]);
    WPRINT_APP_INFO(("GPIO #44 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_45]);
    WPRINT_APP_INFO(("GPIO #45 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_46]);
    WPRINT_APP_INFO(("GPIO #46 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_47]);
    WPRINT_APP_INFO(("GPIO #47 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_48]);
    WPRINT_APP_INFO(("GPIO #48 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_49]);
    WPRINT_APP_INFO(("GPIO #49 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_50]);
    WPRINT_APP_INFO(("GPIO #50 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    cc_gpio_bit = platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_51]);
    WPRINT_APP_INFO(("GPIO #51 -> cc_gpio_bit = %d\n", cc_gpio_bit));
    */



    //I tried these, but the accuracy wasnt near enough.
    //wiced_time_get_time(&start_time);
    //wiced_time_get_time(&end_time);

    /*
    //begin the tests
    cpu_time_start = wiced_get_nanosecond_clock_value();
    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_gpio_output_low(WICED_GPIO_34);
    cpu_time_end = wiced_get_nanosecond_clock_value();
    delta = (cpu_time_end - cpu_time_start);
    WPRINT_APP_INFO(("WICED API took %f nanoseconds, %f useconds\n", (double)delta, (double)(delta/1000)));

    cpu_time_start = wiced_get_nanosecond_clock_value();
    platform_gpio_output_high(&platform_gpio_pinsX[WICED_GPIO_34]);
    platform_gpio_output_low(&platform_gpio_pinsX[WICED_GPIO_34]);
    cpu_time_end = wiced_get_nanosecond_clock_value();
    delta = (cpu_time_end - cpu_time_start);
    WPRINT_APP_INFO(("PLATFORM API took %f nanoseconds, %f useconds\n", (double)delta, (double)(delta/1000)));
    WPRINT_APP_INFO(("cc_gpio_bit = %d\n", cc_gpio_bit));


    cpu_time_start = wiced_get_nanosecond_clock_value();
    //drive high
    PLATFORM_CHIPCOMMON->gpio.output |= (   1 << cc_gpio_bit );
    // Drive the GPIO pin output low
    PLATFORM_CHIPCOMMON->gpio.output &= (~( 1 << cc_gpio_bit ));
    cpu_time_end = wiced_get_nanosecond_clock_value();
    delta = (cpu_time_end - cpu_time_start);
    WPRINT_APP_INFO(("LOW LEVEL took %f nanoseconds, %f useconds\n", (double)delta, (double)(delta/1000)));
    */

    //logic analyzer test

    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_gpio_output_low(WICED_GPIO_34);
    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_gpio_output_low(WICED_GPIO_34);
    //drive high
    PLATFORM_CHIPCOMMON->gpio.output |= (   1 << cc_gpio_bit );
    // Drive the GPIO pin output low
    PLATFORM_CHIPCOMMON->gpio.output &= (~( 1 << cc_gpio_bit ));
    //drive high
    PLATFORM_CHIPCOMMON->gpio.output |= (   1 << cc_gpio_bit );
    // Drive the GPIO pin output low
    PLATFORM_CHIPCOMMON->gpio.output &= (~( 1 << cc_gpio_bit ));

}

/*
 * 43907
GPIO #1 -> cc_gpio_bit = 0
GPIO #2 -> cc_gpio_bit = 1
GPIO #3 -> cc_gpio_bit = 7
GPIO #4 -> cc_gpio_bit = 8
GPIO #5 -> cc_gpio_bit = 9
GPIO #6 -> cc_gpio_bit = 10
GPIO #7 -> cc_gpio_bit = 11
GPIO #8 -> cc_gpio_bit = 12
GPIO #9 -> cc_gpio_bit = 13
GPIO #10 -> cc_gpio_bit = 14
GPIO #11 -> cc_gpio_bit = 15
GPIO #12 -> cc_gpio_bit = 16
GPIO #13 -> cc_gpio_bit = 2
GPIO #14 -> cc_gpio_bit = 3
GPIO #15 -> cc_gpio_bit = 4
GPIO #16 -> cc_gpio_bit = 5
GPIO #17 -> cc_gpio_bit = 6
GPIO #18 -> cc_gpio_bit = 23
GPIO #19 -> cc_gpio_bit = 17
GPIO #20 -> cc_gpio_bit = 18
GPIO #21 -> cc_gpio_bit = 19
GPIO #22 -> cc_gpio_bit = 20
GPIO #23 -> cc_gpio_bit = 7
GPIO #24 -> cc_gpio_bit = 7
GPIO #25 -> cc_gpio_bit = 7
GPIO #26 -> cc_gpio_bit = 7
GPIO #27 -> cc_gpio_bit = 7
GPIO #28 -> cc_gpio_bit = 7
GPIO #29 -> cc_gpio_bit = 24
GPIO #30 -> cc_gpio_bit = 25
GPIO #31 -> cc_gpio_bit = 26
GPIO #32 -> cc_gpio_bit = 27
GPIO #33 -> cc_gpio_bit = 30
GPIO #34 -> cc_gpio_bit = 31
GPIO #35 -> cc_gpio_bit = 7
GPIO #36 -> cc_gpio_bit = 29
GPIO #37 -> cc_gpio_bit = 28
GPIO #38 -> cc_gpio_bit = 7
GPIO #39 -> cc_gpio_bit = 7
GPIO #40 -> cc_gpio_bit = 7
GPIO #41 -> cc_gpio_bit = 7
GPIO #42 -> cc_gpio_bit = 7
GPIO #43 -> cc_gpio_bit = 7
GPIO #44 -> cc_gpio_bit = 7
GPIO #45 -> cc_gpio_bit = 7
GPIO #46 -> cc_gpio_bit = 7
GPIO #47 -> cc_gpio_bit = 7
GPIO #48 -> cc_gpio_bit = 21
GPIO #49 -> cc_gpio_bit = 22
GPIO #50 -> cc_gpio_bit = 7
GPIO #51 -> cc_gpio_bit = 7
*/


