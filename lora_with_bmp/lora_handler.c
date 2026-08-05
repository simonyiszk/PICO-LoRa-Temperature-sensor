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

// void strcpy_xbytes(char* dest, char* src, uint8_t x){
//     uint32_t it = 0;
//     while(x--) dest[it] = src[it++];
// }

// void transmit_int32(int32_t value)
// {
//     uint8_t bytes[4];

//     bytes[0] = (value >> 24) & 0xFF;
//     bytes[1] = (value >> 16) & 0xFF;
//     bytes[2] = (value >> 8) & 0xFF;
//     bytes[3] = value & 0xFF;

//     char cmd[32];

//     char msg_start[] = "radio tx ";
//     char msg_end[] = " 1\r\n";
//     strcpy_xbytes(&cmd[0], &msg_start[0], 9);
//     // char* valuemsg = (char*)&value;
//     strcpy_xbytes(&cmd[9], (char*)&bytes, 4);
//     strcpy_xbytes(&cmd[13], &msg_end[0], 5);
//     uart_puts(UART_ID, cmd);
// }

void transmit_int32(int32_t value)
{
    char cmd[32];

    snprintf(cmd, sizeof(cmd), "radio tx %08ld 1\r\n", (long)value);
    // snprintf(cmd, sizeof(cmd), "radio tx %ld 1\r\n", (long)value);

    uart_puts(UART_ID, cmd);
}
