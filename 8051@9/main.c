/*#include <reg51.h>

// Rename the control bits so they don’t clash with existing macros:
sbit ADC_WR   = P3^6;   // ADC0804 WR pin
sbit ADC_RD   = P3^7;   // ADC0804 RD pin
sbit ADC_INTR = P3^2;   // ADC0804 INTR pin

#define ADC_DATA  P1    // Port 1 for ADC data lines D0–D7

void delay_us(void) {
    unsigned int i;
    for (i = 0; i < 5000; i++); // ~5 000-cycle delay
}

unsigned char read_adc(void) {
    unsigned char value;

    // Start conversion:
    ADC_WR = 0;        // Pull WR low
    delay_us();
    ADC_WR = 1;        // Release WR

    // Wait until INTR goes low:
    while (ADC_INTR == 1);

    // Read the converted 8-bit value:
    ADC_RD = 0;        // Pull RD low
    delay_us();
    value = ADC_DATA;  // Sample D0–D7 from Port 1
    ADC_RD = 1;        // Release RD

    return value;
}

void main(void) {
    unsigned char adc_val;

    // Make P1 an input port (all bits = 1)
    ADC_DATA = 0xFF;

    // Ensure Port 2 is configured as output for LEDs:
    P2 = 0x00;  // All LEDs off initially

    while (1) {
        adc_val = read_adc();
        P2 = adc_val;  // Show the 8-bit ADC result on LEDs (P2.0–P2.7)
    }
}
*/

#include <reg51.h>

#define ADC_DATA  P1        // Port 1 for ADC data bus
sbit ADC_WR   = P3^6;           // Start conversion
sbit ADC_RD   = P3^7;           // Read enable
sbit ADC_INTR = P3^2;           // Conversion complete flag

void delay_us(void) {
    unsigned int i;
    for (i = 0; i < 5000; i++);
}

unsigned char read_adc(void) {
    unsigned char value;
    ADC_WR = 0;         // Start A/D conversion
    delay_us();
    ADC_WR = 1;         // Release WR

    while (ADC_INTR == 1);  // Wait for INTR low

    ADC_RD = 0;         // Read mode
    delay_us();
    value = ADC_DATA;   // Latch data
    ADC_RD = 1;
    return value;
}

void UART_Init(void) {
    TMOD &= 0x0F;       // Clear Timer1 bits
    TMOD |= 0x20;       // Timer1, mode 2 (8-bit auto-reload)
    TH1 = 0xFD;         // 9600 baud @11.0592 MHz
    SCON = 0x50;        // Mode1, RI=0, TI=0, REN=1
    TR1 = 1;            // Start Timer1
    TI = 0; RI = 0;
}

void UART_SendByte(unsigned char b) {
    SBUF = b;
    while (TI == 0);
    TI = 0;
}

void UART_SendString(char *str) {
    while (*str) {
        UART_SendByte(*str++);
    }
}

void ByteToASCII(unsigned char val, char *buf) {
    buf[3] = '\0';
    buf[2] = (val % 10) + '0'; val /= 10;
    buf[1] = (val % 10) + '0'; val /= 10;
    buf[0] = (val % 10) + '0';
}

void main(void) {
    unsigned char adc_val;
    char ascii_buf[4];

    ADC_DATA = 0xFF;   // Port 1 as input
    P2 = 0x00;         // (Optional) keep LEDs off or still display value

    UART_Init();       // Initialize UART for serial output

    while (1) {
        adc_val = read_adc();
        P2 = adc_val;                  // (Optional) keep LEDs updated
        ByteToASCII(adc_val, ascii_buf);
        UART_SendString(ascii_buf);    // Send “000” to “255”
        UART_SendByte('\r');           // Carriage return
        UART_SendByte('\n');           // New line
        // Small delay between samples, if desired
        delay_us();
    }
}