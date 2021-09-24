/* created by Rumeysa Karakavak
  helping from course book and helping from this links
  
  https://github.com/billhhh/hcs12_mp3_player
  http://egitimevreni.com/2-sinif/flutle-calinabilecek-sarki-notalari.html
  http://embedded-lab.com/blog/lab-19-play-musical-notes/
  http://www.phy.mtu.edu/~suits/notefreqs.html
  
  *****Serial Tera Term *******
*/ 

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#define NOTE_C4  262 //do
#define NOTE_D4  294 //re
#define NOTE_E4  330 //mi
#define NOTE_F4  349 //fa
#define NOTE_G4  392 //sol
#define NOTE_A4  440 //la
#define NOTE_B4  494 //si



/******frequency and tempo arrays of songs******/
 
unsigned int kirmiziBalik[] = {
NOTE_F4,NOTE_F4,NOTE_G4, NOTE_F4,NOTE_G4,
NOTE_A4,NOTE_G4,  
NOTE_F4,NOTE_F4,NOTE_G4,NOTE_F4,NOTE_F4,NOTE_G4, 
NOTE_A4,NOTE_A4, NOTE_G4,
NOTE_F4,NOTE_F4,NOTE_G4,NOTE_F4,NOTE_G4,
NOTE_A4,NOTE_G4,
NOTE_F4,NOTE_F4,NOTE_G4,NOTE_F4};

unsigned int kirmiziBalikTempo[] = {
 10,10,10,10,10,
 10,10,10,10,10,
 10,10,10,10,10,
 10,10,10,10,10,
 10,10,10,10,10,
 10,10};


unsigned int ozgunBeste[] ={ NOTE_G4,NOTE_G4,NOTE_G4,NOTE_C4,NOTE_B4,NOTE_A4,NOTE_C4,NOTE_C4,
                            NOTE_B4,NOTE_B4,NOTE_B4,NOTE_A4,NOTE_B4,NOTE_A4,NOTE_B4,NOTE_A4,NOTE_G4,
                            NOTE_A4,NOTE_A4,NOTE_C4,NOTE_B4,NOTE_A4,NOTE_G4,NOTE_F4,NOTE_E4,NOTE_E4,
                            NOTE_E4,NOTE_E4};
                            
unsigned int ozgunBesteTempo[] ={10,10,10,10,10,10,10,10,
                           10,10,10,10,10,10,10,10,
                           10,10,10,10,10,10,10,10};
                        
unsigned int sutIctim[] = {NOTE_D4,NOTE_E4,NOTE_F4, NOTE_G4, NOTE_F4,NOTE_G4,NOTE_G4,NOTE_G4, NOTE_F4,NOTE_G4,NOTE_G4, 
                      NOTE_G4, NOTE_F4,0, NOTE_E4, NOTE_D4, NOTE_E4,NOTE_F4,NOTE_G4, NOTE_E4,NOTE_F4,NOTE_D4, 
                      NOTE_E4, NOTE_C4, NOTE_C4};  
                      
unsigned int sutIctimTempo[] ={10, 10, 10, 10, 10, 10, 10, 10,10, 10, 7, 7,7, 0,10, 10, 10,10, 10,
                        10, 10,10, 10, 10, 10};
                        
                        
/****** lengths of songs  ******/

unsigned int length1 = 0;
unsigned int length2 = 0;
unsigned int length3 = 0;


/******  functions definitions ******/ 
void singASong(unsigned int frequencyArray[], unsigned int tempArray[],unsigned int size);
void DelayFivesec();
void DelayTensec();
void DelayMsecond();
void MSDelay(unsigned int itime);

char receive();
   
