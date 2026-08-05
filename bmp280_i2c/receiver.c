#include <windows.h>
#include <stdio.h>

int main() {
    // Change to your matching COM port (use "\\\\.\\COM10" format if port is COM10 or higher)
    const char *port_name = "COM3"; 
    
    HANDLE hSerial = CreateFile(port_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error opening serial port\n");
        return 1;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error getting device state\n");
        CloseHandle(hSerial);
        return 1;
    }

    // Update settings to match your LoRa module configuration
    dcbSerialParams.BaudRate = CBR_115200; 
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error setting serial port parameters\n");
        CloseHandle(hSerial);
        return 1;
    }

    printf("Listening for LoRa data on %s...\n", port_name);

    char read_buf[256];
    DWORD bytes_read;

    while (1) {
        memset(read_buf, 0, sizeof(read_buf));
        if (ReadFile(hSerial, read_buf, sizeof(read_buf) - 1, &bytes_read, NULL)) {
            if (bytes_read > 0) {
                printf("%s", read_buf);
            }
        }
        Sleep(10); // 10ms delay
    }

    CloseHandle(hSerial);
    return 0;
}
