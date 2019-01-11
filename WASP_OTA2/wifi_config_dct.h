#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* This is the soft AP used for device configuration */
#define CONFIG_AP_SSID       "NOT USED FOR THIS APP"
#define CONFIG_AP_PASSPHRASE "NOT USED FOR THIS APP"
#define CONFIG_AP_SECURITY   WICED_SECURITY_OPEN
#define CONFIG_AP_CHANNEL    1
#define CONFIG_AP_VALID      WICED_FALSE

/* This is the soft AP available for normal operation */
#define SOFT_AP_SSID         "NOT USED FOR THIS APP"
#define SOFT_AP_PASSPHRASE   "NOT USED FOR THIS APP"
#define SOFT_AP_SECURITY     WICED_SECURITY_WPA2_AES_PSK
#define SOFT_AP_CHANNEL      1
//#define SOFT_AP_VALID        WICED_TRUE

/* This is the default AP the device will connect to (as a client)*/
#define CLIENT_AP_SSID       "WASP_2.4"
#define CLIENT_AP_PASSPHRASE "BbBR977KrhAnG33Q"
#define CLIENT_AP_BSS_TYPE   WICED_BSS_TYPE_INFRASTRUCTURE
#define CLIENT_AP_SECURITY   WICED_SECURITY_WPA2_AES_PSK
#define CLIENT_AP_CHANNEL    6
#define CLIENT_AP_BAND       WICED_802_11_BAND_2_4GHZ

/* Override default country code */
#define WICED_COUNTRY_CODE    WICED_COUNTRY_UNITED_STATES
#define WICED_COUNTRY_AGGREGATE_CODE    WICED_COUNTRY_AGGREGATE_XV_0


#define WICED_NETWORK_INTERFACE   WICED_STA_INTERFACE


#ifdef __cplusplus
} /* extern "C" */
#endif
