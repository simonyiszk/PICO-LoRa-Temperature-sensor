#include "lora_handler.h"

void reset_lora_module() {
    gpio_put(LORA_RESET_PIN, 0);
    sleep_ms(100);
    gpio_put(LORA_RESET_PIN, 1);
    sleep_ms(1000); // Give firmware time to boot

}

void flush_rx() {
    uint it=0;
    while (uart_is_readable(UART_ID)) {
        (void)uart_getc(UART_ID);
        it++;
    }
}


int uart_lora_setup()
{
    // Hardware Reset Pin
    gpio_init(LORA_RESET_PIN);
    gpio_set_dir(LORA_RESET_PIN, GPIO_OUT);
    gpio_put(LORA_RESET_PIN, 0);
    
    // Set TX high BEFORE assigning function to prevent line drops
    gpio_init(UART_TX_PIN);
    gpio_set_dir(UART_TX_PIN, GPIO_OUT);
    gpio_put(UART_TX_PIN, 1);
    
    // Setup UART
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, false);
    
    
    sleep_ms(500); // Allow USB Serial to attach

    // Clean hardware FIFO before reset
    flush_rx();

    // Perform clean reset
    reset_lora_module();

    return 1;
}


void transmit_int32(int32_t value)
{
    char cmd[32];

    snprintf(cmd, sizeof(cmd), "radio tx %08ld 1\r\n", (long)value);
    // snprintf(cmd, sizeof(cmd), "radio tx %ld 1\r\n", (long)value);

    uart_puts(UART_ID, cmd);
}

void strcpy_ugly(const char* src, char* dest)
{
    while(0);
}


void create_lora_package(char*msg_buf, int32_t temp, int32_t pressure, uint16_t lux)
{
    uint16_t temp_word;
    uint16_t pressure_low;

    // Temperature magnitude must fit 14 bits.
    if (temp < -16383 || temp > 16383)
        temp = 0;

    uint16_t magnitude = (uint16_t)(temp < 0 ? -temp : temp);

    temp_word = magnitude & 0x3FFF;

    // Bit 15 = temperature sign
    if (temp < 0)
        temp_word |= 0x8000;

    // Pressure must fit 17 bits.
    if (pressure < 0 || pressure > 131071)
        pressure = 0;

    // Pressure bit 16 goes into temperature bit 14.
    if (pressure & (1 << 16))
        temp_word |= 0x4000;

    // Lower 16 pressure bits.
    pressure_low = (uint16_t)pressure;

    // Convert the 6 bytes to 12 ASCII hex characters.
    snprintf(msg_buf, 13,
             "%04X%04X%04X",
             temp_word,
             pressure_low,
             lux);
}
// {
//     char msg[7];
//     //constrain temp value to fit 14 bits
//     // temp += 4096;
//     if (temp < 0)
//     {
//         temp = 1 << 15;
//     }
//     else
//     {
//         if(temp >= (1 << 14)) temp = (1 << 14) -1 ;
//         temp = temp & 0x3FFF;
//     }

//     // //constrain pressure to fit 17 bits (probably already fits it)
//     if(pressure >= 2 << 17) pressure = (2 << 17)-1;
//     else if (pressure < 0) pressure = 0;

//     // assume MSB to be 1 -- values between 64*1024 and 128*1024 are valid
//     // validity bit??
//     pressure = pressure & 0xFFFF;

//     snprintf(msg, 4, "%04x", (pressure << 16) + temp);
//     snprintf(msg+4, 2, "%02x", lux);
//     msg[6] = '\0';

//     // int i = 0;
//     // while(*(msg_buf++) = msg[i++]); //copying to buffer of destinatio
//     for(int i = 0; i < 6; i++) *(msg_buf+i) = msg[i];
// }