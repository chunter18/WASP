#include "wiced_framework.h"
#include "ota2_dct.h"



DEFINE_APP_DCT(ota2_dct_t)
{
    .reboot_count       = 0,
    .ota2_reboot_after_download = 1, //cole add
    .ota2_major_version = APP_VERSION_FOR_OTA2_MAJOR,
    .ota2_minor_version = APP_VERSION_FOR_OTA2_MINOR,
    //.ota2_update_uri    = "192.165.100.226/OTA2_image_file.bin"
};


