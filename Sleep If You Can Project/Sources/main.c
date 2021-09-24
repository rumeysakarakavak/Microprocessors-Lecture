/***********************************************************************************************************/
/*                                                                                                         */
/************************           Microprocessor Final Project        ************************************/
/*                                                                                                         */
/***********************************************************************************************************/
/* Created by 141044063 Rumeysa KARAKAVAK                                                                  */
/* This project is like as basic Alarmy (Sleep If U Can)                                                   */
/* Set a method for turn off alarm and sleep                                                               */
/* When alarm turn on, do directions                                                                       */
/* Wake up easily                                                                                          */
/***********************************************************************************************************/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <time.h>
#include <stdlib.h>


#define LCD_DAT PORTK /* Port K drives LCD data pins, E, and RS */
#define LCD_DIR DDRK /* Direction of LCD port */
#define LCD_E 0x02 /* LCD E signal */
#define LCD_RS 0x01 /* LCD Register Select signal */
#define CMD 0 /* Command type for put2lcd */
#define DATA 1 /* Data type for put2lcd */


#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_F3  175
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_F4  349
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_C5  523


/******frequency and tempo arrays of songs******/
unsigned int alarmMusic[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4
};

unsigned int alarmMusicTempo[] = {
  12, 12, 12, 12,
  12, 12,
  12, 12, 12, 12,
  12, 12, 
  12, 12, 12, 12,
  12, 12
};
                        
                        
/****** lengths of song  ******/
unsigned int length1 = 0;

/****** necessary datas for keypad  ******/
const unsigned char keypad[4][4] =
{
'1','2','3','A',
'4','5','6','B',
'7','8','9','C',
'*','0','#','D'
};
unsigned char column,row;

/******  functions definitions ******/ 
void alarmOnForCalculations(void);/*alarm on and ask calculations*/
void alarmOnForQuestions(void);/*alarm on and ask questions*/
void alarmOff(void); /*alarm off*/
int calculations(int random, int difficulty);/*implementation of calculations*/
int questions(int randomNumber, int category);/*implementation of questions*/
void writeLCD2Lines(char *message1, char *message2);/*writes on lcd 2 lines*/
void writeLCDLine(char *message1);/*writes on lcd 1 line*/
void writeSevenSegment(int countOfTrue);/*writes on 7-segment*/
char receive(void); /*receive from serial Tera Term*/
void singASong(unsigned int frequencyArray[], unsigned int tempArray[],unsigned int size);/*taken on my hw2*/

//codes from course book//
void openlcd(void); /* Initialize LCD display */
void put2lcd(char c, char type); /* Write command or data to LCD controller */
void puts2lcd (char *ptr); /* Write a string to the LCD display */
void delay_50us(int n); /* Delay n 50 microsecond intervals */
void delay_1ms(int time) ; /* Delay n 1 millisecond intervals */
int keypadFunction(void);/*gets input from keyboard and convert*/
void mSDelay(unsigned int itime);
void DelayMsecond();
void MSDelay(unsigned int itime);



