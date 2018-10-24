#pragma once

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * This is the default AP the device will connect to (as a client)
 */


#define CLIENT_AP_SSID_50       "WASP_5.0"
#define CLIENT_AP_PASSPHRASE_50 "GSf2dSaqJYFf9Akc"
#define CLIENT_AP_BSS_TYPE_50   WICED_BSS_TYPE_INFRASTRUCTURE
#define CLIENT_AP_SECURITY_50   WICED_SECURITY_WPA2_MIXED_PSK
#define CLIENT_AP_CHANNEL_50    1
//#define CLIENT_AP_BAND_50       WICED_802_11_BAND_2_4GHZ
#define CLIENT_AP_BAND_50       WICED_802_11_BAND_5GHZ //is this right?

/**
 * The network interface the device will work with
 */
#define WICED_NETWORK_INTERFACE   WICED_STA_INTERFACE


#ifdef __cplusplus
} /*extern "C" */
#endif
