/* Author: Scott Watson
 * Date: February, 2008
 * last modified(If I remembered to update this): April 16, 2008
 * 
 * learning how to use output compare to control a servo
 * 
 * code written for a dsPIC30F4012 running with a 7.5MHz crystal*/
 
#include "p30fxxxx.h"
#define byte unsigned char
#define _ISR __attribute__((interrupt))
#define _ISRFAST __attribute__((interrupt, shadow))
#define UART_ENABLED 1  //set to 0 to turn off uart

//Configuration settings for the dsPIC30F4012
//_FOSC(CSW_FSCM_OFF & XT_PLL8); //to get 30MIPS with 15MHz clock (causes problems on startup! outside or rated operating frequency)
_FOSC(CSW_FSCM_OFF & XT_PLL16); //to get 30MIPS with 7.5MHz clock
//_FOSCSEL(PRIOSC_PLL);
_FWDT(WDT_OFF);	
_FGS(CODE_PROT_OFF);
_FBORPOR(PBOR_OFF & PWRT_64);

//Author: Scott Watson

void init_IO(void);
void init_OC2(void);
void init_Uart(void);
void init_Timer2and3(void);
void sendByte(byte i);
void sendString( char * s);
void sendNum(unsigned int i);
void delay(int microseconds);
void startup_flashes_text(void);
void _ISRFAST _T3Interrupt(void);

volatile int dutyCycle = 710;

int main(void){
	//int x=0;
	
	init_IO();
	init_Uart();
	startup_flashes_text();
	
	init_Timer2and3();
	init_OC2();
	
	
	while(1){
		PORTBbits.RB3=1;
		if(PORTBbits.RB0==1 && dutyCycle<1065){
			PORTBbits.RB3=0;
			sendString("increased duty cycle to ");
			dutyCycle+=5;
			OC2RS=dutyCycle;
			sendNum(dutyCycle);
			sendString(" \t(counterclockwise)\n\r");
			delay(10);
		}
		if(PORTBbits.RB1==1 && dutyCycle>110){
			PORTBbits.RB3=0;
			sendString(" decreased duty cycle to ");
			dutyCycle-=5;
			OC2RS=dutyCycle;
			sendNum(dutyCycle);
			sendString("\t(clockwise)\n\r");
			delay(10);
		}
	}
}

void _ISRFAST _T3Interrupt(void){
//	OC2RS=dutyCycle;
	IFS0bits.T3IF=0;
}

void init_OC2(void){
	/*  OUTPUT COMPARE CONFIGURATION CALCULATIONS
	instruction clock cycle time.... 30MIPS... 33ns
	using timer prescaler of 1:64 so 2.112us per TMR3 incriment
	2.112us * 2^16 = 138.4ms maximum period
	1ms = 473.5 timer ticks
	1.5ms = 710.3 timer ticks
	2ms = 947 timer ticks
	25ms = 11,837.1 timer ticks
	*/	
	OC2CONbits.OCM = 0b000; //keep OC2 off while mod's are made
	OC2CONbits.OCSIDL = 0; //0=continued operation in idle mode
	OC2CONbits.OCTSEL = 1; //1=timer3  0=timer2
	OC2R=dutyCycle;
	OC2RS=dutyCycle;
	OC2CONbits.OCM = 0b110; //initialize OC2 low, generate continuous output pulses
}	

void startup_flashes_text(void){
	PORTBbits.RB2=1;
	delay(200);
	PORTBbits.RB2=0;
	delay(200);  
	PORTBbits.RB2=1;
	delay(200);  
	PORTBbits.RB2=0;
	delay(200);  
	PORTBbits.RB2=1;
	
	sendString("\n\rRunning...\n\r\n\r");
	sendString("Starting at duty cycle = ");
	sendNum(dutyCycle);
	sendString("\n\r");
}	

void init_Timer2and3(void){
	//setup TMR2 and TMR3... in this project, TMR3 is used to do continuous PWM on 
	T2CONbits.T32=0; //turn off 32 bit operation
	T2CONbits.TCKPS=0b11;  // 1:256 prescale value
	T2CONbits.TON=1;
	
	T3CONbits.TCKPS=0b10; // 1:64 prescale value
	T3CONbits.TSIDL=0; //continue in idle mode
	T3CONbits.TCS=0; //internal clock  (F_OSC/4)
	T3CONbits.TON=1;
	PR3=11845; //25ms period for 40Hz operation
	IEC0bits.T3IE=1;  //enable interupts for Timer3
	IPC1bits.T3IP=6;  //interupt priority0-7 where 7 is the highest priority
}

void delay(int mseconds){ //cannot be called with more than about 550ms
	const int onemillisecond = 0b000000001110101;
	//const int onemillisecond = 1;
	mseconds=mseconds*onemillisecond;
	//start config bit selection
	T1CONbits.TON=0;
	TMR1=0;
	T1CONbits.TCS=0; 
	T1CONbits.TCKPS=0b11;
	T1CONbits.TON=1;
	while(TMR1<mseconds){
		Nop();
	}
	T1CONbits.TON=0;
}

//////////////////////////////////////////////
//Use this function to send 1 byte to the UART
void sendByte(byte i){
	if(!UART_ENABLED)return;
	while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
}
//////////////////////////////////////////////
//Use this function to send literal strings in quotes as
//ASCII bytes to the UART
void sendString( char str[]){
	if(!UART_ENABLED)return;
    byte i=0;
    for(i=0; str[i]!=0; i++){
        sendByte(str[i]);
	}
}
//////////////////////////////////////////////
//Use this function to send an unsigned integer to the UART
//as ASCII text
void sendNum(unsigned int i){
	if(!UART_ENABLED)return;
	char tmp[10];
	sprintf(tmp, "%u ", i);
	sendString(tmp);
}

//////////////////////////////////////////////
/* UART BRG "Baud" rate calculation "How To"
Baud Rate is the same as Bits Per Second
	U1BRG = (MIPS / BAUD / 16) - 1
EXAMPLE...
15MHz Oscillator with pll of 8
MIPS = 15MMhz * pll8 / 4 = 30,000,000
30000000/BAUD(230400) = 130.208...
130.208... / 16 - 1 = 7.138... ~~~ 7
****don't forget to invert the signal coming out of the PIC
to create a signal that most computer's serial ports will 
interpret  */
void init_Uart(void){
	if(!UART_ENABLED)return;
    U1MODE = 0x0000;
    // Calculations for differnt Baud rates...
    // 25 for baud of 38400 
    // 7  for baud of 230400 pll8
    // 15 for baud of 115200
    U1BRG = 15;
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}

void init_IO(void){
	//setup master timing inputs and outputs
	ADPCFG=0b1111111111111111;//turn off all analog stuff
	TRISBbits.TRISB0=1; //left button
	TRISBbits.TRISB1=1;	//right button
	TRISBbits.TRISB2=0;	//status LED
	PORTBbits.RB2=0;
	TRISBbits.TRISB3=0; //status LED
	PORTBbits.RB3=0;
}
