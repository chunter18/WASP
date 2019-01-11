

void application_start( )
{
    /*
     * wasp boot sequence elaboration
     *
     * check if we have just returned from hibernate
     * if no - normal boot
     * if yes -
     *   check battery level
     *   bring up network
     *   register with server
     *      check to see if we should hibernate again
     *      if yes - go to hibernate
     *      if no - go to normal boot
     *      or OTA - go to ota boot sequence
     *
     *
     * normal boot
     * init the device
     * bring up the gas guage and check the battery level
     * bring up the adc and prepare it for taking data
     * bring up the network and register as ready to go
     * allocate spots for data to go
     */

}
