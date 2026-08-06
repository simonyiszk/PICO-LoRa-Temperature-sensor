#include "oled96.h"

static void oled_send_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
}

void oled_init(void) {
    uint8_t cmds[] = {
        0xAE,       // Display Off
        0xD5, 0x80, // Set Display Clock Divide Ratio
        0xA8, 0x3F, // Set Multiplex Ratio (64-1)
        0xD3, 0x00, // Set Display Offset
        0x40,       // Set Start Line at 0
        0x8D, 0x14, // Enable Charge Pump
        0x20, 0x00, // Memory Addressing Mode: Horizontal
        0xA1,       // Segment Re-map (A0/A1 flip horizontal)
        0xC8,       // COM Output Scan Direction (C0/C8 flip vertical)
        0xDA, 0x02, // Set COM Pins Hardware Configuration
        0x81, 0xCF, // Set Contrast Control
        0xD9, 0xF1, // Set Pre-charge Period
        0xDB, 0x40, // Set VCOMH Deselect Level
        0xA4,       // Entire Display ON (Resume to RAM)
        0xA6,       // Normal Display (A7 for Inverted)
        0xAF        // Display On
    };

    for (size_t i = 0; i < sizeof(cmds); i++) {
        oled_send_cmd(cmds[i]);
    }
}

void oled_update(void) {
    oled_send_cmd(0x21); // Set Column Address
    oled_send_cmd(0);
    oled_send_cmd(OLED_WIDTH - 1);

    oled_send_cmd(0x22); // Set Page Address
    oled_send_cmd(0);
    oled_send_cmd((OLED_HEIGHT / 8) - 1);

    uint8_t temp[OLED_BUF_LEN + 1];
    temp[0] = 0x40; // Co = 0, D/C# = 1 (Data byte header)
    memcpy(&temp[1], oled_buffer, OLED_BUF_LEN);

    i2c_write_blocking(I2C_PORT, OLED_ADDR, temp, sizeof(temp), false);
}

void oled_clear(void) {
    memset(oled_buffer, 0x00, sizeof(oled_buffer));
}

void oled_draw_char(int x, int page, char c) {
    if (x < 0 || x > OLED_WIDTH - 6 || page < 0 || page > 7) return;

    // Convert lowercase to uppercase for this minimal font
    if (c >= 'a' && c <= 'z') c -= 32;
    if (c < ' ' || c > 'Z') c = ' ';

    int idx = c - ' ';
    for (int i = 0; i < 5; i++) {
        oled_buffer[page * OLED_WIDTH + x + i] = font5x7[idx][i];
    }
    oled_buffer[page * OLED_WIDTH + x + 5] = 0x00; // 1-px space between chars
}

void oled_draw_string(int x, int page, const char *str) {
    while (*str && x <= OLED_WIDTH - 6) {
        oled_draw_char(x, page, *str++);
        x += 6;
    }
}

void i2c_oled_setup(void) {
    // stdio_init_all();

    // Initialize I2C peripheral at 400kHz
    // i2c_init(I2C_PORT, 400 * 1000);
    // gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA_PIN);
    // gpio_pull_up(I2C_SCL_PIN);

    oled_init();
    oled_clear();

    oled_update();
}