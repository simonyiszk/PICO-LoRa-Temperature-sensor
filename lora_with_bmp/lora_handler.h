#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "datatypedef.h"
#include <stdio.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define LORA_RESET_PIN 2


//user settings:

#define DEVICE_ID   "123"        // 3 char ID

#define LORA_FREQ   "868100000"  // freqency (Hz)
#define LORA_BW     "125"        // bandwidth
#define LORA_SF     "sf7"        // Spreading Factor
#define LORA_CR     "4/5"        // Coding Rate
#define LORA_PWR    "14"         // power
#define LORA_PA     "on"         // PA Boost
#define LORA_CRC    "on"         // CRC

#define LOOP_DELAY_MS 10000      // Id?z�t�s (ms)


// void delay_ms(uint16_t ms);

// void UART2_SendString(const char *str);

// void lora_test();

// void send_at_command(const char *cmd);

//new
void flush_rx();
void reset_lora_module();
int uart_lora_setup();
inline static void lora_transmit(const char* msg){

    while(*msg++) uart_putc(UART_ID, *msg);
    // uart_puts(UART_ID, msg);
    uart_puts(UART_ID, "\r\n");
}

void strcpy_xbytes(char* dest, char* src, uint8_t x);




void transmit_int32(int32_t value);