void main(void) {
  
  char *msg1 ;
  char *msg2 ;
  char choose = 0;
  
  DDRH=0x00;
  DDRB=0XFF;
  DDRT = 0x20; // for 5.bit output. 
  DDRB = DDRB | 0xFF;  // PTB as output
  openlcd();
    
  //serial
  SCI0BDL=26;
  SCI0BDH=0x0;
  SCI0CR1=0x0;
  SCI0CR2=0x0C; 
  
  //write lcd to select the method.
  msg1 = "Alarmi kapatma";
  msg2 = "yontemini secin";
  writeLCD2Lines(msg1, msg2);
  delay_1ms(500); /* wait until "clear display" command complete */
  msg1 = "1-Islem";
  msg2 = "2-Dogru-Yanlis";
  writeLCD2Lines(msg1, msg2);
  delay_1ms(100); /* wait until "clear display" command complete */
    
  choose = receive() - '0'; // do integer
    

  /*get 'choose' on serial tera term.*/
  /*1 for calculations 2 for questions*/
  if(choose == 1){
    
    msg1 = "Switchten level";
    msg2 = "secimi yapin";
    writeLCD2Lines(msg1, msg2);
    delay_1ms(500);
    
    msg1 = "1-Kolay 2-Orta";
    msg2 = "     3-Zor    ";
    writeLCD2Lines(msg1, msg2);
    delay_1ms(1500);

    alarmOnForCalculations();
    PTP = 0x00;
    PORTB =0xFF;
    
  } else if(choose == 2){
    
    msg1 = "Kategori";
    msg2 = "secimi yapin";
    writeLCD2Lines(msg1, msg2);
    delay_1ms(500);
    
    msg1 = "1-Tarih 2-Bilim";
    msg2 = "   3-Genel   ";
    writeLCD2Lines(msg1, msg2);
    delay_1ms(1500);

    alarmOnForQuestions();
    PTP = 0x00;
    PORTB =0xFF;
  }
    

  EnableInterrupts;

  __asm(swi);
  for(;;) {
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}

void alarmOnForCalculations(void){

    int i = 0;
    int level = 0;
    int trueCount = 0;
    int randomNumber = 0;
    int calcCount = 0;

    DDRT = 0x20; // for 5.bit output. 
    DDRH = 0x00;
    PTP=0xF7; // just last 7segment
    level = PTH;
    delay_1ms(1500);
    
    length1 = sizeof(alarmMusic)/sizeof(int); 
    while(trueCount<5){
    
      singASong(alarmMusic, alarmMusicTempo, length1); 
      randomNumber++;
      trueCount += calculations(randomNumber, level);
      writeSevenSegment(trueCount);
      
      if(randomNumber == 10 && trueCount!= 5){
        trueCount = 0;
        randomNumber = 0;
      }
    }
     
    if(trueCount == 5){
      alarmOff();
    }

  return;      
}  


int calculations(int random, int difficulty){
  int answer = 0;
  int result = 0;      
  char *msg1;
  
  if(difficulty == 1){

    switch(random){
  
    case 1: msg1 = "3 * 0 = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;   
            if(answer == 0){
               result = 1;
            }
            break;
            
    case 2: msg1 = " 8 / 4  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 2){
               result = 1;
            }
            break;
    
    case 3: msg1 = " 2 * 4  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 8){
               result = 1;
            }
            break;
             
    case 4: msg1 = " 1 * 3  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 3){
               result = 1;
            }
            break;
             
    case 5: msg1 = " 28 / 4  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 7){
               result = 1;
            }
            break;
             
    case 6: msg1 = " 34 - 25  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 9){
               result = 1;
            }
            break;
             
    case 7: msg1 = " 14 - 8  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 6){
               result = 1;
            }
            break;
         
    case 8: msg1 = " 35 / 5  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 7){
               result = 1;
            }
            break;
             
    case 9: msg1 = " 49 / 9  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 7){
               result = 1;
            }
            break;
             
    case 10:msg1 = " 39 / 13  = ?"; 
            writeLCDLine(msg1);
            answer = keypadFunction()-48; 
            if(answer == 3){
               result = 1;
            }
            break;
    }
  }

  else if(difficulty == 2){

    switch(random){
  
    case 1: msg1 = " 49 / 9 - 2 = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 5){
               result = 1;
            }
            break;
            
    case 2: msg1 = " 2 * 3 - 5 = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;
            if(answer == 1){
               result = 1;
            }
            break;
    
    case 3: msg1 = " 56 / 7 + 1  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48; 
            if(answer == 9){
               result = 1;
            }
            break;
             
    case 4: msg1 = " 30 / 5 + 2  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48; 
            if(answer == 8){
               result = 1;
            }
            break;
             
    case 5: msg1 = " 5 * 3 - 9  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48; 
            if(answer == 6){
               result = 1;
            }
            break;
             
    case 6: msg1 = " 48 / 8 - 6  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48; 
            if(answer == 0){
               result = 1;
            }
            break;
             
    case 7: msg1 = " 21 / 3 - 5  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;  
            if(answer == 2){
               result = 1;
            }
            break;
         
    case 8: msg1 = " 8 * 2 - 9  = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48; 
            if(answer == 7){
               result = 1;
            }
            break;
             
    case 9: msg1 = " 4 - 2 * 0 = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;   
            if(answer == 4){
               result = 1;
            }
            break;
             
    case 10:msg1 = " 12 - 9 * 1 = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48; 
            if(answer == 3){
               result = 1;
            }
            break;
    }
  }

  else if(difficulty == 3){

    switch(random){
  
    case 1: msg1 = "72 /12 - 3 *2 =?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48; 
            if(answer == 0){
               result = 1;
            }
            break;
            
    case 2: msg1 = "45 - 7 * 6 ";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;  
            if(answer == 3){
               result = 1;
            }
            break;
    
    case 3: msg1 = "5 * 8 / 20";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;   
            if(answer == 2){
               result = 1;
            }
            break;
             
    case 4: msg1 = "8 *14 - 12 *9 =?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;   
            if(answer == 4){
               result = 1;
            }
            break;
             
    case 5: msg1 = "87 /3 - 72 /3 =?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;   
            if(answer == 5){
               result = 1;
            }
            break;
             
    case 6: msg1 = "95 /5 - 4 *4 = ?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;  
            if(answer == 3){
               result = 1;
            }
            break;
             
    case 7: msg1 = "9 * 4 / 6=?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;   
            if(answer == 6){
               result = 1;
            }
            break;
         
    case 8: msg1 = "8 *13  - 11 *9=?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;    
            if(answer == 5){
               result = 1;
            }
            break;
             
    case 9: msg1 = "84 /21 + 3 * 1=?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;    
            if(answer == 7){
               result = 1;
            }
            break;
             
    case 10:msg1 = "56 *7 - 49 /7 =?";  
            writeLCDLine(msg1);
            answer = keypadFunction()-48;    
            if(answer == 1){
               result = 1;
            }
            break;
    }
  } 
  return result;
}

