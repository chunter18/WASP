#include "wiced.h"

void application_start(void)
{
    wiced_core_init();

    //doing self test here
    //ADXL_test is connected to GPIO_16 which is WICED_GPIO_12
    wiced_gpio_init(WICED_GPIO_12, OUTPUT_PUSH_PULL);
    wiced_gpio_output_low(WICED_GPIO_12);
    uint16_t sample1 = 0, sample2 = 0;
    uint16_t delta = 0;


    //self test operation
    //sample1 = adc_sample(); //need to measure output voltage first
    wiced_gpio_output_high(WICED_GPIO_12); //turn on self test pin
    wiced_rtos_delay_microseconds(300);
    //sample2 = adc_sample(); //measure output again
    delta = sample1-sample2;
    if(delta > 22 || delta < (-22)) //22mv
    {
        //self test fail, return
        return;
    }
    else
    {
        //self test pass
        wiced_gpio_output_low(WICED_GPIO_12); //release the pin
        wiced_rtos_delay_microseconds(300); //results will be valid again after this time
    }



}
