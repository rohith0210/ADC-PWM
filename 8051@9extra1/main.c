#include <reg51.h>
#include <stdio.h>

#define ADC_DATA P1
sbit ADC_RD = P3^7;
sbit ADC_WR = P3^6;
sbit ADC_INTR = P3^2;

void delay_us(unsigned int us) {
    while(us--);
}

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 1275; j++);
}

void uart_init() {
    SCON = 0x50;  // 8-bit UART, REN enabled
    TMOD = 0x20;  // Timer1 mode2
    TH1 = 0xFD;   // 9600 baud
    TR1 = 1;
}

void uart_tx(char c) {
    SBUF = c;
    while(TI == 0);
    TI = 0;
}

void uart_tx_str(char *str) {
    while(*str) uart_tx(*str++);
}

unsigned char read_adc() {
    ADC_WR = 0;
    delay_us(1);
    ADC_WR = 1;

    while(ADC_INTR == 1); // wait for conversion

    ADC_RD = 0;
    delay_us(1);
    {
        unsigned char value = ADC_DATA;
        RD = 1;
        return value;
    }
}

void main() {
    unsigned char adc_val;
    float voltage, temperature;
    char str[16];

    uart_init();

    while(1) {
        adc_val = read_adc();
        voltage = (adc_val * 5.0) / 255.0;        // Vref = 5.0V
        temperature = voltage * 100.0;         

        sprintf(str, "Temp = %.2f C\r\n", temperature);
        uart_tx_str(str);

        delay_ms(1000);  // Read every 1 second
    }
}
