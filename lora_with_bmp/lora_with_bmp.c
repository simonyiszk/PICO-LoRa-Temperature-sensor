
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
extern void i2c_oled_setup();


#define SAMPLE_CYCLE_AWAIT_MS 1000


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

    i2c_init(I2C_ID, 400 * 1000);
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
        bmp280_measure(&temperature, &pressure, &params);

        char temp_display[32];
        char pressure_display[32];
        snprintf(temp_display, 10, "%.2f °C", temperature / 100.0);
        snprintf(pressure_display, 12, "%.3f kPa", pressure / 1000.0);

        oled_draw_string(10, 2, temp_display);
        oled_draw_string(10, 4, pressure_display);
        oled_update();

        transmit_int32('t');
        sleep_ms(1000);
        transmit_int32(temperature);
        printf("%.3f\n", temperature / 1000.f);
        printf("temperature: %08ld\n", temperature);
        sleep_ms(1000);
        transmit_int32('p');
        sleep_ms(1000);
        transmit_int32(pressure);
        printf("pressure: %08ld\n", pressure);
        //sleep_ms(1000);
        //transmit_int32((int32_t)battery_level());

        sleep_ms(SAMPLE_CYCLE_AWAIT_MS);
        led_toggle();
    }

    return 0;
}