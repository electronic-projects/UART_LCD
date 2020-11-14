#include <mega8.h>
#include <stdio.h>
#include <delay.h>
#include <alcd.h>
//***************************************************************
#asm
   .equ __lcd_port=0x18 ;PORTB
#endasm
//***************************************************************
unsigned char i;
unsigned char data_lcd[32];
unsigned long result;
unsigned char rx_data[7],flag_rx,flag_tx=0,i_tx=0;
unsigned char tx_data[10]={' ',' ',' ',' ',' ','T','E','S','T',0x0D};
//*************************************************************** 
interrupt [USART_RXC] void usart_rx_isr(void)
{
unsigned char data;
static unsigned char flag_valid=0,i_rx=0;
   data=UDR;
   if(data=='#'){flag_valid=1;i_rx=0;}
   if(flag_valid)
   {      
      rx_data[i_rx]=data;  
      if(++i_rx>=7)
      {
       flag_rx=1;
       flag_valid=0;
      }  
   } 
}     
//*************************************************************** 
interrupt [USART_TXC] void usart_tx_isr(void)
{
   if(flag_tx)UDR=tx_data[i_tx];
   if(++i_tx>=10)flag_tx=0;   
}   
//*************************************************************** 
void main(void)
{

PORTB=0x00;
DDRB=0x00;

PORTD=0x00;
DDRD=0x00;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=0x00;
UCSRB=0xD8;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x33;

#asm("sei") 

lcd_init(16);
//*************************************************************** 
while(1)
{                                
   if(flag_rx){
          flag_rx=0;     
          if(rx_data[0]=='#')
          {
           PORTC.3=!PORTC.3;
           UDR=tx_data[0];i_tx=1;flag_tx=1; 
           flag_rx=0;   
           result = 0; 
           for (i=7;i<0;i--)
           {
              rx_data[i] = rx_data[i]-48;     
              result = (result*10) + rx_data[i];
           } 
           
           sprintf(data_lcd,"NUMBER : %d",result);      
           lcd_gotoxy(0,0);
           lcd_puts(rx_data);         
          }
   }     
}
}