void main(void) {
   unsigned char  str[] ="Get Dip switchs";
   unsigned int i=0;
   unsigned int  number1=0;
   unsigned int  number2=0; 
   unsigned char recieve1;
   unsigned char recieve2;
   
   // lengths of melodies.
   length1 = sizeof(kirmiziBalik)/sizeof(int);
   length2 = sizeof(ozgunBeste)/sizeof(int);
   length3 = sizeof(sutIctim)/sizeof(int);

   SCI0BDL=26;
   SCI0BDH=0x0;
   SCI0CR1=0x0;
   SCI0CR2=0x0C;
   DDRB=0xFF;
   DDRP=0xFF;
   DDRH=0x00;
   DDRT = 0x20; // for 5.bit output. 
   DDRB = DDRB | 0xFF;  // PTB as output
 
 for(;;)  {
  
    for(i = 0; i < 15; ++i){
        while(!(SCI0SR1 & SCI0SR1_TDRE_MASK));
        SCI0DRL=str[i];  
    }

    recieve1=receive();
    recieve2=receive();
         
    number1 = recieve1-'0';
    number2 = recieve2-'0';
    
while(TRUE){
  
     PTH &= 0XFC;
     PTP=0xF7;
         
    if(number1 == 0 &  number2 ==0){    //00
       PORTB = 0x3F;
       // first song
       singASong(kirmiziBalik, kirmiziBalikTempo,length1);
       // second song
       singASong(ozgunBeste, ozgunBesteTempo, length2);
       // third song
       singASong(sutIctim, sutIctimTempo, length3);
    
    } else if(number1 == 0 &  number2 ==1){  //01
        PORTB=0x06;
        for(i = 0; i < 3; i++){
        singASong(kirmiziBalik, kirmiziBalikTempo, length1); 
      }
      
    }else if(number1 ==1 &  number2 ==0){  //10
       PORTB= 0x5B; 
       for(i = 0; i < 3; i++){
        singASong(ozgunBeste, ozgunBesteTempo, length2); 
      }
      
    }else if(number1 ==1 &  number2 ==1){  //11
        PORTB= 0x4F;
        for(i = 0; i < 3; i++){
        singASong(sutIctim, sutIctimTempo, length3); 
       }
    }
   
 }
  } EnableInterrupts;


  for(;;) {
    _FEED_COP();
  }
}
   
char receive(){
    unsigned char data;
  while(!(SCI0SR1 & SCI0SR1_RDRF_MASK));
       data=SCI0DRL;
    return data;  
}


// play song function.
// gets frequency and tempo array and size. plays the songs.
void singASong(unsigned int frequencyArray[], unsigned int tempArray[], unsigned int size){

  long i = 0, j = 0;
  long numberOfCycles = 0;
  int duration;
  int delay = 0;
  unsigned int pause = 0;
  
  for(i = 0; i < size ; i++){
       duration =200 /tempArray[i];    // sets speed of song /200 or else 
                                           // increasing or decreasing amplitude 
      if(frequencyArray[i] == 0) {
          //pause  
          delay = 0;
          numberOfCycles = duration * 10000; 
      } 
      else {
          // calculate the delay value 1 second's worth of 1000000 microseconds, divided by the frequency
          delay = 1000000 / frequencyArray[i] / 2;
          // calculate the number of cycles for proper timing multiply frequency
          numberOfCycles = frequencyArray[i] * duration/ 100; 
       }
       
       TSCR1=0x80; //timer enable -> for tcnt last bit 1
       TSCR2=0x00; //pre-scaler timer normal 24mhz
       TIOS=TIOS_IOS5_MASK; //0x20 -> PT5 output (DDRT)
        
       TCTL1=0x04; // toggle (rising- falling edge)
      
       for (j = 0; j < numberOfCycles * 3; j++) { // note duration 
      
          TC5 = TC5 + delay * 3; // control for TCNT.flag -> 1 
                                    //toggle (better sound *4)(1000 cycleda degiþiyo ama anlamýyoruz)
          while(!(TFLG1 & 0x20)); // 5. bit toggle, cahange the edge 
          TFLG1 |= 0x20;
       }
                                                                             
       pause = duration*17/10;
       MSDelay(pause); 
   }
       
}

void DelayMsecond(){
        
    unsigned int i;
    for(i = 0; i < 366;++i){
      
         while(!(TFLG2 & 0x80));
         TFLG2 |=0x80;
    }
}

void DelayTensec(){
  
  unsigned int i;
  for(i=0; i<10; ++i){
      
     DelayMsecond();
  }
  
}

void DelayFivesec(){
  
  unsigned int i;
  for(i=0; i<5; ++i){
      
     DelayMsecond();
  }
  
}
void MSDelay(unsigned int itime){
    unsigned int i;
    unsigned int j;
    
    for(i=0;i<itime;i++)
      for(j=0;j<4;j++);
}