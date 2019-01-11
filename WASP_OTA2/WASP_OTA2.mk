NAME := WASP_OTA_base

APPLICATION_DCT    := ota2_dct.c


#no idea about why this is here or why its needed
GLOBAL_DEFINES     += PLATFORM_NO_DDR=1

GLOBAL_DEFINES     += APPLICATION_STACK_SIZE=16000
GLOBAL_DEFINES     += DCT_CRC32_CALCULATION_SIZE_ON_STACK=2048

$(NAME)_SOURCES    := ota2_base.c				\
					  ota2_helper.c				\
					  ota2_dct.c				
					  


#the example app includes DNS, not sure if thats needed?
$(NAME)_COMPONENTS := utilities/wiced_log 		 	 \
                      utilities/mini_printf 		 \
                      daemons/ota2_service		 	 \
                      filesystems/wicedfs			 \
                      protocols/HTTP
                      
OTA_APPLICATION	:= snip.ota2_extract-$(PLATFORM)
OTA_APP    := build/$(OTA_APPLICATION)/binary/$(OTA_APPLICATION).stripped.elf
                      
WIFI_CONFIG_DCT_H  := wifi_config_dct.h

#other makefile says this is needed for cyw43907eval kit
GLOBAL_DEFINES     += WWD_TEST_NVRAM_OVERRIDE
GLOBAL_DEFINES += TX_PACKET_POOL_SIZE=5 \
                  RX_PACKET_POOL_SIZE=20 \
                  PBUF_POOL_TX_SIZE=8 \
                  PBUF_POOL_RX_SIZE=8 \
                  WICED_ETHERNET_DESCNUM_TX=32 \
                  WICED_ETHERNET_DESCNUM_RX=8 \
                  WICED_ETHERNET_RX_PACKET_POOL_SIZE=32+WICED_ETHERNET_DESCNUM_RX
                    
                    
#tcp window size is usually 7k, reduce here due to need for reduced host memory usage
#and the host needing the extra processing time
#increase this (and provide more memory) if faster download speeds are desired
GLOBAL_DEFINES += WICED_TCP_WINDOW_SIZE=3072

VALID_PLATFORMS    += CYW943907AEVAL1F

VALID_OSNS_COMBOS  := ThreadX-NetX_Duo