
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


//          FELHASZN�L�I BE�LL�T�SOK


#define DEVICE_ID   "123"        // 3 jegy? ID

#define LORA_FREQ   "868100000"  // Frekvencia (Hz)
#define LORA_BW     "125"        // S�vsz�less�g
#define LORA_SF     "sf7"        // Spreading Factor
#define LORA_CR     "4/5"        // Coding Rate
#define LORA_PWR    "14"         // Teljes�tm�ny
#define LORA_PA     "on"         // PA Boost
#define LORA_CRC    "on"         // CRC

#define LOOP_DELAY_MS 10000      // Id?z�t�s (ms)

//typedef unsigned int uint16_t;

void delay_ms(uint16_t ms)
{
    while(ms--) __delay_ms(1);
}

void UART2_SendString(const char *str)
{
    while(*str) {
        while(!EUSART2_IsTxReady());
        EUSART2_Write(*str++);
    }
}

//          F?program
void main(void)
{
    SYSTEM_Initialize();
    
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    TRISEbits.TRISE1 = 0; 
    LATEbits.LATE1 = 0; 

    // --- Sorsz�ml�l� v�ltoz� ---
    uint8_t packet_counter = 0;

    // --- 0. IND�T�SI K�SLELTET�S ---
    for(int i=0; i<10; i++) {
        LATEbits.LATE1 = 1; delay_ms(250);
        LATEbits.LATE1 = 0; delay_ms(250);
    }

    //          RESET
    delay_ms(2000);
    UART2_SendString("sys reset\r\n");
    delay_ms(2000);

    //          KONFIGUR�L�S
    UART2_SendString("radio set mod lora\r\n"); delay_ms(300);
    UART2_SendString("radio set freq " LORA_FREQ "\r\n"); delay_ms(300);
    UART2_SendString("radio set bw " LORA_BW "\r\n"); delay_ms(300);
    UART2_SendString("radio set sf " LORA_SF "\r\n"); delay_ms(300);
    UART2_SendString("radio set cr " LORA_CR "\r\n"); delay_ms(300);
    UART2_SendString("radio set pa " LORA_PA "\r\n"); delay_ms(300);
    UART2_SendString("radio set pwr " LORA_PWR "\r\n"); delay_ms(300);
    UART2_SendString("radio set crc " LORA_CRC "\r\n"); delay_ms(300);

    for(int i=0; i<6; i++) {
        LATEbits.LATE1 = !LATEbits.LATE1;
        delay_ms(100);
    }
    LATEbits.LATE1 = 0;

    //      M�R�S �S K�LD�S CIKLUS
    while(1)
    {
        //          A. M�r�s
        ADC_ChannelSelect(0);
        ADC_ConversionStart();
        while(!ADC_IsConversionDone());
        uint16_t adc = ADC_ConversionResultGet();

        float vout = ((float)adc / 4095.0f) * 3.3f;
        float temp_C = ((vout * 1000.0f) - 500.0f) / 10.0f;

        //          B. Sz�veg el?k�sz�t�se (EGYJEGY? SORSZ�MMAL!)
        char msg_buffer[40];
        
        // M�dos�t�s: %02u helyett %u lett a sorsz�mn�l
        // Form�tum: [ID][Sorsz�m][Adat] -> Pl: "1235T:25.5 C"
        sprintf(msg_buffer, "%s%uT:%.1f C", DEVICE_ID, packet_counter, (double)temp_C);

        // --- Sorsz�ml�l� l�ptet�se ---
        packet_counter++;
        if (packet_counter > 9) { // 0-9 k�z�tt tartjuk
            packet_counter = 0;
        }

        // --- C. K�ld�s ---
        LATEbits.LATE1 = 1;
        UART2_SendString("radio tx ");

        char hex_temp[4];
        for(int i = 0; msg_buffer[i] != '\0'; i++) {
            sprintf(hex_temp, "%02X", msg_buffer[i]);
            UART2_SendString(hex_temp);
        }

        UART2_SendString(" 1\r\n");
        
        delay_ms(100);
        LATEbits.LATE1 = 0;

        delay_ms(LOOP_DELAY_MS); 
    }
}