void alarmOnForQuestions(void){

  int i = 0;
  int trueCount = 0;
  int randomNumber = 0;
  int categorySelect = 0;
  char *msg1;
  char *msg2;
  
  DDRT = 0x20; // for 5.bit output. 
  DDRH = 0x00;
  PTP = 0xF7; // just last 7segment
  categorySelect = PTH;

  msg1 = "klavyeden";
  msg2 = "d-dogru y-yanlis";
  writeLCD2Lines(msg1, msg2);
  delay_1ms(1500);

  length1 = sizeof(alarmMusic)/sizeof(int); 
  while(trueCount<5){

    singASong(alarmMusic, alarmMusicTempo, length1); 
    randomNumber++;
    trueCount += questions(randomNumber,categorySelect);
    writeSevenSegment(trueCount);
     
    if(randomNumber == 10 && trueCount!= 5){
     trueCount = 0;
     randomNumber = 0;
    }
  }
       
  if(trueCount == 5){
    alarmOff();
  }

}


int questions(int randomNumber, int category){

  int trueSensor = 1;
  int falseSensor = 0;
  int result = 0;      
  int answer = -1;
  char *msg1 ;
  char *msg2 ;
  
  //serial
  SCI0BDL=26;
  SCI0BDH=0x0;
  SCI0CR1=0x0;
  SCI0CR2=0x0C;

  //ask general culture questions.

  //historical
  if(category == 1){

    switch(randomNumber){
        case 1: msg1 = "Dunyada en kisa";
                msg2 = "savas 40 dkdir";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                
        case 2: msg1 = "2.AbdulHamid";
                msg2 = "Sherlock fanidir";     //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
        
        case 3: msg1 = "Kibrit cakmaktan";
                msg2 = "once bulunmustur";   //false 
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
                 
        case 4: msg1 = "Escobarkizi icin";
                msg2 = "2 milyon $ yakti";   //true
                writeLCD2Lines(msg1, msg2);     
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 5: msg1 = "Aztek Oxforddan";
                msg2 = "daha eskidir";   //false    
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
                 
        case 6: msg1 = "Sigara eskiden";
                msg2 = "sagliklidenmisti";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 7: msg1 = "Osmanli 3 yuzyil";
                msg2 = "hukum surmustur";  //false 6
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
             
        case 8: msg1 = "Ay takvimini";
                msg2 = "Asurlar bulmustu";   //false sumer
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
                 
        case 9: msg1 = "Nintendo Osmanli";
                msg2 = "zamaninda cikmis";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 10:msg1 = "Hitlerin yegeni";
                msg2 = "ABDye savasmisti";   //true
                writeLCD2Lines(msg1, msg2);
               delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
    }            
  }

  //scientific
  else if(category == 2){

    switch(randomNumber){
        case 1: msg1 = "Insan ve muzun";
                msg2 = "dnasi %50 benzer";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                
        case 2: msg1 = "Soguk su sicak";
                msg2 = "sudan agirdir.";   //false
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
        
        case 3: msg1 = "Bir sac teli";
                msg2 = "3kg tasiyabilir";   //true 
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 4: msg1 = "kan tum vucudu";
                msg2 = "23saniyede gezer";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 5: msg1 = "Ayni yere 2 defa";
                msg2 = "yildirim dusmez";   //false  
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
                 
        case 6: msg1 = "Bir insanda 20";
                msg2 = "litre kan vardir";   //false
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
                 
        case 7: msg1 = "Japon baliginin";
                msg2 = "hafizasi 3sndir";   //false
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
             
        case 8: msg1 = "Karincalar";  //true
                msg2 = "uyumaz.";
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 9: msg1 = "DeveKusununBeyni";
                msg2 = "gozundenKucuktur";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 10:msg1 = "IQ ruya ile";
                msg2 = "dogru orantilidi";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
    }            
  }
  //general
  else if(category == 3){
      switch(randomNumber){
      
        case 1: msg1 = "Guney Afrikada";
                msg2 = "3 baskent vardir";      //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                
        case 2: msg1 = "1in turevi 1dir"; //false
                writeLCDLine(msg1);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
        
        case 3: msg1 = "Cicekler muzikle";
                msg2 = "daha hizli buyur";  //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 4: msg1 = "Dunyanin 2/3si";  //true
                msg2 = "kar gormemistir";
                writeLCD2Lines(msg1, msg2);     
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 5: msg1 = "Roma rakaminda 0";
                msg2 = "E ile gosterilir";     //false
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
                 
        case 6: msg1 = "Bir yilan 10 gun";
                msg2 = "uyuyabilir";      //false 3
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='y'){
                  result = 1;
                }
                break;
                 
        case 7: msg1 = "Aspirinin ham";
                msg2 = "maddesi soguttur";     //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
             
        case 8: msg1 = "Vatikan dunyada";
                msg2 = "en kucuk ulkedir";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 9: msg1 = "Salatalik sebze";
                msg2 = "degil meyvedir";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
                 
        case 10:msg1 = "Inci ";
                msg2 = "sirkede erir";   //true
                writeLCD2Lines(msg1, msg2);
                delay_1ms(500) ; 
                answer = receive();
                if(answer =='d'){
                  result = 1;
                }
                break;
    }
  }
  return result;
}

