#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include <stdlib.h>

#define I2C_PORT i2c0
#define I2C_PORT_B i2c1
#define I2C_FREQ 400000
#define ADS1115_I2C_ADDR 0x48
//ADS A
const uint8_t SDA_PIN = 16;
const uint8_t SCL_PIN = 17;
//ADS B
const uint8_t SDA_PIN_B = 14;
const uint8_t SCL_PIN_B = 15;
//
static const uint8_t ADS1115_POINTER_CONVERSION = 0x00;
static const uint8_t ADS1115_POINTER_CONFIGURATION = 0x01; 
/***************************
 * Function Declarations*/

/* */
typedef struct ads1115_adc {
    i2c_inst_t *i2c_port;
    uint8_t i2c_addr;
    uint16_t config;
} ads1115_adc_t;

void ads1115_read_config(ads1115_adc_t *adc);
void ads1115_read_adc(ads1115_adc_t *adc,uint16_t *adc_valeu, uint8_t chanel);
void ads1115_init(i2c_inst_t *i2c_port, uint8_t i2c_addr,ads1115_adc_t *adc);

void ads1115_init(i2c_inst_t *i2c_port, uint8_t i2c_addr,
                  ads1115_adc_t *adc) {
    adc->i2c_port = i2c_port;
    adc->i2c_addr = i2c_addr;
    ads1115_read_config(adc);
}
void ads1115_read_config(ads1115_adc_t *adc){
    // Default configuration after power up should be 34179.
    // Default config with bit 15 cleared is 1411
    uint8_t dst[2];
    i2c_write_blocking(adc->i2c_port, adc->i2c_addr,
                       &ADS1115_POINTER_CONFIGURATION, 1, true);
    i2c_read_blocking(adc->i2c_port, adc->i2c_addr, &dst, 2,
                      false);
    adc->config = (dst[0] << 0x08) | dst[1];
    
}

void ads1115_read_adc( ads1115_adc_t *adc, uint16_t *adc_valeu, uint8_t chanel){
    // If mode is single-shot, set bit 15 to start the conversion.
    
    ads1115_read_config(adc);
    adc->config &= ~(0x07<<0x0c);//Clear the MUX
    adc->config &= ~(0x07<<0x09);//Clear the PGA

    adc->config |= (0x07 & (0x04 + chanel))<<0x0c;
    adc->config |= (0x01<<0x0f);
    adc->config |= (0x01<<0x09);
    uint8_t src[3];
    src[0] = ADS1115_POINTER_CONFIGURATION;
    src[1] = (uint8_t)(adc->config >> 0x08);
    src[2] = (uint8_t)(adc->config & 0xff);
    i2c_write_blocking(adc->i2c_port, adc->i2c_addr, &src, 3,false);
    sleep_ms(8);
    ads1115_read_config(adc);
     while (adc->config & 0x8000 == 0 ){
            ads1115_read_config(adc);
        }
    // Now read the value from last conversion
    uint8_t dst[2];
    i2c_write_blocking(adc->i2c_port, adc->i2c_addr,&ADS1115_POINTER_CONVERSION, 1, true);
    i2c_read_blocking(adc->i2c_port, adc->i2c_addr, &dst, 2,false);
    sleep_ms(8);
    *adc_valeu = (dst[0] << 8) | dst[1];
    
}

struct ads1115_adc adc_A;
struct ads1115_adc adc_B;
int main()
{
    stdio_init_all();
    //Inicializar ADS A
    i2c_init(I2C_PORT, I2C_FREQ);
    //Inicializa ADS B
    i2c_init(I2C_PORT_B, I2C_FREQ);
    /*
    Seteo de ADS A
    */
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    /*
    Inicializa ADS B
    */
    gpio_set_function(SDA_PIN_B, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN_B, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN_B);
    gpio_pull_up(SCL_PIN_B);
    // Initialise ADC
    ads1115_init(I2C_PORT, ADS1115_I2C_ADDR, &adc_A);
    ads1115_init(I2C_PORT_B, ADS1115_I2C_ADDR, &adc_B);
    uint16_t ADS_A[4];
    uint16_t ADS_B[4];
    while(1){
        
        ads1115_read_adc( &adc_A,&ADS_A[0],  0x00 );
        ads1115_read_adc( &adc_A,&ADS_A[1],  0x01 );
        ads1115_read_adc( &adc_A,&ADS_A[2],  0x02 );
        ads1115_read_adc( &adc_A,&ADS_A[3],  0x03 );
        

        ads1115_read_adc( &adc_A,&ADS_B[0],  0x00 );
        ads1115_read_adc( &adc_A,&ADS_B[1],  0x01 );
        ads1115_read_adc( &adc_A,&ADS_B[2],  0x02 );
        ads1115_read_adc( &adc_A,&ADS_B[3],  0x03 );
        printf("ADSA 0: %d ADSA 1: %d ADSA 2: %d ADSA 3: %d \n",ADS_A[0],ADS_A[1],ADS_A[2],ADS_A[3]);
        printf("ADSB 0: %d ADSB 1: %d ADSB 2: %d ADSB 3: %d \n",ADS_B[0],ADS_B[1],ADS_B[2],ADS_B[3]);
        sleep_ms(100);
    }
} 
