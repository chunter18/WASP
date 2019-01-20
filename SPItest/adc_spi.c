#include "wiced.h"

#define CS_DEASSERT         0 << 28
#define CS_ASSERT           1 << 28

#define TRANSFER_R_CMD          3 << 28
#define TRANSFER_BYTES_R(x)     (TRANSFER_R_CMD | (x & 0xF))

wiced_spi_device_t spi_device =
{
    .port               = WICED_SPI_1,
    .chip_select        = WICED_GPIO_22, //CS PIN IN 43097
    .speed              = 1000000,     //1mhz
    .mode               = ( SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_MSB_FIRST ),
    .bits               = 8
};

int32_t ad400x_init(struct ad400x_dev **device, struct ad400x_init_param init_param)

void application_start(void)
{
    uint32_t spi_eng_msg_cmds[3] = { CS_DEASSERT,
                                     TRANSFER_BYTES_R(2),
                                     CS_ASSERT
                                    };

    int32_t ret;
    ret = ad400x_init(&dev, ad400x_init_param);
}


/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad400x_init(struct ad400x_dev **device,
            struct ad400x_init_param init_param)
{
    struct ad400x_dev *dev;
    int32_t ret;
    uint8_t data = 0;

    dev = (struct ad400x_dev *)malloc(sizeof(*dev));
    if (!dev)
        return -1;

    ret = spi_init(&dev->spi_desc, init_param.spi_init);
    if (ret < 0)
        goto error;

    dev->dev_id = init_param.dev_id;
    dev->spi_desc->cs_delay = init_param.spi_init.cs_delay;

    dev->spi_desc->max_data_width = init_param.num_bits;

    spi_set_transfer_length(dev->spi_desc, 16);
    ad400x_spi_reg_read(dev, &data);

    spi_set_transfer_length(dev->spi_desc, dev->spi_desc->max_data_width);
    data = AD400X_TURBO_MODE(init_param.turbo_mode) |
           AD400X_HIGH_Z_MODE(init_param.high_z_mode) |
           AD400X_SPAN_COMPRESSION(init_param.span_compression) |
           AD400X_EN_STATUS_BITS(init_param.en_status_bits);
    ret = ad400x_spi_reg_write(dev, data);
    if (ret < 0)
        goto error;

    *device = dev;

    return ret;

error:
    ad400x_remove(dev);
    return ret;
}

int32_t spi_init()
{

    uint32_t        data_width;

    //TODO WICED SPI INIT HERE!!!!!!!!!!

    // perform a reset
    spi_eng_write(desc, SPI_ENGINE_REG_RESET, 0x01);
    usleep(100000); //100 millisec
    spi_eng_write(desc, SPI_ENGINE_REG_RESET, 0x00);

    spi_eng_read(desc, SPI_ENGINE_REG_DATA_WIDTH, &data_width);
    desc->max_data_width = data_width;
    spi_set_transfer_length(desc, desc->max_data_width);

    *descriptor = desc;
    return 0;
}



int32_t spi_eng_write(spi_desc *desc, uint32_t reg_addr, uint32_t ui32data)
{
    Xil_Out32((desc->spi_baseaddr + reg_addr), ui32data);

    return 0;
}

int32_t spi_eng_read(spi_desc *desc, uint32_t reg_addr, uint32_t *reg_data)
{
    *reg_data = Xil_In32((desc->spi_baseaddr + reg_addr));

    return 0;
