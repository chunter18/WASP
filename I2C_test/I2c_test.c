#include "wiced.h"


#define LTC2941_SLAVE_ADDR         (0x64)
#define LTC2941_SLAVE_ADDR_BINARY  (0b1100100) //7 bit addr
#define LTC2941_CONFIG_BYTE        (0b11111100)

//these are the internal resistor values
#define RESISTOR_VAL_OHMS           0.05
#define RESISTOR_VAL_MILLIOHMS      50

#define NUM_I2C_MESSAGE_RETRIES     (3)

//sixteenBitNumber = (upperByte<<8) | lowerByte;

/*
 * reg map
 * addr     name    desc            readonly    default
 * 0x00     A       status              y       0x80
 * 0x01     B       control             N       0x7F
 * 0x02     C       accumchrg msb       N       0xFF
 * 0x03     D       accumchrg lsb       N       0xFF
 * 0x04     E       chrgthrsh hi msb    N       0xFF
 * 0x05     F       chrgthrsh hi lsb    N       0xFF
 * 0x06     G       chrgthrsh lo msb    N       0x00
 * 0x07     G       chrgthrsh lo lsb    N       0x00
 */

/*
 * Status reg A bit enumeration
 * Bit     name         operation               default
 * A[7]    Chip ID      1=LTC2491                   1
 * A[6]     not used
 * A[5]     accumulatedd charge over/underflow      0
 * a[4]     not used
 * a[3]     charge alert hight                      0
 * a[2]     charge alert low                    `   0
 * a[1]     vbat alert                              0
 * a[0]     undervoatge lockout alert               X
 */

/*
 * control reg B bit enumeration
 * Bit      Name            op              default
 * B[7:6]   Vbat alert      11=3v thresh    00
 *                          10=2.9v thresh
 *                          01=2.8v thresh
 *                          00=vbat off
 *B[5:3]    prescaler M     eq              111 (128)
 *                   M=2^(4*B[5] + 2*B[4] + B[3])
 *B[2:1]    AL/CC config    10=alert mode   10
 *B[2:1]                    01=charge complete mode
 *                          00=pin disabled
 *                          11=not allowed
 *b[0]      shutdown        shutdown analog    0
 *                          section (1=shutdown)
 */

wiced_result_t LTC2941_init();
uint8_t LTC2941_1reg_read(uint8_t regaddr);
wiced_result_t LTC2941_1reg_write(uint8_t regaddr, uint8_t write_byte);
float LTC2941_code_to_coulombs(uint16_t adc_code, uint16_t prescalar);
float LTC2942_code_to_mAh(uint16_t adc_code, uint16_t prescalar );
float LTC2942_code_to_voltage(uint16_t adc_code);

static wiced_i2c_device_t i2c_device_gas_gauge =
{
        .port = WICED_I2C_1, //we are using i2c_0 for wasp, denoted as I2C_1 in the makefiles
        .address = LTC2941_SLAVE_ADDR_BINARY,   //hardcoded slave address
        .address_width = I2C_ADDRESS_WIDTH_7BIT,
        .speed_mode = I2C_STANDARD_SPEED_MODE, //ltc2491 supports 100 and 400khz
                                               //we will be using 100 because its plenty fast
};


//these are ltc2942 values - do they need to be changed?
const float LTC2941_CHARGE_lsb = 0.085E-3;
const float LTC2941_VOLTAGE_lsb = .3662E-3;
const float LTC2941_FULLSCALE_VOLTAGE = 6;



void application_start(void)
{

    wiced_init();


    LTC2941_init();

    //TODO - write code to check battery level
    //This will require a multireg read function
}



