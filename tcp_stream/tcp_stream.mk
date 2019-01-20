NAME := WASP_tcpstream_test

$(NAME)_SOURCES := tcp_stream.c

$(NAME)_COMPONENTS := protocols/SNTP

WIFI_CONFIG_DCT_H := wifi_config_dct.h
VALID_OSNS_COMBOS := ThreadX-NetX_Duo ThreadX-NetX FreeRTOS-LwIP