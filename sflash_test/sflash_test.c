#include "wiced.h"
#include "spi_flash.h"

#define WRITE_CHUNK_SIZE        (8*1024)  /* Writing in chunks is only needed to prevent reset by watchdog */
#define SECTOR_SIZE             (4096)
#define SFLASH_START_ADDR       (0x00600000)
                       //chip size 0x800000


unsigned long new_base_addr;
static uint8_t   Rx_Buffer[SECTOR_SIZE +10];
const char teststring[] = /* 0 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             /* 1024 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             /* 2048 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             /* 3072 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             /* 4096 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef012";
                             /* 4643 */

wiced_result_t write_to_spi_flash();

void application_start(void)
{
    sflash_handle_t sflash_handle;
    //char teststring[] = "IAMTYLERANDIAMAVERYBIGCHUNGUS";
    unsigned long size;
    size = sizeof(teststring)-1;

    /* Initialise the serial flash driver */
    if ( init_sflash( &sflash_handle, 0, SFLASH_WRITE_ALLOWED ) != 0 )
    {
        /* return from main in order to try rebooting... */
        return -1;
    }

    //write seems to work fine
    //write_to_spi_flash();
    unsigned long chip_size;
    sflash_get_size( &sflash_handle, &chip_size );
    WPRINT_APP_INFO(("\n\nSFLASH Device ID ( 0x%lx) \n", sflash_handle.device_id ));
    WPRINT_APP_INFO(("SFLASH Size      ( 0x%lx)\n\n", chip_size ));

    if ( ( chip_size != 0 ) && ( size + SFLASH_START_ADDR  > chip_size ) )
    {
        WPRINT_APP_INFO(( "Size (%lu from address %lu) too big to fit on chip (%lu) - aborting!\n", size, SFLASH_START_ADDR, chip_size ));
        deinit_sflash( &sflash_handle);
    }

    WPRINT_APP_INFO(( "Writing location\n" ));

    unsigned long pos;

    pos = 0;
    while ( pos < size )
    {
        unsigned int write_size = ( size - pos > (unsigned long) WRITE_CHUNK_SIZE )? (unsigned int) WRITE_CHUNK_SIZE : (unsigned int) ( size - pos );
        if ( 0 != sflash_write( &sflash_handle, (unsigned long)(pos + SFLASH_START_ADDR ), &teststring[pos], write_size ) )
        {
            /* Verify Error - Chip not erased properly */
            wiced_result_t result = WICED_ERROR;
            WPRINT_APP_INFO(( "Write error - abort!\n" ));
            //goto back_to_idle;
            deinit_sflash( &sflash_handle);
            WPRINT_APP_INFO(( "Going back to idle\n" ));
        }
        pos += write_size;
        (void) platform_watchdog_kick( );
    }

    //write finished, do a read and confirm

    WPRINT_APP_INFO(( "Verifying existing data!\n" ));

    // Read data from SPI FLASH memory
    //unsigned long pos;
    pos = 0;
    while ( pos < size )
    {
        unsigned int read_size = ( size - pos > (unsigned long) sizeof(Rx_Buffer) )? (unsigned int) sizeof(Rx_Buffer) : (unsigned int) ( size - pos );
        if ( 0 != sflash_read( &sflash_handle, pos + SFLASH_START_ADDR, Rx_Buffer, read_size ) )
        {
            // Verify Error - Chip not erased properly /
            WPRINT_APP_INFO(( "Read error - abort!\n" ));
            deinit_sflash( &sflash_handle);
        }
        if ( 0 != memcmp( Rx_Buffer, &teststring[pos], (size_t) read_size ) )
        {
            // Existing data different
            WPRINT_APP_INFO(( "Existing data is different - stop verification\n" ));
            Rx_Buffer[(sizeof(teststring)/sizeof(teststring[0]))] = '\0'; //null terminate
            WPRINT_APP_INFO(( "%s\n",Rx_Buffer ));
            ///*@innerbreak@ /* Only break out of inner-most loop
            break;
        }
        pos += read_size;
        (void) platform_watchdog_kick( );
    }
    if ( pos >= size )
    {
        // Existing data matches
        // No write required
        WPRINT_APP_INFO(( "Existing data matches - successfully aborting!\n" ));
        deinit_sflash( &sflash_handle);
    }
}

wiced_result_t write_to_spi_flash()
{
    sflash_handle_t sflash_handle;
    char teststring[] = "IAMTYLERANDIAMAVERYBIGCHUNGUS";
    unsigned long size;
    size = sizeof(teststring)-1;

    /* Initialise the serial flash driver */
    if ( init_sflash( &sflash_handle, 0, SFLASH_WRITE_ALLOWED ) != 0 )
    {
        /* return from main in order to try rebooting... */
        return -1;
    }

    unsigned long chip_size;
    sflash_get_size( &sflash_handle, &chip_size );
    WPRINT_APP_INFO(("\n\nSFLASH Device ID ( 0x%lx) \n", sflash_handle.device_id ));
    WPRINT_APP_INFO(("SFLASH Size      ( 0x%lx)\n\n", chip_size ));

    if ( ( chip_size != 0 ) && ( size + SFLASH_START_ADDR  > chip_size ) )
    {
        WPRINT_APP_INFO(( "Size (%lu from address %lu) too big to fit on chip (%lu) - aborting!\n", size, SFLASH_START_ADDR, chip_size ));
        deinit_sflash( &sflash_handle);
    }

    WPRINT_APP_INFO(( "Writing location\n" ));

    unsigned long pos;

    pos = 0;
    while ( pos < size )
    {
        unsigned int write_size = ( size - pos > (unsigned long) WRITE_CHUNK_SIZE )? (unsigned int) WRITE_CHUNK_SIZE : (unsigned int) ( size - pos );
        if ( 0 != sflash_write( &sflash_handle, (unsigned long)(pos + SFLASH_START_ADDR ), &teststring[pos], write_size ) )
        {
            /* Verify Error - Chip not erased properly */
            wiced_result_t result = WICED_ERROR;
            WPRINT_APP_INFO(( "Write error - abort!\n" ));
            //goto back_to_idle;
            deinit_sflash( &sflash_handle);
            WPRINT_APP_INFO(( "Going back to idle\n" ));
        }
        pos += write_size;
        (void) platform_watchdog_kick( );
    }
}
