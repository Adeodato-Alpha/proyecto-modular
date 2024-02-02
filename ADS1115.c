#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include <stdlib.h>

#define I2C_PORT i2c0
#define I2C_FREQ 400000
#define ADS1115_I2C_ADDR 0x48
const uint8_t SDA_PIN = 16;
const uint8_t SCL_PIN = 17;
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
uint16_t ads1115_read_adc(ads1115_adc_t *adc, uint8_t chanel);
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
                       &ADS1115_POINTER_CONFIGURATION, 1, false);
    i2c_read_blocking(adc->i2c_port, adc->i2c_addr, &dst, 2,
                      false);
    adc->config = (dst[0] << 0x08) | dst[1];
    
}

uint16_t ads1115_read_adc( ads1115_adc_t *adc, uint8_t chanel){
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
    i2c_write_blocking(adc->i2c_port, adc->i2c_addr,&ADS1115_POINTER_CONVERSION, 1, false);
    i2c_read_blocking(adc->i2c_port, adc->i2c_addr, &dst, 2,false);
    sleep_ms(8);
    return (dst[0] << 8) | dst[0];
    
}

struct ads1115_adc adc_A;
int main()
{
    stdio_init_all();
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Initialise ADC
    ads1115_init(I2C_PORT, ADS1115_I2C_ADDR, &adc_A);
    uint16_t adc_valueA;
    uint16_t adc_valueB;
    uint16_t adc_valueC;
    uint16_t adc_valueD;
    while(1){
        adc_valueA = ads1115_read_adc( &adc_A,  0x00 );
        adc_valueB = ads1115_read_adc( &adc_A,  0x01 );
        adc_valueC = ads1115_read_adc( &adc_A,  0x02 );
        adc_valueD = ads1115_read_adc( &adc_A,  0x03 );
        printf("Valor adc 0: %d     Valor adc 1: %d        Valor adc 2: %d      Valor adc 3: %d \n",adc_valueA,adc_valueB,adc_valueC,adc_valueD);
        sleep_ms(100);
    }
    
}
