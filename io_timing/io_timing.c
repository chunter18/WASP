#include "wiced.h"

void application_start(void)
{

    wiced_core_init();
    wiced_time_t    start_time;
    wiced_time_t    end_time;

    //choosing gpio that will be our cnv pin
    //hopefully the WICE init will work for all of the ways we are doing this
    wiced_gpio_init(WICED_GPIO_34, OUTPUT_PUSH_PULL);



    //begin the tests
    wiced_time_get_time(&start_time);
    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_time_get_time(&end_time);
}
