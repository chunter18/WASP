#include "wiced.h"
#include <inttypes.h>
#include "wiced_osl.h"

#define SPI_CLOCK_SPEED_HZ        ( 1000000 )
#define SPI_BIT_WIDTH             ( 8 )
#define SPI_MODE                  ( SPI_CLOCK_FALLING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_MSB_FIRST | SPI_CS_ACTIVE_LOW )
#define MCP3208_START             (0x01<<7)
#define MCP3208_SE                (0x01<<6)
#define MCP3208_DIFF              (0x00<<6)
#define MCP3208_CH0               (0x00<<3)
#define MCP3208_CH1               (0x01<<3)
#define MCP3208_CH2               (0x02<<3)
#define MCP3208_CH3               (0x03<<3)
#define MCP3208_CH4               (0x04<<3)
#define MCP3208_CH5               (0x05<<3)
#define MCP3208_CH6               (0x06<<3)
#define MCP3208_CH7               (0x07<<3)

static const wiced_spi_device_t spi1_device =
{
        .port        = WICED_SPI_2,
        .chip_select = WICED_GPIO_NONE,
        .speed       = SPI_CLOCK_SPEED_HZ,
        .mode        = SPI_MODE,
        .bits        = SPI_BIT_WIDTH
};

wiced_spi_message_segment_t spi_segment;
uint16_t *wbuf;
uint16_t *rbuf;
uint16_t code, val;
wiced_queue_t queue;

uint16_t spi_bytes(void);
void thread_main(int arg);
wiced_result_t adc_init( void );
uint16_t adc_takesample( void );

void application_start(void)
{
    wiced_result_t result;
    wiced_thread_t thread;
    wiced_thread_function_t function;
    uint32_t stack_size = 1024;
    int arg;
    uint32_t count; //holds  count of items in queue

    wiced_init();
    adc_init();
    platform_init_nanosecond_clock();

    result = wiced_rtos_init_queue(&queue, NULL, 4, 5*118);
    if(result == WICED_SUCCESS)
        WPRINT_APP_INFO( ("Got good queue\n") );
    else
    {
        WPRINT_APP_INFO( ("queue alloc error!\n") );
        return;
    }

    //needs to be &thread
    result = wiced_rtos_create_thread (&thread, 5, "threadname", thread_main, 5*1024, &arg); //function is just function name
    osl_udelay(22);
}

uint16_t spi_bytes(void)
{
      return (uint16_t)(rand() % 65535);
}

void thread_main(int arg)
{
    UNUSED_PARAMETER(arg);
    uint64_t        cpu_time_start;
    uint64_t        cpu_time_end;
    uint64_t        delta;
    //this is where we wanna take adc samples
    uint16_t res1 = 0;
    uint16_t res2 = 0;
    //i wanna time this?
    cpu_time_start = wiced_get_nanosecond_clock_value();
    res1 = adc_takesample();
    cpu_time_end = wiced_get_nanosecond_clock_value();
    delta = (cpu_time_end - cpu_time_start);
    WPRINT_APP_INFO(("takesample took %f nanoseconds, %f useconds\n", (float)delta, (float)(delta/1000)));

    cpu_time_start = wiced_get_nanosecond_clock_value();
    osl_udelay(5);
    cpu_time_end = wiced_get_nanosecond_clock_value();
    delta = (cpu_time_end - cpu_time_start);
    WPRINT_APP_INFO(("delay took %f nanoseconds, %f useconds\n", (float)delta, (float)(delta/1000)));

    res2 = adc_takesample();

    cpu_time_start = wiced_get_nanosecond_clock_value();
    uint32_t push = ((uint32_t)res1 << 16) + res2;
    wiced_rtos_push_to_queue(&queue, &push, 0);
    cpu_time_end = wiced_get_nanosecond_clock_value();
    delta = (cpu_time_end - cpu_time_start);
    WPRINT_APP_INFO(("covert+push took %f nanoseconds, %f useconds\n", (float)delta, (float)(delta/1000)));

    return;
}

/*
 * Initializes SPI for shitty adc. we will use it for io delay timing stuff
 */
wiced_result_t adc_init( void )
{
    wiced_result_t result = WICED_SUCCESS;
    float vin;

    wbuf = malloc(sizeof(uint16_t)*4);
    if (wbuf == NULL)
    {
        WPRINT_APP_INFO( ("Unable to allocate wbuf\n" ));
        return result;
    }

    rbuf = malloc(sizeof(uint16_t)*4);
    if (rbuf == NULL)
    {
        WPRINT_APP_INFO( ("Unable to allocate rbuf\n" ));
        free( wbuf );
        return result;
    }

    memset(wbuf, 0, (sizeof(uint16_t)*4));
    memset(rbuf, 0, (sizeof(uint16_t)*4));

    /* Initialize SPI1 */
    if ( wiced_spi_init( &spi1_device ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "SPI1 Initialization Failed\n" ) );
        free( rbuf );
        free( wbuf );
        return WICED_ERROR;
    }

    spi_segment.tx_buffer = (void*)wbuf;
    spi_segment.rx_buffer = (void*)rbuf;
    spi_segment.length = 4;

    wbuf[0] = MCP3208_START | MCP3208_SE | MCP3208_CH7; /* single-ended, CH7) */
    result = wiced_spi_transfer( &spi1_device, &spi_segment, 1 );
    if (result != WICED_SUCCESS)
    {
        WPRINT_APP_INFO( ( "SPI1 Transfer Failed\n" ) );
        free( rbuf );
        free( wbuf );
        return WICED_ERROR;
    }

    // get the 12b out of the return
    code = 0;
    code = (rbuf[1]&0xff)<<8;
    code |= (rbuf[1]&0xff00)>>8;
    code = (code>>2) & 0xffff;

    // reverse bit order
    val  = ((code&0x1)<<11) | ((code&0x2)<<9) | ((code&0x4)<<7) | ((code&0x8)<<5);
    val |= ((code&0x20)<<1) | ((code&0x10)<<3);
    val |= ((code&0x80)>>3) | ((code&0x40)>>1);
    val |= ((code&0x800)>>11) | ((code&0x400)>>9) | ((code&0x200)>>7) | ((code&0x100)>>5);

    vin = (3.30 / 4096) * val;
    WPRINT_APP_INFO( ( "MCP3208 CH7 %.1fV\n", vin ) );

    free( wbuf );
    free( rbuf );

    return WICED_SUCCESS;
}

uint16_t adc_takesample( void )
{
    wiced_spi_transfer( &spi1_device, &spi_segment, 1 );
    code = 0;
    code = (rbuf[1]&0xff)<<8;
    code |= (rbuf[1]&0xff00)>>8;
    code = (code>>2) & 0xffff;
    val  = ((code&0x1)<<11) | ((code&0x2)<<9) | ((code&0x4)<<7) | ((code&0x8)<<5);
    val |= ((code&0x20)<<1) | ((code&0x10)<<3);
    val |= ((code&0x80)>>3) | ((code&0x40)>>1);
    val |= ((code&0x800)>>11) | ((code&0x400)>>9) | ((code&0x200)>>7) | ((code&0x100)>>5);
    return val;
}