void writeLCD2Lines(char *message1, char *message2){
 
  put2lcd(0x01,CMD);
  delay_1ms(100); 
  puts2lcd(message1);
  put2lcd(0xC0,CMD); 
  delay_1ms(1);
  puts2lcd(message2); 
}

void writeLCDLine(char *message1){
        
  put2lcd(0x01,CMD);
  puts2lcd(message1);
}

char receive(void){
  unsigned char data;
  while(!(SCI0SR1 & SCI0SR1_RDRF_MASK));
        data=SCI0DRL;
  return data;  
}

void alarmOff(void){
  char *msg1;
  TIOS = 0x00;
  msg1 = "    Gunaydin    ";
  writeLCDLine(msg1);
  delay_1ms(500);
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
                                    //toggle (better sound *8)(1000 cycleda degi?iyo ama anlamyyoruz)
          while(!(TFLG1 & 0x20)); // 5. bit toggle, change the edge 
          TFLG1 |= 0x20;
       }
                                                                             
       pause = duration*17/10;
       MSDelay(pause); 
   }       
}
void writeSevenSegment(int countOfTrue){

  PTP = 0x07;

  if(countOfTrue == 0) {     
      PORTB = 0x3F;        
      delay_1ms(10);
    
  }else if (countOfTrue == 1){
    PORTB =0x06 ;        
    delay_1ms(10);
  
  }else if(countOfTrue == 2){
    PORTB = 0x5B ;        
    delay_1ms(10); 
  
  }else if(countOfTrue == 3){     
    PORTB = 0x4F ;        
    delay_1ms(10);
  
  }else if(countOfTrue == 4){ 
    PORTB = 0x66;       
    delay_1ms(10);
  
  }else if(countOfTrue == 5) {
    PORTB = 0x6D;        
    delay_1ms(10);
  
  }else if(countOfTrue == 6 ) {
    PORTB =0x7D;           
    delay_1ms(10);
  
  }else if(countOfTrue == 7){
    PORTB = 0x07 ;     
    delay_1ms(10);
  
  }else if(countOfTrue == 8) {
    PORTB =0x7F ;     
    delay_1ms(10);

  }else if(countOfTrue == 9){
    PORTB = 0x6F ;            
    delay_1ms(10);
  }
}

