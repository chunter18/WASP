#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wiced_ota2_service.h"


/*
 * define the current version here or on make command line as args
 * make <application>-<platform> ota2_image APP_VERSION_FOR_OTA2_MAJOR=x APP_VERSION_FOR_OTA2_MINOR=y
 */
#if defined(OTA2_SUPPORT)
#ifndef APP_VERSION_FOR_OTA2_MAJOR
#define APP_VERSION_FOR_OTA2_MAJOR  1
#endif
#ifndef APP_VERSION_FOR_OTA2_MINOR
#define APP_VERSION_FOR_OTA2_MINOR  0
#endif
#endif


typedef struct ota2_dct_s
{
        uint32_t                    reboot_count;
        uint8_t                     ota2_reboot_after_download;                     /* automatic reboot after successful download */
        uint16_t                    ota2_major_version;                             /* define APP_VERSION_FOR_OTA2_MAJOR as make arg */
        uint16_t                    ota2_minor_version;                             /* define APP_VERSION_FOR_OTA2_MINOR as make arg */
        //char                        ota2_update_uri[WICED_OTA2_HTTP_QUERY_SIZE];    /* default Web address of update package */
} ota2_dct_t;

#ifdef __cplusplus
} /* extern "C" */
#endif
