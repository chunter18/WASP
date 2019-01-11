#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * This is the soft AP used for device configuration
 */
#define CONFIG_AP_SSID       "WICED Config"
#define CONFIG_AP_CHANNEL    1
#define CONFIG_AP_SECURITY   WICED_SECURITY_WPA2_AES_PSK
#define CONFIG_AP_PASSPHRASE "12345678"

/**
 * This is the soft AP available for normal operation (if used)
 */
#define SOFT_AP_SSID         "WICED Device"
#define SOFT_AP_CHANNEL      1
#define SOFT_AP_SECURITY     WICED_SECURITY_WPA2_AES_PSK
#define SOFT_AP_PASSPHRASE   "WICED_PASSPHRASE"


#define CLIENT_AP_SSID              "NETGEAR-Guest"
#define CLIENT_AP_PASSPHRASE        "BbBR977KrhAnG33Q"
#define CLIENT_AP_BSS_TYPE          WICED_BSS_TYPE_INFRASTRUCTURE
#define CLIENT_AP_SECURITY          WICED_SECURITY_WPA2_MIXED_PSK
#define CLIENT_AP_CHANNEL           1
#define CLIENT_AP_BAND              WICED_802_11_BAND_2_4GHZ


/**
 * The network interface the device will work with
 */
#define WICED_NETWORK_INTERFACE   WICED_STA_INTERFACE



#ifdef __cplusplus
} /*extern "C" */
#endif