/******************************** CODES ON COURSE BOOK ****************************************/
void openlcd(void){

  LCD_DIR = 0xFF; /* configure LCD_DAT port for output */
  delay_1ms(100); /* Wait for LCD to be ready */
  put2lcd(0x28, CMD); /* set 4-bit data, 2-line display, 5x7 font */
  put2lcd(0x0F, CMD); /* turn on display, cursor, blinking */
  put2lcd(0x06, CMD); /* move cursor right */
  put2lcd(0x01, CMD); /* clear screen, move cursor to home */
  delay_1ms(20); /* wait until "clear display" command complete */
}
void puts2lcd (char *ptr){
  while (*ptr) { /* While character to send */
  put2lcd(*ptr, DATA); /* Write data to LCD */
  delay_50us(1); /* Wait for data to be written */
  ptr++; /* Go to next character */}
}
void put2lcd(char c, char type) {
  
  char c_lo, c_hi;
    
  c_hi = (c & 0xF0) >> 2; /* Upper 4 bits of c */
  c_lo = (c & 0x0F) << 2; /* Lower 4 bits of c */
    
  LCD_DAT &= (~LCD_RS); /* select LCD command register */
  if (type == DATA)
    LCD_DAT = c_hi | LCD_RS; /* output upper 4 bits, E, RS high */
  else 
    LCD_DAT = c_hi; /* output upper 4 bits, E, RS low */
      
  LCD_DAT |= LCD_E; /* pull E signal to high */
  __asm(nop); /* Lengthen E */
  __asm(nop);
  __asm(nop);
  LCD_DAT &= (~LCD_E); /* pull E to low */
    
  /*lsb nibble */
    
  LCD_DAT &= (~LCD_RS); /* select LCD command register */
  if (type == DATA)
    LCD_DAT = c_lo | LCD_RS; /* output lower 4 bits, E, RS high */
  else 
    LCD_DAT = c_lo; /* output lower 4 bits, E, RS low */
  LCD_DAT |= LCD_E; /* pull E to high */
  __asm(nop); /* Lengthen E */
  __asm(nop);
  __asm(nop);
  LCD_DAT &= (~LCD_E); /*pull E to low */
    
  delay_1ms(1); /* Wait for command to execute */     
}

