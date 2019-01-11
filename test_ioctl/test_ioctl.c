#include "wiced.h"
#include "resources.h"
#ifdef __IAR_SYSTEMS_ICC__
  #include "iar_unistd.h"
#else
  #include "unistd.h"
#endif

#define MAX_LINE_LENGTH  (128)
#define MAX_HISTORY_LENGTH (20)
#define DELAY_BETWEEN_SCANS       ( 5000 )

static char line_buffer[MAX_LINE_LENGTH];
static char history_buffer_storage[MAX_LINE_LENGTH * MAX_HISTORY_LENGTH];

void application_start(void)
{
    /* Initialise the device */
    wiced_init();

    /* Bring up the STA (client) interface ------------------------------------------------------- */
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);

    wiced_rtos_delay_microseconds(1000);


    //IOVAR test section
    uint32_t assoc_time;
    wwd_wifi_get_iovar_value(IOVAR_STR_SCAN_ASSOC_TIME,&assoc_time,WWD_STA_INTERFACE);
    WPRINT_APP_INFO(("\n scan association time is %d seconds\n", (int)assoc_time));


    //IOCTL test section
    uint32_t magic;
    uint32_t version;
    uint32_t loop;
    uint32_t msglevel;
    uint32_t promisc;
    uint32_t rate;
    uint32_t maxrate;
    uint32_t instance;
    uint32_t infra;
    uint32_t auth;
    uint32_t bssid;
    uint32_t ssid;
    uint32_t channel;
    uint32_t srl;
    uint32_t plcphdr;
    uint32_t radio;
    uint32_t phytype;
    uint32_t fixrate;
    uint32_t key;
    uint32_t regulatory;
    uint32_t roam_trigger;
    uint32_t roam_delta;
    uint32_t txant;
    uint32_t antdiv;
    uint32_t closed;
    uint32_t bcnprd;
    uint32_t dtimprd;
    uint32_t srom;
    uint32_t country;
    uint32_t pm;
    uint32_t forcelink;
    uint32_t phyreg;
    uint32_t radioreg;
    uint32_t revinfo;
    uint32_t ucantdiv;
    uint32_t rxant;
    uint32_t attim;
    uint32_t rssi_value;
    uint32_t phy_noise;
    uint32_t bss_info;
    uint32_t bandlist;

    //magic not working
    if(wwd_wifi_get_ioctl_value( WLC_GET_MAGIC, &magic, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n MAGIC for the associated AP is = %d \n", (int)magic));
    wiced_rtos_delay_microseconds(1000);

    //version not working
    if(wwd_wifi_get_ioctl_value( WLC_GET_VERSION, &version, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n version for the associated AP is = %d \n", (int)version));
    wiced_rtos_delay_microseconds(1000);

    //not sure what this is - val is 4850565
    if(wwd_wifi_get_ioctl_value( WLC_GET_LOOP, &loop, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n loop for the associated AP is = %d \n", (int)loop));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_MSGLEVEL, &msglevel, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n msglevel for the associated AP is = %d \n", (int)msglevel));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_PROMISC, &promisc, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n promisc for the associated AP is = %d \n", (int)promisc));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_RATE, &rate, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n rate for the associated AP is = %d \n", (int)rate));
    wiced_rtos_delay_microseconds(1000);
    //if(wwd_wifi_get_ioctl_value( WLC_GET_MAX_RATE, &maxrate, WWD_STA_INTERFACE )!=WWD_SUCCESS)
    //    WPRINT_APP_INFO(("\n maxrate for the associated AP is = %d \n", (int)maxrate));
    //wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_INSTANCE, &instance, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n instance for the associated AP is = %d \n", (int)instance));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_INFRA, &infra, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n infra for the associated AP is = %d \n", (int)infra));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_AUTH, &auth, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n auth for the associated AP is = %d \n", (int)auth));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_BSSID, &bssid, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n bssid for the associated AP is = %d \n", (int)bssid));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_SSID, &ssid, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n ssid for the associated AP is = %d \n", (int)ssid));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_CHANNEL, &channel, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n channel for the associated AP is = %d \n", (int)channel));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_SRL, &srl, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n srl for the associated AP is = %d \n", (int)srl));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_PLCPHDR, &plcphdr, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n plcphdr for the associated AP is = %d \n", (int)plcphdr));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_RADIO, &radio, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n radio for the associated AP is = %d \n", (int)radio));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_PHYTYPE, &phytype, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n phytype for the associated AP is = %d \n", (int)phytype));
    wiced_rtos_delay_microseconds(1000);
    //if(wwd_wifi_get_ioctl_value( WLC_GET_FIXRATE, &fixrate, WWD_STA_INTERFACE )!=WWD_SUCCESS)
    //    WPRINT_APP_INFO(("\n fixrate for the associated AP is = %d \n", (int)fixrate));
    //wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_KEY, &key, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n key for the associated AP is = %d \n", (int)key));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_REGULATORY, &regulatory, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n regulatory for the associated AP is = %d \n", (int)regulatory));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_ROAM_TRIGGER, &roam_trigger, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n roam trigger for the associated AP is = %d \n", (int)roam_trigger));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_ROAM_DELTA, &roam_delta, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n roam delta for the associated AP is = %d \n", (int)roam_delta));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_TXANT, &txant, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n txant for the associated AP is = %d \n", (int)txant));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_ANTDIV, &antdiv, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n antdiv for the associated AP is = %d \n", (int)antdiv));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_CLOSED, &closed, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n closed for the associated AP is = %d \n", (int)closed));
    wiced_rtos_delay_microseconds(1000);\
    if(wwd_wifi_get_ioctl_value( WLC_GET_BCNPRD, &bcnprd, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n bcnprd for the associated AP is = %d \n", (int)bcnprd));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_DTIMPRD, &dtimprd, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n dtimprd for the associated AP is = %d \n", (int)dtimprd));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_SROM, &srom, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n SROM for the associated AP is = %d \n", (int)srom));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_COUNTRY, &country, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n country for the associated AP is = %d \n", (int)country));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_PM, &pm, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n pm for the associated AP is = %d \n", (int)pm));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_FORCELINK, &forcelink, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n forcelink for the associated AP is = %d \n", (int)forcelink));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_PHYREG, &phyreg, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n phyreg for the associated AP is = %d \n", (int)phyreg));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_RADIOREG, &radioreg, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n radioreg for the associated AP is = %d \n", (int)radioreg));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_REVINFO, &revinfo, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n  for the associated AP is = %d \n", (int)revinfo));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_UCANTDIV, &ucantdiv, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n ucantdiv for the associated AP is = %d \n", (int)ucantdiv));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_RX_ANT, &rxant, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n rxant for the associated AP is = %d \n", (int)rxant));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_ATIM, &attim, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n atim for the associated AP is = %d \n", (int)attim));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_RSSI, &rssi_value, WWD_STA_INTERFACE )!=WWD_SUCCESS)
            WPRINT_APP_INFO(("\n RSSI for the associated AP is = %d dBm\n", (int)rssi_value));
    if(wwd_wifi_get_ioctl_value( WLC_GET_PHY_NOISE, &phy_noise, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n phy noise for the associated AP is = %d \n", (int)phy_noise));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_BSS_INFO, &bss_info, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n bss_info for the associated AP is = %d \n", (int)bss_info));
    wiced_rtos_delay_microseconds(1000);
    if(wwd_wifi_get_ioctl_value( WLC_GET_BANDLIST, &bandlist, WWD_STA_INTERFACE )!=WWD_SUCCESS)
        WPRINT_APP_INFO(("\n bandlist for the associated AP is = %d \n", (int)bandlist));
    wiced_rtos_delay_microseconds(1000);
}
