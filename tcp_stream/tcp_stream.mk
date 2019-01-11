NAME := WASP_tcpstream_test

$(NAME)_SOURCES := tcp_stream.c

WIFI_CONFIG_DCT_H := wifi_config_dct.h
VALID_OSNS_COMBOS := ThreadX-NetX_Duo ThreadX-NetX FreeRTOS-LwIP