int keypadFunction(void){

  DDRJ |=0x02; 
  PTJ &=~0x02;                            //ACTIVATE LED ARRAY ON PORT B
  DDRP |=0x0F;                           //
  PTP |=0x0F;                            //TURN OFF 7SEG LED
  DDRA = 0x0F;                           //MAKE ROWS INPUT AND COLUMNS OUTPUT
   
    while(1){                              //OPEN WHILE(1)
      do{                                 //OPEN do1
         PORTA = PORTA | 0x0F;            //COLUMNS SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
      }while(row == 0x00);                //WAIT UNTIL KEY PRESSED //CLOSE do1

      do{                                 //OPEN do2
         do{                              //OPEN do3
            mSDelay(1);                   //WAIT
            row = PORTA & 0xF0;           //READ ROWS
         }while(row == 0x00);             //CHECK FOR KEY PRESS //CLOSE do3
         
         mSDelay(15);                     //WAIT FOR DEBOUNCE
         row = PORTA & 0xF0;
      }while(row == 0x00);                //FALSE KEY PRESS //CLOSE do2

      while(1){                           //OPEN while(1)
         PORTA &= 0xF0;                   //CLEAR COLUMN
         PORTA |= 0x01;                   //COLUMN 0 SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
         if(row != 0x00){                 //KEY IS IN COLUMN 0
            column = 0;
            break;                        //BREAK OUT OF while(1)
         }
         PORTA &= 0xF0;                   //CLEAR COLUMN
         PORTA |= 0x02;                   //COLUMN 1 SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
         if(row != 0x00){                 //KEY IS IN COLUMN 1
            column = 1;
            break;                        //BREAK OUT OF while(1)
         }

         PORTA &= 0xF0;                   //CLEAR COLUMN
         PORTA |= 0x04;                   //COLUMN 2 SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
         if(row != 0x00){                 //KEY IS IN COLUMN 2
            column = 2;
            break;                        //BREAK OUT OF while(1)
         }
         PORTA &= 0xF0;                   //CLEAR COLUMN
         PORTA |= 0x08;                   //COLUMN 3 SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
         if(row != 0x00){                 //KEY IS IN COLUMN 3
            column = 3;
            break;                        //BREAK OUT OF while(1)
         }
         row = 0;                         //KEY NOT FOUND
      break;                              //step out of while(1) loop to not get stuck
      }                                   //end while(1)

      if(row == 0x10){
        return keypad[0][column];         //OUTPUT TO PORTB LED
 
      }
      else if(row == 0x20){
         return keypad[1][column];
 
      }
      else if(row == 0x40){
         return keypad[2][column];
 
      }
      else if(row == 0x80){
         return keypad[3][column];
 
      }

      do{
         mSDelay(15);
         PORTA = PORTA | 0x0F;            //COLUMNS SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
      }while(row != 0x00);                //MAKE SURE BUTTON IS NOT STILL HELD
   } 

}void delay_50us(int n) /* Delay n 50 microsecond intervals */
{
  
  int a;
  for(; n>0; n--)
    for(a=133; a>0;a--);  
}

void delay_1ms(int time){
  int i;
  while(time>0){
    for(i = 0; i <2000; ++i);
    --time;
  }
}

void DelayMsecond(){
        
    unsigned int i;
    for(i = 0; i < 366;++i){
      
         while(!(TFLG2 & 0x80));
         TFLG2 |=0x80;
    }
}
void MSDelay(unsigned int itime){
    unsigned int i;
    unsigned int j;
    for(i=0;i<itime;i++)
      for(j=0;j<4;j++);
}

void mSDelay(unsigned int itime){
unsigned int i; unsigned int j;
   for(i=0;i<itime;i++)
      for(j=0;j<4000;j++);
}