wiced_result_t LTC2941_init()
{
    //intiializing the gas guage part. To do this we first need to initilize the i2c interface.
    /* Initialize I2C */
    WPRINT_APP_INFO( ( "I2C Initialization\n" ) );
    if ( wiced_i2c_init( &i2c_device_gas_gauge ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "I2C Initialization Failed\n" ) );
        return WICED_ERROR;
    }

    //with the i2c bus up, we can now init the actual device
    //we do so by reading the status reg at add 0x00 and checking to see if it aligns
    //with what we are expecting.

    uint8_t reg_content;
    reg_content = LTC2941_1reg_read(0x00);
    if(reg_content == 0x80)
    {
        WPRINT_APP_INFO( ( "LTC2491 connected, initialization succeeded. bringing up AL/CC\n" ) );
    }
    else
    {
        WPRINT_APP_INFO( ( "Status reg returned bad arg: 0x%x\n",reg_content ) );
    }

    //next we need to set up the vbat alert.
    //we need to write to the control reg that alert is on, and the level is 3v
    //binary format of that write is 11111100 - stored in LTC2941_config_byte

    if ( LTC2941_1reg_write(0x01, LTC2941_CONFIG_BYTE) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "failed to bring up AL/CC\n" ) );
        return WICED_ERROR;
    }

    return WICED_SUCCESS;

}

uint8_t LTC2941_1reg_read(uint8_t regaddr)
{
    wiced_result_t result;

    uint8_t data = 0; //read contents go here

    //send the address of the reg we want to read
    result = wiced_i2c_write( &i2c_device_gas_gauge, WICED_I2C_START_FLAG, &regaddr, 1 );
    //read the reg contents from the device
    result = wiced_i2c_read( &i2c_device_gas_gauge,WICED_I2C_REPEATED_START_FLAG | WICED_I2C_STOP_FLAG, &data, 1 );

    if(result == WICED_SUCCESS)
    {
        return data;
    }
    else
    {
        WPRINT_APP_ERROR( ( "I2C 1 reg read failed\n"));
        return -1;
    }
}

uint8_t LTC2941_nreg_read(uint8_t first_regaddr, int n)
{
    wiced_result_t result;

    uint8_t data = 0; //read contents go here

    //send the address of the reg we want to read
    result = wiced_i2c_write( &i2c_device_gas_gauge, WICED_I2C_START_FLAG, &first_regaddr, 1 );
    //read the reg contents from the device
    result = wiced_i2c_read( &i2c_device_gas_gauge,WICED_I2C_REPEATED_START_FLAG | WICED_I2C_STOP_FLAG, &data, 1 );

    if(result == WICED_SUCCESS)
    {
        return data;
    }
    else
    {
        WPRINT_APP_ERROR( ( "I2C 1 reg read failed\n"));
        return -1;
    }
}

wiced_result_t LTC2941_1reg_write(uint8_t regaddr, uint8_t write_byte)
{
    wiced_result_t result;
    uint8_t cmd[2] = {regaddr, write_byte};

    //first, write the actual sequence we want to write (reg address, byte to go into that reg).
    result = wiced_i2c_write( &i2c_device_gas_gauge, WICED_I2C_START_FLAG | WICED_I2C_STOP_FLAG, &cmd, 2 );

    //next we want to read from that very same reg to confirm it was written correctly.
    uint8_t data = 0;
    data = LTC2941_1reg_read(regaddr);
    if(data == write_byte)
        return WICED_SUCCESS;
    else
        return WICED_ERROR;
}



float LTC2941_code_to_coulombs(uint16_t adc_code, uint16_t prescalar)
// The function converts the 16-bit RAW adc_code to Coulombs
{
    float coulomb_charge;
    coulomb_charge =  1000*(float)(adc_code*LTC2941_CHARGE_lsb*prescalar*50E-3)/(RESISTOR_VAL_OHMS*128);
    coulomb_charge = coulomb_charge*3.6f;
    return(coulomb_charge);
}

float LTC2942_code_to_mAh(uint16_t adc_code, uint16_t prescalar )
// The function converts the 16-bit RAW adc_code to mAh
{
  float mAh_charge;
  mAh_charge = 1000*(float)(adc_code*LTC2941_CHARGE_lsb*prescalar*50E-3)/(RESISTOR_VAL_OHMS*128);
  return(mAh_charge);
}

float LTC2942_code_to_voltage(uint16_t adc_code)
// The function converts the 16-bit RAW adc_code to Volts
{
  float voltage;
  voltage = ((float)adc_code/(65535))*LTC2941_FULLSCALE_VOLTAGE;
  return(voltage);
}
