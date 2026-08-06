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
