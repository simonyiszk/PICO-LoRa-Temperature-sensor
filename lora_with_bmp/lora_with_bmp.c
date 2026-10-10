
// #include <stdio.h>
// #include "hardware/i2c.h"
// #include "hardware/uart.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "lora_handler.h"
#include "bmp_handler.h"
extern int battery_level();
extern void oled_draw_string(int, int, const char*);
extern void oled_update();
extern void oled_clear();
extern void i2c_oled_setup();
extern uint16_t lightsensor_cycle();


#define SAMPLE_CYCLE_AWAIT_MS 2000

// start of blink initialization
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h" 
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 670
#endif

// Perform initialisation
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

void led_toggle(){
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
}

//end of blink initialization


int main() {
    stdio_init_all();
    uart_lora_setup();

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    struct bmp280_calib_param params;
    i2c_bmp280_setup(&params);
    i2c_oled_setup();
    pico_led_init();

    while(1){
        int32_t temperature = 0;
        int32_t pressure = 0;
        uint16_t light = 0;
        bmp280_measure(&temperature, &pressure, &params);
        light = lightsensor_cycle();

        char temp_display[32];
        char pressure_display[32];
        char light_data_display[32];
        oled_clear();
        snprintf(temp_display, 10, "%.2f °C\0", temperature / 100.0);
        snprintf(pressure_display, 12, "%.3f kPa\0", pressure / 1000.0);
        snprintf(light_data_display, 10, "%.2f lx\0", light / 100.0);

        oled_draw_string(10, 3, temp_display);
        oled_draw_string(10, 5, pressure_display);
        oled_draw_string(10, 7, light_data_display);
        oled_update();

        char cmd[64];
        char package[32];
        create_lora_package(package, temperature, pressure, light);

        snprintf(cmd, sizeof(cmd), "radio tx %s 1\r\n", package);
        // snprintf(cmd, sizeof(cmd), "radio tx %04X0123%04X0123%04X 1\r\n", temperature, pressure - (1 << 16), light);
        uart_puts(UART_ID, cmd);

        sleep_ms(SAMPLE_CYCLE_AWAIT_MS);
        //led_toggle();
    }

    return 0;
}