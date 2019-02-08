#include "wiced.h"
#include <inttypes.h>

void canned_function(void);
uint16_t spi_bytes(void);

void application_start(void)
{
    wiced_result_t result;
    wiced_queue_t queue;
    char name[] = "WASP_main_queue"; //probably not needed - just takes up memory
    uint32_t size = sizeof(uint16_t);
    uint32_t n_entries = 10*236; //how big is needed
    uint32_t count; //holds  count of items in queue

    wiced_init();
    //wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);

    //init queue
    /*
     * BIG NOTE - threadX queues are allocated in chunks of 32 bits/4bytes.
     * I got init errors when passing a bytes size of 2 (uint16) (wanted to have 1 entry be 1 sample)
     * we also cant alloc size of 472 (wanted to have 236 samples be a message, 1 entry is whole buffer)
     * SO - the queue needs to be sized at like 4 bytes - 2 samples are a message (probably or them together)
     *    - this means a packet buffer consists of 236/2 or 118 queue messages
     *    - for size do 5*118 or 5 full buffers worth. that should be plenty
     *
     * or bigger with a different schema? ask dez
     */
    result = wiced_rtos_init_queue(&queue, NULL, 4, 5*118);
    if(result == WICED_SUCCESS)
        WPRINT_APP_INFO( ("Got good queue\n") );
    else
        WPRINT_APP_INFO( ("queue alloc error!\n") );


    result = wiced_rtos_get_queue_occupancy(&queue, &count);
    if(result == WICED_SUCCESS)
        WPRINT_APP_INFO( ("queue occupancy = %d\n", count) );
    else
        WPRINT_APP_INFO( ("couldnt get queue occupancy\n") );

    result = wiced_rtos_is_queue_empty(&queue);
    if(result == WICED_SUCCESS)
        WPRINT_APP_INFO( ("queue empty\n") );
    else
        WPRINT_APP_INFO( ("queue not empty\n") );

    result = wiced_rtos_is_queue_full(&queue);
    if(result == WICED_SUCCESS)
        WPRINT_APP_INFO( ("queue full\n") );
    else
        WPRINT_APP_INFO( ("queue not full\n") );

    for(int i = 0; i < 10; i++)
    {
        uint16_t x = (uint16_t)i;
        result = wiced_rtos_push_to_queue(&queue, &x, 0); //not sure about last arg. why would we wait to return
        if(result == WICED_SUCCESS)
            WPRINT_APP_INFO( ("successful push\n") );
        else
            WPRINT_APP_INFO( ("push %d failed\n", i) );
    }

    result = wiced_rtos_is_queue_empty(&queue);
    if(result == WICED_SUCCESS)
        WPRINT_APP_INFO( ("queue empty\n") );
    else
        WPRINT_APP_INFO( ("queue not empty\n") );

    result = wiced_rtos_get_queue_occupancy(&queue, &count);
    if(result == WICED_SUCCESS)
        WPRINT_APP_INFO( ("queue occupancy = %d\n", count) );
    else
        WPRINT_APP_INFO( ("couldnt get queue occupancy\n") );

    uint16_t y;
    result = wiced_rtos_pop_from_queue(&queue, &y, 0);
    if(result == WICED_SUCCESS)
        WPRINT_APP_INFO( ("pop return sucess\n") );
    else
        WPRINT_APP_INFO( ("pop failed\n") );
    if(y == 0)
        WPRINT_APP_INFO( ("got good val\n") );
    else
        WPRINT_APP_INFO( ("bad vaule %d\n", y) );



    result = wiced_rtos_deinit_queue(&queue);
    if(result == WICED_SUCCESS)
        WPRINT_APP_INFO( ("deinit success\n") );
    else
        WPRINT_APP_INFO( ("deinit error!\n") );

}

void canned_function(void)
{
    return;
}

uint16_t spi_bytes(void)
{
      return (uint16_t)(rand() % 65535);
}
