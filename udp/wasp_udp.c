#include "wiced.h"
#include "sntp.h"
#include <inttypes.h>

//what is a good sync time? ill choose 1 min
#define TIME_SYNC_PERIOD    (60 * SECONDS)

/* Change the server IP address to match the TCP echo server address */
#define SERVER_IP_ADDRESS MAKE_IPV4_ADDRESS(192,168,1,219)
#define UDP_MAX_DATA_LENGTH         30
#define UDP_TARGET_PORT             50007

wiced_result_t tx_udp_packet_base(); //original send function before modification
wiced_result_t tx_udp_packet();
wiced_result_t rx_udp_packet(uint32_t timeout);
uint16_t spi_bytes(void);



static wiced_udp_socket_t  udp_socket;
//static wiced_timed_event_t udp_tx_event;
static uint32_t tx_count   = 0;

typedef struct{
    uint32_t                packet_count; //udp -> including a packet count for easier reconstruction
    wiced_iso8601_time_t    time_start;
    uint64_t                nano_time_start;
    uint16_t                samples[236];
}tx_pckt_t; //4 + 236*2 + 27 + 8 bytes = 511 bytes


void application_start(void)
{
    wiced_iso8601_time_t     iso8601_time;
    uint64_t                 cpu_time;

    /* Initialize the WICED device */
    wiced_init();

    platform_init_nanosecond_clock();
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);

    //sntp_start_auto_time_sync( TIME_SYNC_PERIOD ); //this is takin forever
    wiced_time_get_iso8601_time( &iso8601_time );
    WPRINT_APP_INFO( ("Current time is: %s\n", (char*)&iso8601_time) );
    WPRINT_APP_INFO( ("sizeof variable: %d\n", sizeof(iso8601_time))); //sizeof gets you the number of bytes
    cpu_time = wiced_get_nanosecond_clock_value();
    WPRINT_APP_INFO(("cpu_time: %f\n", (double)cpu_time));

    /* Create UDP socket */
    if (wiced_udp_create_socket(&udp_socket, UDP_TARGET_PORT, WICED_STA_INTERFACE) != WICED_SUCCESS)
    {
        WPRINT_APP_INFO( ("UDP socket creation failed\n") );
    }


    //lets try sending some example data first.
    for(int i = 0; i < 10; i++)
        tx_udp_packet();




    return;
}


/*
 * Sends a UDP packet
 */
wiced_result_t tx_udp_packet_base()
{
    wiced_packet_t*          packet;
    char*                    udp_data;
    uint16_t                 available_data_length;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( target_ip_addr, SERVER_IP_ADDRESS );

    /* Create the UDP packet */
    if ( wiced_packet_create_udp( &udp_socket, UDP_MAX_DATA_LENGTH, &packet, (uint8_t**) &udp_data, &available_data_length ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP tx packet creation failed\n") );
        return WICED_ERROR;
    }

    /* Write packet number into the UDP packet data */
    sprintf( udp_data, "%d", (int) tx_count++ );

    /* Set the end of the data portion */
    //wiced_packet_set_data_end( packet, (uint8_t*) udp_data + UDP_MAX_DATA_LENGTH );
    wiced_packet_set_data_end( packet, (uint8_t*)udp_data + strlen(udp_data));

    /* Send the UDP packet */
    if ( wiced_udp_send( &udp_socket, &target_ip_addr, UDP_TARGET_PORT, packet ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP packet send failed\n") );
        wiced_packet_delete( packet ); /* Delete packet, since the send failed */
        return WICED_ERROR;
    }

    /*
     * NOTE : It is not necessary to delete the packet created above, the packet
     *        will be automatically deleted *AFTER* it has been successfully sent
     */

    WPRINT_APP_INFO( ("sent: %d\n", (int)tx_count) );

    return WICED_SUCCESS;
}

wiced_result_t tx_udp_packet()
{
    wiced_packet_t*          packet;
    char*                    udp_data;
    tx_pckt_t                raw_data;
    uint16_t                 available_data_length;
    wiced_iso8601_time_t     iso8601_time;
    //uint64_t                 cpu_time;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( target_ip_addr, SERVER_IP_ADDRESS );

    /* Create the UDP packet */
    if ( wiced_packet_create_udp( &udp_socket, 512, &packet, (uint8_t**) &udp_data, &available_data_length ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP tx packet creation failed\n") );
        return WICED_ERROR;
    }

/* memcpy example - void * memcpy ( void * destination, const void * source, size_t num );
struct {
    char name[40];
    int age;
} person, person_copy;
char myname[] = "Pierre de Fermat";
// using memcpy to copy string:
memcpy ( person.name, myname, strlen(myname)+1 );
person.age = 46;
//using memcpy to copy structure:
memcpy ( &person_copy, &person, sizeof(person) );
printf ("person_copy: %s, %d \n", person_copy.name, person_copy.age );
*/

    /* fiddling with the data */
    raw_data.packet_count = tx_count++;
    wiced_time_get_iso8601_time( &iso8601_time );
    raw_data.time_start = iso8601_time;
    raw_data.nano_time_start = wiced_get_nanosecond_clock_value();

    //udp max packet is 65507 bytes long
    //many things use 512 bytes - this is a safe quantity

    //100khz - 100 samples per ms, 100*1000==100,000 samples per second

    //send a packet every 4ms to be under 512
    for(int i = 0; i < 236; i++)
    {
        raw_data.samples[i] = spi_bytes();
    }

    memcpy(udp_data, &raw_data, sizeof(raw_data)); //pack it into the udp buffer

    /* Set the end of the data portion */
    wiced_packet_set_data_end( packet, (uint8_t*) udp_data + strlen(udp_data) ); //what is this max data length?

    /* Send the UDP packet */
    if ( wiced_udp_send( &udp_socket, &target_ip_addr, UDP_TARGET_PORT, packet ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP packet send failed\n") );
        wiced_packet_delete( packet ); /* Delete packet, since the send failed */
        return WICED_ERROR;
    }

    /*
     * NOTE : It is not necessary to delete the packet created above, the packet
     *        will be automatically deleted *AFTER* it has been successfully sent
     */

    WPRINT_APP_INFO( ("Current time is: %s\n", (char*)&iso8601_time) );
    WPRINT_APP_INFO(("cpu_time: %f\n", (float)raw_data.nano_time_start));
    WPRINT_APP_INFO( ("sent: %d\n", (int)tx_count) );

    return WICED_SUCCESS;
}

wiced_result_t rx_udp_packet(uint32_t timeout)
{
    wiced_packet_t* packet;
    char*           udp_data;
    uint16_t        data_length;
    uint16_t        available_data_length;

    /* Wait for UDP packet */
    wiced_result_t result = wiced_udp_receive( &udp_socket, &packet, timeout );

    if ( ( result == WICED_ERROR ) || ( result == WICED_TIMEOUT ) )
    {
        return result;
    }

    wiced_packet_get_data( packet, 0, (uint8_t**) &udp_data, &data_length, &available_data_length );

    if (data_length != available_data_length)
    {
        WPRINT_APP_INFO(("Fragmented packets not supported\n"));
        return WICED_ERROR;
    }

    /* Null terminate the received string */
    udp_data[ data_length ] = '\x0';

    WPRINT_APP_INFO( ("%s\n\n", udp_data) );

    /* Delete packet as it is no longer needed */
    wiced_packet_delete( packet );

    return WICED_SUCCESS;
}

uint16_t spi_bytes(void)
{
      return (uint16_t)(rand() % 65535);
}
