#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int battery_level() {
    // 1. Initialize the ADC hardware
    adc_init();
    // 2. Select ADC Input 3 (GPIO 29 / VSYS)
    adc_select_input(3);
    // Conversion factors
    const float conversion_factor = 3.3f / (121); // 12-bit ADC max value is 4095
    const float voltage_divider = 3.0f;           // Internal 3:1 divider on VSYS
    // LiPo voltage limits
    const float v_max = 4.2f;
    const float v_min = 3.3f;

    // 3. Read the 12-bit raw value (0 - 4095)
    uint16_t raw_value = adc_read();

    // 4. Calculate actual battery voltage
    float voltage = raw_value * conversion_factor * voltage_divider;

    // 5. Calculate percentage and clamp it between 0.0 and 100.0
    float percentage = ((voltage - v_min) / (v_max - v_min)) * 100.0f;
    if (percentage > 100.0f) percentage = 100.0f;
    if (percentage < 0.0f) percentage = 0.0f;

     // Print results to serial monitor
    printf("Voltage: %.2f V | Battery: %.1f%%\n", voltage, percentage);

    return percentage;

}
