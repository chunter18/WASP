#include "wiced.h"
#include <math.h>


#define LTC2941_SLAVE_ADDR         (0x64)
#define LTC2941_SLAVE_ADDR_BINARY  (0b1100100) //7 bit addr
#define LTC2941_CONFIG_BASE        (0b11000100)

//these are the internal resistor values
#define RESISTOR_VAL_OHMS           0.05
#define RESISTOR_VAL_MILLIOHMS      50

#define NUM_I2C_MESSAGE_RETRIES     (3)

//#define BATTERY_CAPACITY 100  //mah
//#define BATTERY_CAPACITY 500  //mah
//#define BATTERY_CAPACITY 1000 //mah
//#define BATTERY_CAPACITY 1500 //mah
//#define BATTERY_CAPACITY 2000 //mah
#define BATTERY_CAPACITY 2500   //mah

#define LTC2941_PRESCALAR_M_1                   0x00
#define LTC2941_PRESCALAR_M_2                   0x08
#define LTC2941_PRESCALAR_M_4                   0x10
#define LTC2941_PRESCALAR_M_8                   0x18
#define LTC2941_PRESCALAR_M_16                  0x20
#define LTC2941_PRESCALAR_M_32                  0x28
#define LTC2941_PRESCALAR_M_64                  0x30
#define LTC2941_PRESCALAR_M_128                 0x38

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

wiced_result_t LTC2941_init_startup();
wiced_result_t LTC2941_init();
uint8_t LTC2941_1reg_read(uint8_t regaddr);
wiced_result_t LTC2941_1reg_write(uint8_t regaddr, uint8_t write_byte);
uint16_t LTC2941_2reg_read(uint8_t first_regaddr);
wiced_result_t LTC2941_2reg_write(uint8_t first_regaddr, uint16_t write_short);
float LTC2941_get_coulombs(uint16_t prescalar);
float LTC2941_get_mAh(uint16_t prescalar );
float LTC2942_get_voltage();
uint8_t find_prescalar();
uint8_t battery_percentage(uint16_t prescalar);

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
    wiced_result_t result;
    uint16_t prescaler = 128;
    float voltage, mah, coulombs;
    wiced_init();

    prescaler = find_prescalar();
    WPRINT_APP_INFO(( "m = %d\n", prescaler));


    //try to bring it up a few times if it doesnt work.
    //wont always work the first time.
    for(int i = 0; i < 3; i++)
    {
        result = LTC2941_init_startup();
        if(result == WICED_SUCCESS)
            break;
    }
    if( result == WICED_ERROR)
        return; //really we would go to hibernate with a wakeup time of like 10 seconds?

    //weve initialized.

    coulombs = LTC2941_get_coulombs(prescaler);
    mah = LTC2941_get_mAh(prescaler);
    //voltage =  LTC2942_get_voltage(); - this function isnt meant for ltc2941
    //the value is walways bad so DONT USE IT EVER
    WPRINT_APP_INFO(( "Coulombs = %f\n",coulombs));
    WPRINT_APP_INFO(( "mah = %f\n", mah));
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
        WPRINT_APP_INFO( ( "LTC2491 connected, initialization succeeded. \n" ) );
        return WICED_SUCCESS;
    }
    else
    {
        WPRINT_APP_INFO( ( "Status reg returned bad arg: 0x%x\n",reg_content ) );
        return WICED_ERROR;
    }
}

wiced_result_t LTC2941_init_startup()
{
    uint8_t ltc2941_config_byte = 0;
    wiced_result_t result;

    result = LTC2941_init();
    if(result != WICED_SUCCESS)
        return result;

    WPRINT_APP_INFO( ( "First time startup, bringing up AL/CC. \n" ) );

    //next we need to set up the vbat alert.
    //we need to write to the control reg that alert is on, and the level is 3v
    //binary format of that write is 11_XXX_10_0 - stored in LTC2941_config_byte
    //xxx is the prescaler bits

    uint8_t m = find_prescalar();
    switch(m)
    {
        case (1):
            ltc2941_config_byte = LTC2941_PRESCALAR_M_1|LTC2941_CONFIG_BASE;
            break;
        case (2):
            ltc2941_config_byte = LTC2941_PRESCALAR_M_2|LTC2941_CONFIG_BASE;
            break;
        case (4):
            ltc2941_config_byte = LTC2941_PRESCALAR_M_4|LTC2941_CONFIG_BASE;
            break;
        case (8):
            ltc2941_config_byte = LTC2941_PRESCALAR_M_8|LTC2941_CONFIG_BASE;
            break;
        case (16):
            ltc2941_config_byte = LTC2941_PRESCALAR_M_16|LTC2941_CONFIG_BASE;
            break;
        case (32):
            ltc2941_config_byte = LTC2941_PRESCALAR_M_32|LTC2941_CONFIG_BASE;
            break;
        case (64):
            ltc2941_config_byte = LTC2941_PRESCALAR_M_64|LTC2941_CONFIG_BASE;
            break;
        case (128):
            ltc2941_config_byte = LTC2941_PRESCALAR_M_128|LTC2941_CONFIG_BASE;
            break;
    }

    WPRINT_APP_INFO( ( "config byte =  0x%x\n",ltc2941_config_byte ) );

    if ( LTC2941_1reg_write(0x01, ltc2941_config_byte) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "failed to bring up AL/CC\n" ) );
        return WICED_ERROR;
    }

    //getting adc_code to write to the registers
    float regval;
    uint16_t write_code;
    uint8_t msb, lsb;
    regval = BATTERY_CAPACITY*(RESISTOR_VAL_OHMS*128)/(LTC2941_CHARGE_lsb*m*50E-3*1000.0);
    //mAh_charge = 1000*(float)(adc_code*LTC2941_CHARGE_lsb*prescalar*50E-3)/(RESISTOR_VAL_OHMS*128);
    regval = roundf(regval);
    if(regval > 65535)
        write_code = 0xFF;
    else
        write_code = (uint16_t)regval;
    WPRINT_APP_INFO(("regval = %lf, writing %d\n",regval, write_code));

    msb = (uint8_t)((write_code & 0xFF00) >> 8);
    lsb = (uint8_t)(write_code & 0x00FF);
    WPRINT_APP_INFO(( "16->0x%x msb->0x%x, lsb->0x%x\n",write_code, msb, lsb));

    result = LTC2941_2reg_write(0x02, regval);
    if(result != WICED_SUCCESS)
        return result;

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

