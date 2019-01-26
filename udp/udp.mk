NAME := WASP_udp

$(NAME)_SOURCES := wasp_udp.c

$(NAME)_COMPONENTS := protocols/SNTP

WIFI_CONFIG_DCT_H := wifi_config_dct.h

VALID_OSNS_COMBOS := ThreadX-NetX_Duo ThreadX-NetX FreeRTOS-LwIP