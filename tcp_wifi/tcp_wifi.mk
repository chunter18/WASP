NAME := WASP_tcp_wifi

$(NAME)_SOURCES := tcp_wifi.c

WIFI_CONFIG_DCT_H := wifi_config_dct.h

GLOBAL_DEFINES  := PLATFORM_POWERSAVE_DEFAULT=1
GLOBAL_DEFINES  += PLATFORM_WLAN_POWERSAVE_STATS=1
GLOBAL_DEFINES  += PLATFORM_MCU_POWERSAVE_MODE_INIT=PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP
GLOBAL_DEFINES  += WICED_DEEP_SLEEP_SAVE_PACKETS_NUM=4

VALID_OSNS_COMBOS := ThreadX-NetX_Duo ThreadX-NetX FreeRTOS-LwIP