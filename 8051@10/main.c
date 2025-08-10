#include <REGX51.H>
unsigned char duty = 2;
unsigned char count =0;
void timer0_init(){
  TMOD = 0x04;
  TH0 = 	6;
	TL0 = 6;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
	
}
void timer0_ISR(void) interrupt 1{
    count++;
	if( count < duty ){
	  P1_0 = 1;
	 }
	else{
	  P1_0 = 0;
	}
	if (count >= 4){
        count = 0;
	}
}
void main(){
	P1_0 = 0;
 timer0_init();
	while(1){
	
	}
		
}
	