#include "mbed.h"

Serial async_port(p9, p10);     // setup tx and rx on pins 9 and 10
DigitalOut red_led(p25);        // red led
DigitalOut green_led(p26);      // green led
DigitalOut myled1(LED1);        // Onboard LED1
DigitalOut myled2(LED2);        // Onboard LED2

DigitalOut strobe(p7);          // a strobe to trigger scope
DigitalIn switch_ip1(p5);
DigitalIn switch_ip2(p6);
char switch_word;               // the word we will send
char recd_val;                  // the received value

//#define DEBUG2

Serial pc(USBTX, USBRX); // tx, rx

int main() {  
    
    async_port.baud( 9600 );      // set baud rate to 9600 default, accept default format, 8-bits, no parity

    #ifdef DEBUG
        switch_word = 0x00;
    #endif

    while( 1 ) {

        #ifdef DEBUG
            // Cycling the switch_word for testing.
            switch_word = (switch_word + 1) % 4;
        #else  
    
            //Set up the word to be sent, by testing switch input
            switch_word = 0xa0;
            if( switch_ip1 == 1 )
                switch_word = switch_word | 0x01;   // OR in lsb
            if( switch_ip2 == 1 )
                switch_word = switch_word | 0x02;   // OR in next lsb                
        #endif
        
        strobe = 1;                             // short strobe pulse
        wait_ms( 500 );
        strobe = 0;
        async_port.putc( switch_word );         // transmit switch_word
        if( async_port.readable() == 1 )        // is there a character to be read?
            recd_val = async_port.getc();       // if yes, then read it

        #ifdef DEBUG
            recd_val = switch_word;
        #endif

        // set leds according to incoming word from slave
        
        myled1 = 0;                     // preset both to 0
        myled2 = 0;
        pc.printf("========================\r\n");     // print to console

        recd_val &= 0x03;     // AND out unwanted bits
        
        pc.printf("%d\r\n", recd_val);
        
        if( recd_val == 1 ) {
            myled1 = 1;
            pc.printf("LED1 ON, recd_val = 1\r\n");     // print to console
        }
        if( recd_val == 2 ) {
            myled2 = 1;
            pc.printf("LED2 ON, recd_val = 2\r\n");     // print to console
        }
        if( recd_val == 3 ) {
            myled1 = 1;
            myled2 = 1;
            pc.printf("LED1 and LED2 ON, recd_val = 3\r\n\r\n");     // print to console
        }
    }   // while 
}   // main
