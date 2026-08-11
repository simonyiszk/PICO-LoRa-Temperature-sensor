#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "datatypedef.h"

#define I2C_PORT i2c1
#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7
#define I2C_ADDR_H 0x5C
#define I2C_ADDR_L 0x23 //will use this, ADDR pin needs to be pulled down

// void lightsensor_init(){
//     uint8_t mode = 0b00100000;
//     i2c_write_blocking(I2C_PORT, I2C_ADDR_L, &mode, 1, true); //H-res one time read setting
// }

// void lightsensor_read(uint8_t* dest){
//     // uint8_t raw[2];
//     i2c_read_blocking(I2C_PORT, I2C_ADDR_L, dest, 2, false);
// }

// uint16_t lightsensor_cycle(){
//     lightsensor_init();
//     sleep_ms(240);
//     uint8_t raw[2];
//     lightsensor_read(raw);
//     return (uint16_t)((raw[0] << 8 + raw[1]) / 1.2);
// }

uint16_t lightsensor_cycle(void)
{
    uint8_t command = 0x20; //H-res one time read setting
    uint8_t raw[2];

    // Start one-time H-resolution measurement
    i2c_write_blocking(I2C_PORT, I2C_ADDR_L, &command, 1, false);

    // Maximum H-resolution measurement time
    sleep_ms(180);

    // Read result
    i2c_read_blocking(I2C_PORT, I2C_ADDR_L, raw, 2, false);

    uint16_t value = ((uint16_t)raw[0] << 8) | raw[1];

    return (uint16_t)(value / 0.012f);
    //az kéne hogy csak a value-t  továbbítom.
}