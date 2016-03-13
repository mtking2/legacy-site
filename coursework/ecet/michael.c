/* This code is a diagnostic and SumoBot instructon set for the WCU NT development board 
 * using the Microchip PIC24HJ64GP502 16 bit microcontroller. It is tested using
 * the Microchip C30 version3.3 compiler, MPLAB IDE X version 1.6.
 *   
 * Peter Tay, Ph.D. March 2013
 * Michael King, Junior ECET, May 2014
 */

#include <p24hj64gp502.h>  // If another supported microcontroller is used, change this header file.
#define FCY 4000000UL // Use internal FRC with OSCTUN=0x0011
#include <libpic30.h>  // This is needed for __delay_ms().
#include <stdio.h>
#include <math.h>

// config
_FBS (BWRP_WRPROTECT_OFF & BSS_NO_FLASH);
_FGS (GSS_OFF & GCP_OFF & GWRP_OFF);
_FOSCSEL (FNOSC_FRC & IESO_OFF);
_FOSC (FCKSM_CSDCMD & IOL1WAY_OFF & OSCIOFNC_ON & POSCMD_NONE);
_FWDT(FWDTEN_OFF);
_FPOR(FPWRT_PWR1 & ALTI2C_OFF);

// UART initialization function
//parameter BAUDRATEREG1 determines baud speed
void UART1Init(int);
//UART transmit function, parameter Ch is the character to send
void UART1PutChar(char);
//UART receive function, returns the value received.
char UART1GetChar();

void forward() {
    // Using bit-masking
    PORTB&=0x0fff;
    __delay_us(20);
    PORTB|=0x5000;
    __delay_us(20);
}

void reverse() {
    // Using bit-masking
    PORTB&=0x0fff;
    __delay_us(10);
    PORTB|=0xa000;
    __delay_us(10);
}

void left() {
    // Using bit-masking
    PORTB&=0x0fff;
    __delay_us(20);
    PORTB|=0x9000;
    __delay_us(20);
}

void soft_left() {
    PORTBbits.RB15=0;
    __delay_us(10);
    PORTBbits.RB14=0;
    __delay_us(10);
    PORTBbits.RB13=0;
    __delay_us(10);
    PORTBbits.RB12=1;
    __delay_us(10);
}

void right() {
    // Using bit-masking
    PORTB&=0x0fff;
    __delay_us(10);
    PORTB|=0x6000;
    __delay_us(10);

}

void stop() {
    PORTB&=0x0fff;
}

void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt( void ) {
    /* Interrupt Service Routine code goes here */
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

void lineDetector() {
    OSCTUN=0x0011;
    ODCBbits.ODCB9=1;
    RPOR4bits.RP9R=0b10010;  // Map OC1 to RP2
    // Initialize Output Compare Module
    OC1CONbits.OCM = 0b000; // Disable Output Compare Module
    OC1RS = 55; // Write the duty cycle for the PWM pulse on RP2
    // The output duty cycle is OC1RS/PR2.
    OC1CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC1CONbits.OCM = 0b110; // Select the Output Compare mode
    // Initialize and enable Timer2
    T2CONbits.TON = 0; // Disable Timer
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TGATE = 0; // Disable Gated Timer mode
    T2CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    TMR2 = 0x0000; // Clear timer register
    PR2 = 110; // Load the period value
    IPC1bits.T2IP = 0x01; // Set Timer 2 Interrupt Priority Level
    IFS0bits.T2IF = 0; // Clear Timer 2 Interrupt Flag
    IEC0bits.T2IE = 1; // Enable Timer 2 interrupt
    T2CONbits.TON = 1; // Start Timer
    _T2Interrupt();
}

float rangeFinder() {
    // RA0(AN0) - RF trigger
    // RA1(AN1) - RF output

    // speed of sound is approximately 1116 ft per second
    float velocity=0.1275;
    float timer=0;
    float distance=0;
    TRISBbits.TRISB11=0;
    PORTBbits.RB11=0;
    __delay_us(50);
    PORTBbits.RB11=1;
    __delay_us(20);
    PORTBbits.RB11=0;
    TRISBbits.TRISB11=1;

    while(!PORTBbits.RB11) {
    }

    while(PORTBbits.RB11) {
        timer+=0.5;
        __delay_us(100);
    }
    if (timer>90) {
        timer=90;
    } else {
        timer++;
    }
    distance=velocity*timer*12;
    __delay_ms(15);
    return distance;
}

int main() {
    
    
    float dist;
    //OSCTUN=0x0011;
    AD1PCFGL=0xffff;
    TRISB=0x008f;
    PORTB=0x0000;
    RPINR18bits.U1RXR	 =5;	 //UART1 receive set to RB5 to tx on breakout board.
    RPOR3bits.RP6R	 =3;	 //UART1 transmit set to RB6 to rx on breakout board.
    //unsigned char data=0,nl=10, cr=13;
    //char sbuf[30];
    UART1Init(12); //Initiate UART1 to 19200 at 8MHz FRC
    lineDetector();
    __delay_ms(5000);

    while (1) {
        
        dist=rangeFinder();

        
        if (dist>0&&dist<=35   && PORTBbits.RB7) {
            stop();
            __delay_us(50);
            
            forward();
            
        } else if (!PORTBbits.RB7) {
            stop();
            __delay_us(50);
            
            reverse();
            __delay_ms(500);

            stop();
            __delay_us(50);
            
            right();
            __delay_ms(500);

        } else {
            stop();
            __delay_us(50);
            
            left();
        }

        // Tera Term display
        /*
        sprintf(sbuf,"%3.2f inches",dist);

        for (i=0;i<13;i++) {
            UART1PutChar(sbuf[i]);
        }
	    UART1PutChar(nl);
        UART1PutChar(cr);
        */
    }
    return 0;
}

// UART initialization function
//parameter BAUDRATEREG1 determines baud speed
void UART1Init(int BAUDRATEREG1) {
    //Set up registers
    U1BRG = BAUDRATEREG1;	//set baud speed
    U1MODE=0x8000;	 //turn on module
    U1STA=0x8400;	 //set interrupts
    //reset RX interrupt flag
    IFS0bits.U1RXIF = 0;
}

//UART transmit function, parameter Ch is the character to send
void UART1PutChar(char Ch) {
    //transmit ONLY if TX buffer is empty
    while(U1STAbits.UTXBF == 1);
    U1TXREG = Ch;
}

char UART1GetChar() {
    char Temp;
    //wait for buffer to fill up, wait for interrupt
    while(IFS0bits.U1RXIF == 0);
    Temp = U1RXREG;
    //reset interrupt
    IFS0bits.U1RXIF = 0;
    //return my received byte
    return Temp;
}