uint16_t LTC2941_2reg_read(uint8_t first_regaddr)
{
    wiced_result_t result;

    uint8_t data[2] = {0,0}; //read contents go here
    uint16_t sample = 0;

    //send the address of the reg we want to read
    result = wiced_i2c_write( &i2c_device_gas_gauge, WICED_I2C_START_FLAG, &first_regaddr, 2 );
    //read the reg contents from the device
    result = wiced_i2c_read( &i2c_device_gas_gauge,WICED_I2C_REPEATED_START_FLAG | WICED_I2C_STOP_FLAG, &data, 2 );

    if(result == WICED_SUCCESS)
    {
        sample = ((uint16_t)data[0] << 8) | data[1];
        return sample;
    }
    else
    {
        WPRINT_APP_ERROR( ( "I2C 2 reg read failed\n"));
        return -1;
    }
}

wiced_result_t LTC2941_1reg_write(uint8_t regaddr, uint8_t write_byte)
{
    uint8_t cmd[2] = {regaddr, write_byte};

    //first, write the actual sequence we want to write (reg address, byte to go into that reg).
    wiced_i2c_write( &i2c_device_gas_gauge, WICED_I2C_START_FLAG | WICED_I2C_STOP_FLAG, &cmd, 2 );

    //next we want to read from that very same reg to confirm it was written correctly.
    uint8_t data = 0;
    data = LTC2941_1reg_read(regaddr);
    if(data == write_byte)
        return WICED_SUCCESS;
    else
        return WICED_ERROR;
}

wiced_result_t LTC2941_2reg_write(uint8_t first_regaddr, uint16_t write_short)
{
    uint8_t msb = (uint8_t)((write_short & 0xFF00) >> 8);
    uint8_t lsb = (uint8_t)(write_short & 0x00FF);
    uint8_t cmd[3] = {first_regaddr, msb, lsb};

    wiced_i2c_write( &i2c_device_gas_gauge, WICED_I2C_START_FLAG | WICED_I2C_STOP_FLAG, &cmd, 3 );

    uint16_t rec = LTC2941_2reg_read(first_regaddr);

    if(rec == write_short)
        return WICED_SUCCESS;
    else
        return WICED_ERROR;
}



float LTC2941_get_coulombs(uint16_t prescalar)
{
    uint16_t adc_code = 0;
    adc_code = LTC2941_2reg_read(0x02);
    float coulomb_charge;
    float mah;
    float qlsb = (LTC2941_CHARGE_lsb*50E-3*prescalar)/(RESISTOR_VAL_OHMS*128); //qlsb from datasheet
    mah =  1000*(float)adc_code*qlsb; //colombs - need to be scaled back to mah from amp hours
    coulomb_charge = mah*3.6f; //1mah = 3.6 coulombs
    return(coulomb_charge);
}

float LTC2941_get_mAh(uint16_t prescalar )
{
  uint16_t adc_code = 0;
  adc_code = LTC2941_2reg_read(0x02);
  float mAh_charge;
  //see get_colombs for a better explanation
  WPRINT_APP_ERROR( ( "%d\n",adc_code));
  mAh_charge = 1000*(float)(adc_code*LTC2941_CHARGE_lsb*prescalar*50E-3)/(RESISTOR_VAL_OHMS*128);
  return(mAh_charge);
}

float LTC2942_get_voltage() //dont use
{
  uint16_t adc_code = 0;
  adc_code = LTC2941_2reg_read(0x02);
  float voltage;
  voltage = ((float)adc_code/(65535))*LTC2941_FULLSCALE_VOLTAGE;
  return(voltage);
}

uint8_t find_prescalar()
{
    int valid_m[8]       = {1,2,4,8,16,32,64,128};
    float differences[8] = {0,0,0,0,0, 0, 0, 0};

    float numerator = 128*(BATTERY_CAPACITY)*RESISTOR_VAL_OHMS;
    float denominator = 65536*(0.085)*50E-3;
    float result = numerator/denominator;

    //WPRINT_APP_INFO(( "m result calculated = %f\n", result));

    for(int i = 0; i < 8; i++)
    {
        differences[i] = result - valid_m[i];
    }

    int index = 0;
    for (int i = 0; i < 8; i++)
    {
        if (differences[i] < 0)
        {
            index = i;
            break;
        }
    }

    uint8_t m = valid_m[index];
    //WPRINT_APP_INFO(( "m chosen = %d\n", m));
    return m;
}

uint8_t battery_percentage(uint16_t prescalar)
{
    float mah = LTC2941_get_mAh(prescalar);
    return (uint8_t)(mah/BATTERY_CAPACITY);
}
