/*
 *For IR protocol :
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 
 * For the Menu : 
 *THIS IS THE NEW VERSION THAT WORKS WITH THE NEW LIBRARIES!!!***
 *TFTLCD.h and TouchScreen.h are from adafruit.com where you can also purchase a really nice 2.8" TFT with touchscreen :)
 *2012 Jeremy Saglimbeni - thecustomgeek.com
 
 For Entire Code
 * Copyright (c) 2017 Frederique Cavalier for HelloTigerLab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */


#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #include "SoftwareSerial.h"
  SoftwareSerial port(12,13);
#else // Arduino 0022 - use modified NewSoftSerial
  #include "WProgram.h"
  #include "NewSoftSerial.h"
  NewSoftSerial port(12,13);
#endif 


// *** SPFD5408 change -- Begin
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>
// *** SPFD5408 change -- End

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0xFFFF
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0x0000
#define JJCOLOR   0x1CB6

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;



// include the IRremote library --------------------------------
//ledIR sur pin 9 obligatoirement
#include <IRremote.h>
IRsend irsend;

//include EasyVR library -------------------------------------------- 

#include "EasyVR.h"
EasyVR easyvr(port);
bool useCommands = true;
EasyVRBridge bridge;
int8_t group, idx;

//0 = snd beep
//1 = 13 eme rue
//2 = appel fred
//3 = appel jean michel
//4 = appel jg
//5 = appel yann
//6 = arte
//7 = baisser de chaine
//8 = baisser de son
//9 = bonjour
//10 = canal +
//11 = chaines preferees
//12 = france 2
//13 = HD1
//14 = La box
//15 = lumiere 1
//16 = lumiere 2
//17 = lumiere 3
//18 = mise en veille
//19 = monter de chaine
//20 = monter le son
//21 = planete +
//22 = police 
//23 = pompiers
//24 = prise deux
//25 = prise trois 
//26 = prise une
//27 = que veux tu
//28 = radio
//29 = samu
//30 = serie club
//31 = telephone
//32 = television
//33 = TF1
//34 = TMC
//35 = urgences

//BUTTON PIN --------------------------------------------------
int pushButton = 10; //Push button with résistance in pull-down 
int etatpushButton = LOW;


int i = 0;
int page = 0;
int blv;
int sleep = 0;
int pulsev = 0;
int redflag = 0;
int greenflag = 0;
int redled = 2;
int greenled = A4;
int backlight = 3;
int battfill;
unsigned long sleeptime;
unsigned long battcheck = 10000; // the amount of time between voltage check and battery icon refresh
unsigned long prevbatt;
int battv;
int battold;
int battpercent;
int barv;
int prevpage;
int sleepnever;
int esleep;
int backlightbox;
int antpos = 278;
unsigned long awakeend;
unsigned long currenttime;
unsigned long ssitime;
char voltage[10];
char battpercenttxt [10];
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

// VOID PAUSE === mise en veille //

void wait()

{

while(digitalRead(pushButton) == LOW);  //do nothing until button pressed
if (digitalRead(pushButton)== HIGH) etatpushButton=!etatpushButton;
while (digitalRead(pushButton) == LOW);
digitalWrite(pushButton,etatpushButton);

}

  
//VOID SETUP --------------------------------------------------

void setup()
{

//Serial1.begin(250000);
//  Serial.println("HX8357 Test!");
  
//  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
  Serial.begin(9600);
  tft.begin(0x9341); // SDFP5408
  
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  tft.setTextColor(YELLOW);  tft.setTextSize(2); 
  
//pinMode(pushButton, INPUT);
//digitalRead(pushButton);

// bridge mode?
  if (bridge.check())
  {
    cli();
    bridge.loop(0, 1, 12, 13);
  }
  // run normally
  //Serial.begin(9600);
  port.begin(9600);
  pinMode(pushButton, INPUT);// met le bouton en entrée
  digitalRead(pushButton);
  pinMode(4, OUTPUT); // initializes pin 4 for LED as an output
  pinMode(9,OUTPUT); // The library initializes pin 3 as an output
 // digitalWrite(9, LOW);// Since our LED is connected to pin 9, we initialize it here
 // low with pull-down HARDWARE---> LOW for check light // we define button (LOW or HIGH)
 
 
 // if shield sim, code for initialize :

  //SIM900.begin(19200);               
  //SIM900power();  
  //delay(1000);  // give time to log on to network. 
  //if (!easyvr.detect())
  //{
  //Serial.println("EasyVR not detected!");
  //for (;;);
  //}

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  //Serial.println("EasyVR detected!");
  easyvr.setTimeout(10);
  easyvr.setLanguage(5);
  easyvr.setKnob(0);
  easyvr.setLevel(1);
}


void loop(void) {
  
    {
  //tft.fillScreen(BLACK);
    tft.setRotation(1);
 //   SIM900.println("ATH");   // hang up    SIM900.println("ATD");
 //   SIM900.println("RING");
 //   SIM900.println("AT+COLP");
      

    digitalRead(pushButton);
    wait(); 
    drawhomeicon();
    easyvr.playSound(9, EasyVR::VOL_DOUBLE);  // BONJOUR
    //delay (5);
    digitalRead(pushButton);
 //   SIM900.println("ATH");   // hang up
 //   SIM900.println("ATD");
 //   delay (1500);

// FONCTION 1 BIP DEPART //

        if (digitalRead(pushButton) == HIGH) 
        {
        digitalWrite(4, HIGH); //set LED ON
        clearsettings();
        delay(50);
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        homescr();
        delay (100);
        digitalRead(pushButton);
        }
              
        else 
        {
        //SIM900.println("ATH");   // hang up
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        return;
        }  
 
 }
}
void redraw() { // redraw the page
  if ((prevpage != 6) || (page !=7)) {
    clearcenter();
  }
  if (page == 0) {
    homescr();
  }
  if (page == 1) {
    menuTV();
  }
  if (page == 2) {
    menuTEL();
  }
  if (page == 3) {
    menuLUMIERES();
  }
  if (page == 4) {
    menuCHAINES();
  }
  if (page == 5) {
    menu5();
  }
  if (page == 6) {
    settingsscr();
  }
}
void clearcenter() { // the reason for so many small "boxes" is that it's faster than filling the whole thing
  tft.drawRect(0, 20, 150, 50, BLACK);
  tft.drawRect(170, 20, 150, 50, BLACK);
  tft.drawRect(0, 80, 150, 50, BLACK);
  tft.drawRect(170, 80, 150, 50, BLACK);
  tft.drawRect(0, 140, 150, 50, BLACK);
  tft.drawRect(170, 140, 150, 50, BLACK);
  tft.fillRect(22, 37, 106, 16, BLACK);
  tft.fillRect(192, 37, 106, 16, BLACK);
  tft.fillRect(22, 97, 106, 16, BLACK);
  tft.fillRect(192, 97, 106, 16, BLACK);
  tft.fillRect(22, 157, 106, 16, BLACK);
  tft.fillRect(192, 157, 106, 16, BLACK);
}
void clearsettings() { // this is used to erase the extra drawings when exiting the settings page
  tft.fillRect(0, 20, 480, 320, BLACK);//tft.fillRect(0, 20, 320, 110, BLACK);
  delay(500);
  clearmessage();
}

void homescr() {
  digitalWrite(4, LOW); // met la LED en OFF
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();

 
  tft.setCursor(41, 37);
  tft.print("TV"); // menu1
  easyvr.playSound(32, EasyVR::VOL_DOUBLE); // TELEVISION
  digitalRead(pushButton);
  delay (200);
  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(41, 37);
        tft.setTextColor(BLUE); 
        tft.print("TV");
        delay (150);
        clearsettings();
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART  
        menuTV();
        digitalRead(pushButton);
        delay (150); 
        return;        
     }
     
     
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(210, 37);
  tft.print("BOX"); // menu 2
  easyvr.playSound(14, EasyVR::VOL_DOUBLE); // LA BOX
  digitalRead(pushButton);
  delay (200);
  
  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON
        tft.setCursor(210, 37);
        tft.setTextColor(BLUE);      
        tft.print("BOX");
        irsend.sendNEC(0x404050AF,32); // LA BOX 
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART   
        delay (150);   
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;        
     }
  
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(41, 97);
  tft.print("TEL"); // menu 3
  easyvr.playSound(31, EasyVR::VOL_DOUBLE); // TELEPHONER
  digitalRead(pushButton);
  delay (200);
  
  if (digitalRead(pushButton) == HIGH)
      {       
        digitalWrite(4, HIGH); // met la LED en ON
        tft.setCursor(41, 97);
        tft.setTextColor(BLUE);
        tft.print("TEL");
        delay (150);
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        menuTEL();
        return;       
     }
  
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(210, 97);
  tft.print("RADIO"); // menu 4
  easyvr.playSound(28, EasyVR::VOL_DOUBLE); // RADIO
  digitalRead(pushButton);
  delay (200);
  
  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON
        tft.setCursor(210, 97);
        tft.setTextColor(BLUE);
        tft.print("RADIO");
        delay (150);
        digitalWrite(4, LOW); // met la LED en OFF
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;        
     }
  
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(41, 157);
  tft.print("LUMIERES"); // menu 5
  easyvr.playSound(15, EasyVR::VOL_DOUBLE); // LUMIERES
  digitalRead(pushButton);
  delay (200);
  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON
        tft.setCursor(41, 157);
        tft.setTextColor(BLUE);
        tft.print("LUMIERES");
        delay (150);
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        menuLUMIERES();
        digitalRead(pushButton);
        delay (150);
        return;      
     }
  
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(200, 157);
  tft.print("VEILLE");
  easyvr.playSound(18, EasyVR::VOL_DOUBLE); // VEILLE
  digitalRead(pushButton);
  delay (200);
  if (digitalRead(pushButton) == HIGH)
      {
        tft.setCursor(200, 157);
        tft.setTextColor(BLUE);
        tft.print("VEILLE");
        delay (150);
        clearsettings();
        drawhomeicon();
        wait();
        digitalRead(pushButton);
        return;
     }
        
        clearsettings();
        drawhomeicon();
        wait();
        digitalRead(pushButton);
        return;
  
}


void menuTV() {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
  
  tft.setCursor(41, 37);
  tft.print("TV");
  easyvr.playSound(32, EasyVR::VOL_DOUBLE); // TELEVISION
  digitalRead(pushButton);
  delay (300);
  if (digitalRead(pushButton) == HIGH)
      {           
       digitalWrite(4, HIGH); // met la LED en ON
       tft.setCursor(41, 37);
       tft.setTextColor(BLUE); 
       tft.print("TV");
       irsend.sendRC5(0x84C,12); // TV
       digitalWrite(4, LOW); // met la LED en OFF
       easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
       delay (150);
       clearsettings();
       easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
       drawhomeicon();
       return;                   
     }
     
    else (digitalRead(pushButton) == LOW);   
    tft.setCursor(210, 37);
    tft.print("FAVORITES");
    easyvr.playSound(11, EasyVR::VOL_DOUBLE); // CHAINES PREFEREES
    digitalRead(pushButton);
    delay (200);
    if (digitalRead(pushButton) == HIGH)
    {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(210, 37);
        tft.setTextColor(BLUE); 
        tft.print("FAVORITES");
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        delay (150);
        clearsettings();
        menuCHAINES();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return; 
                   
     }
     
  else (digitalRead(pushButton) == LOW);
  tft.setCursor(41, 97);
  tft.print("CHAINE +");
  easyvr.playSound(19, EasyVR::VOL_DOUBLE); // MONTER DE CHAINE
  digitalRead(pushButton);
  delay (200);
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(41, 97);
        tft.setTextColor(BLUE);
        tft.print("CHAINE +");
        irsend.sendNEC(0x4040A857,32); // MONTE DE CHAINE
        delay (150);
        digitalWrite(4, LOW); // met la LED en OFF
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;                   
     }
     
  else (digitalRead(pushButton) == LOW);
  tft.setCursor(210, 97);
  tft.print("CHAINES -");
  easyvr.playSound(7, EasyVR::VOL_DOUBLE); // BAISSER DE CHAINE
  digitalRead(pushButton);
  delay (200);
  
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON     
        tft.setCursor(210, 97);
        tft.setTextColor(BLUE); 
        tft.print("CHAINES -");
        irsend.sendNEC(0x404018E7,32); // BAISSE DE CHAINE
        delay (150);
        digitalWrite(4, LOW); // met la LED en OFF
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;                  
     }
     
  else (digitalRead(pushButton) == LOW);   
  tft.setCursor(41, 157);
  tft.print("SON +");
  easyvr.playSound(20, EasyVR::VOL_DOUBLE); // MONTER LE SON
  digitalRead(pushButton);
  delay (200);
  
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(41, 157);
        tft.setTextColor(BLUE); 
        tft.print("SON +");
        irsend.sendNEC(0x4040827D,32); // MONTE LE SON
        irsend.sendNEC(0x4040827D,32); // MONTE LE SON
        delay (150);
        digitalWrite(4, LOW); // met la LED en OFF
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;         
     }
     
  else (digitalRead(pushButton) == LOW);   
  tft.setCursor(210, 157);
  tft.print("SON -");
  easyvr.playSound(8, EasyVR::VOL_DOUBLE); // BAISSER LE SON
  digitalRead(pushButton);
  delay (200);
  
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(290, 240);
        tft.setTextColor(BLUE); 
        tft.print("SON -");
        irsend.sendNEC(0x404042BD,32); // BAISSER LE SON
        irsend.sendNEC(0x404042BD,32); // BAISSER LE SON
        delay (150);
        digitalWrite(4, LOW); // met la LED en OFF
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;                  
     }
     
  clearsettings();
  drawhomeicon();
  easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
  return;
 
  
}
void menuTEL() {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
  
  tft.setCursor(75, 40);
  tft.print("SAMU"); // menu1
  easyvr.playSound(29, EasyVR::VOL_DOUBLE); // SAMU
  digitalRead(pushButton);
  delay (300);
  
  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(75, 40);
        tft.setTextColor(BLUE); 
        tft.print("SAMU");
        //callSamu();
        delay (100);
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;              
     }     
     
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(310, 40);
  tft.print("POLICE"); // menu 2
  easyvr.playSound(22, EasyVR::VOL_DOUBLE); // POLICE
  digitalRead(pushButton);
  delay (300);

  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON
        tft.setCursor(310, 40);
        tft.setTextColor(BLUE);      
        tft.print("POLICE");
        //callPolice();     
        delay (100);
        digitalWrite(4, LOW); // met la LED en OFF
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;
        
     }
     
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(290, 145);
  tft.print("POMPIERS"); // menu 3
  easyvr.playSound(23, EasyVR::VOL_DOUBLE); // POMPIERS
  digitalRead(pushButton);
  delay (300);
  
  if (digitalRead(pushButton) == HIGH)
      {
        
        digitalWrite(4, HIGH); // met la LED en ON
        tft.setCursor(290, 145);
        tft.setTextColor(BLUE);
        tft.print("POMPIERS");
        //callPompiers();
        delay (100);
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;
        
     }
     
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(75, 145);
  tft.print("FRED"); // 
  easyvr.playSound(2, EasyVR::VOL_DOUBLE); // FRED
  digitalRead(pushButton);
  delay (300);
  
  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON
        tft.setCursor(75, 145);
        tft.setTextColor(BLUE);
        tft.print("FRED");
        //callFred();
        delay (100);
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        return;
     }
     
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(40, 240);
  tft.print("JEAN-GAB"); // menu 5
  easyvr.playSound(4, EasyVR::VOL_DOUBLE); // JEAN-GAB
  digitalRead(pushButton);
  delay (300);
  
  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON
        tft.setCursor(40, 240);
        tft.setTextColor(BLUE);
        tft.print("JEAN-GAB");
        //callJeangab();
        delay (100);
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(24, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;    
     }
  
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(310, 240);
  tft.print("NANOU");
  easyvr.playSound(5, EasyVR::VOL_DOUBLE); // NANOU
  digitalRead(pushButton);
  delay (300);
  
  if (digitalRead(pushButton) == HIGH)
      {
        tft.setCursor(310, 240);
        tft.setTextColor(BLUE);
        tft.print("NANOU");
        //callNanou();
        delay (100);
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;
     }
        
        clearsettings();
        drawhomeicon();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        wait;
        return;
}
void menuLUMIERES() {
  digitalWrite(4, LOW); // met la LED en OFF
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
 
  tft.setCursor(40, 40);
  tft.print("ALLUME"); // menu1
  easyvr.playSound(22, EasyVR::VOL_DOUBLE); // LUMIERE
  digitalRead(pushButton);
  delay (300);
  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(40, 40);
        tft.setTextColor(BLUE); 
        tft.print("ALLUME");
        //led.on();       // set the LED on // Allume la lumiere       
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        delay (100);
        clearsettings();
        easyvr.playSound(24, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;       
     }
     
     
  else (digitalRead(pushButton) == LOW);
  tft.setTextColor(WHITE);
  tft.setCursor(290, 40);
  tft.print("ETEINS"); // menu 2
  easyvr.playSound(22, EasyVR::VOL_DOUBLE); // LUMIERE
  digitalRead(pushButton);
  delay (300);
  
  if (digitalRead(pushButton) == HIGH)
      {
        digitalWrite(4, HIGH); // met la LED en ON
        tft.setCursor(290, 40);
        tft.setTextColor(BLUE);      
        tft.print("ETEINS");
        //led.off();       // set the LED on // Allume la lumiere 
        digitalWrite(4, LOW); // met la LED en OFF
        easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        delay (300);  
        clearsettings();
        easyvr.playSound(24, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;        
     }
  clearsettings();
  drawhomeicon();
  easyvr.playSound(24, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
  wait;
  return;  

}
void menuCHAINES() {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
  
  tft.setCursor(41, 37);
  tft.print("TF1");
  easyvr.playSound(33, EasyVR::VOL_DOUBLE); // CHAINE 1
  digitalRead(pushButton);
  delay (100);
  if (digitalRead(pushButton) == HIGH)
      {           
       digitalWrite(4, HIGH); // met la LED en ON
       tft.setCursor(41, 37);
       tft.setTextColor(BLUE); 
       tft.print("TF1");
       irsend.sendNEC(0x4040807F,32); // CHAINE 1
       digitalWrite(4, LOW); // met la LED en OFF
       //easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
       delay (50);
       clearsettings();
       easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
       drawhomeicon();
       return;                   
     }
     
    else (digitalRead(pushButton) == LOW);   
    tft.setCursor(210, 37);
    tft.print("FRANCE 2");
    easyvr.playSound(12, EasyVR::VOL_DOUBLE); // CHAINE 2
    digitalRead(pushButton);
    delay (300);
    if (digitalRead(pushButton) == HIGH)
    {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(210, 37);
        tft.setTextColor(BLUE); 
        tft.print("FRANCE 2");
        irsend.sendNEC(0x404040BF,32); // CHAINE 2
        digitalWrite(4, LOW); // met la LED en OFF
        //easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        delay (50);
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return; 
                   
     }
     
  else (digitalRead(pushButton) == LOW);
  tft.setCursor(41, 97);
  tft.print("13e RUE");
  easyvr.playSound(1, EasyVR::VOL_DOUBLE); // CHAINE 41
  digitalRead(pushButton);
  delay (100);
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(41, 97);
        tft.setTextColor(BLUE);
        tft.print("13e RUE");
        irsend.sendNEC(0x404020DF,32); // CHAINE 41
        delay (100);
        irsend.sendNEC(0x4040807F,32); // CHAINE 41
        delay (50);
        digitalWrite(4, LOW); // met la LED en OFF
        //easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;                   
     }
     
  else (digitalRead(pushButton) == LOW);
  tft.setCursor(210, 97);
  tft.print("HD1");
  easyvr.playSound(13, EasyVR::VOL_DOUBLE); // CHAINE 66
  digitalRead(pushButton);
  delay (100);
  
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON     
        tft.setCursor(290, 140);
        tft.setTextColor(BLUE); 
        tft.print("HD1");
        irsend.sendNEC(0x4040609F,32); // CHAINE 66
        delay (100);
        irsend.sendNEC(0x4040609F,32); // CHAINE 66
        delay (50);
        digitalWrite(4, LOW); // met la LED en OFF
        //easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;                  
     }
     
  else (digitalRead(pushButton) == LOW);   
  tft.setCursor(41, 157);
  tft.print("CANAL +");
  easyvr.playSound(10, EasyVR::VOL_DOUBLE); // CHAINE 4
  digitalRead(pushButton);
  delay (300);
  
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(41, 157);
        tft.setTextColor(BLUE); 
        tft.print("CANAL +");
        irsend.sendNEC(0x404020DF,32); // CHAINE 4
        delay (100);
        digitalWrite(4, LOW); // met la LED en OFF
        //easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;         
     }
     
  else (digitalRead(pushButton) == LOW);   
  tft.setCursor(210, 157);
  tft.print("ARTE");
  easyvr.playSound(6, EasyVR::VOL_DOUBLE); // ARTE
  digitalRead(pushButton);
  delay (100);
  
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(210, 157);
        tft.setTextColor(BLUE); 
        tft.print("ARTE");
        irsend.sendNEC(0x4040E01F,32); // ARTE
        delay (50);
        digitalWrite(4, LOW); // met la LED en OFF
        //easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;                  
     }
     
     
  else (digitalRead(pushButton) == LOW);   
  tft.setCursor(41, 37);
  tft.print("TMC");
  easyvr.playSound(34, EasyVR::VOL_DOUBLE); // TMC
  digitalRead(pushButton);
  delay (100);
  
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(41, 37);
        tft.setTextColor(BLUE); 
        tft.print("TMC");
        irsend.sendNEC(0x4040609F,32); // CHAINE 60
        delay (100);
        irsend.sendNEC(0x404000FF,32); // 
        delay (50);
        digitalWrite(4, LOW); // met la LED en OFF
        //easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;                  
     }
     
  else (digitalRead(pushButton) == LOW);   
  tft.setCursor(210, 37);
  tft.print("TV BREIZH");
  easyvr.playSound(4, EasyVR::VOL_DOUBLE); // NANOU (A CHANGER)
  digitalRead(pushButton);
  delay (100);
  
  if (digitalRead(pushButton) == HIGH)
     {
        digitalWrite(4, HIGH); // met la LED en ON    
        tft.setCursor(210, 37);
        tft.setTextColor(BLUE); 
        tft.print("TV BREIZH");
        irsend.sendNEC(0x4040A05F,32); // CHAINE 53
        delay (100);
        irsend.sendNEC(0x4040C03F,32); // CHAINE 0
        delay (50);
        digitalWrite(4, LOW); // met la LED en OFF
        //easyvr.playSound(0, EasyVR::VOL_DOUBLE); // BIP DE DEPART
        clearsettings();
        easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
        drawhomeicon();
        return;                  
     }
     
  clearsettings();
  drawhomeicon();
  easyvr.playSound(18, EasyVR::VOL_DOUBLE); // MISE EN VEILLE
  return;
 
  
}

void menu5() {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
  tft.setCursor(22, 37);
  tft.print("Menu 5 B1");
  tft.setCursor(192, 37);
  tft.print("Menu 5 B2");
  tft.setCursor(22, 97);
  tft.print("Menu 5 B3");
  tft.setCursor(192, 97);
  tft.print("Menu 5 B4");
  tft.setCursor(22, 157);
  tft.print("Menu 5 B5");
  tft.setCursor(192, 157);
  tft.print("Menu 5 B6");
}

void settingsscr() {
  // backlight level
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.fillRect(0, 20, 60, 50, RED);
  tft.drawRect(0, 20, 60, 50, WHITE);
  tft.drawRect(80, 20, 160, 50, JJCOLOR);
  tft.fillRect(260, 20, 60, 50, GREEN);
  tft.drawRect(260, 20, 60, 50, WHITE);
  tft.setCursor(22, 33);
  tft.print("-");
  tft.setCursor(282, 33);
  tft.print("+");
  tft.setTextSize(1);
  tft.setCursor(120, 31);
  tft.print("Backlight Level");
  tft.drawRect(110, 48, 100, 10, WHITE);
  blbar();
  // sleep time
  tft.setTextSize(3);
  tft.fillRect(0, 80, 60, 50, RED);
  tft.drawRect(0, 80, 60, 50, WHITE);
  tft.drawRect(80, 80, 160, 50, JJCOLOR);
  tft.fillRect(260, 80, 60, 50, GREEN);
  tft.drawRect(260, 80, 60, 50, WHITE);
  tft.setCursor(22, 93);
  tft.print("-");
  tft.setCursor(282, 93);
  tft.print("+");
  tft.setTextSize(1);
  tft.setCursor(130, 91);
  tft.print("Sleep Time");
  showsleep();
  //?? uncomment this if you want a third adjustable option
  /*
  tft.fillRect(0, 140, 60, 50, RED);
   tft.drawRect(0, 140, 60, 50, WHITE);
   tft.drawRect(80, 140, 160, 50, JJCOLOR);
   tft.fillRect(260, 140, 60, 50, GREEN);
   tft.drawRect(260, 140, 60, 50, WHITE);
   tft.print(22, 153, "-", WHITE, 3);
   tft.print(130, 151, "Thing #3", WHITE);
   tft.print(282, 153, "+", WHITE, 3);
   tft.drawRect(110, 168, 100, 10, WHITE);
   */
  battv = readVcc(); // read the voltage
  itoa (battv, voltage, 10);
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.setCursor(12, 213);
  tft.print(voltage);
  tft.setCursor(60, 213);
  tft.print("mV");
  /*
  battpercent = (battv / 5000) * 100, 2;
  itoa (battpercent, battpercenttxt, 10);
  tft.print(102, 213, battpercenttxt, YELLOW, 2);
  */
}
void sleepinc() { // sleep increese adjustment
  if (sleeptime == 14400000) {
    sleepnever = 1;
    esleep = 12;
    sleeptime = 11111111;
    showsleep();
  }
  if (sleeptime == 3600000) {
    sleeptime = 14400000;
    esleep = 11;
    showsleep();
  }
  if (sleeptime == 1800000) {
    sleeptime = 3600000;
    esleep = 10;
    showsleep();
  }
  if (sleeptime == 1200000) {
    sleeptime = 1800000;
    esleep = 9;
    showsleep();
  }
  if (sleeptime == 600000) {
    sleeptime = 1200000;
    esleep = 8;
    showsleep();
  }
  if (sleeptime == 300000) {
    sleeptime = 600000;
    esleep = 7;
    showsleep();
  }
  if (sleeptime == 120000) {
    sleeptime = 300000;
    esleep = 6;
    showsleep();
  }
  if (sleeptime == 60000) {
    sleeptime = 120000;
    esleep = 5;
    showsleep();
  }
  if (sleeptime == 30000) {
    sleeptime = 60000;
    esleep = 4;
    showsleep();
  }
  if (sleeptime == 20000) {
    sleeptime = 30000;
    esleep = 3;
    showsleep();
  }
  if (sleeptime == 10000) {
    sleeptime = 20000;
    esleep = 2;
    showsleep();
  }
  delay(350);
}
void sleepdec() { // sleep decreese adjustment
  if (sleeptime == 20000) {
    sleeptime = 10000;
    esleep = 1;
    showsleep();
  }
  if (sleeptime == 30000) {
    sleeptime = 20000;
    esleep = 2;
    showsleep();
  }
  if (sleeptime == 60000) {
    sleeptime = 30000;
    esleep = 3;
    showsleep();
  }
  if (sleeptime == 120000) {
    sleeptime = 60000;
    esleep = 4;
    showsleep();
  }
  if (sleeptime == 300000) {
    sleeptime = 120000;
    esleep = 5;
    showsleep();
  }
  if (sleeptime == 600000) {
    sleeptime = 300000;
    esleep = 6;
    showsleep();
  }
  if (sleeptime == 1200000) {
    sleeptime = 600000;
    esleep = 7;
    showsleep();
  }
  if (sleeptime == 1800000) {
    sleeptime = 1200000;
    esleep = 8;
    showsleep();
  }
  if (sleeptime == 3600000) {
    sleeptime = 1800000;
    esleep = 9;
    showsleep();
  }
  if (sleeptime == 14400000) {
    sleeptime = 3600000;
    esleep = 10;
    showsleep();
  }
  if (sleepnever == 1) {
    sleeptime = 14400000;
    sleepnever = 0;
    esleep = 11;
    showsleep();
  }
  delay(350);
}
void showsleep() { // shows the sleep time on the settings page
  tft.fillRect(110, 108, 80, 10, BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  if (sleeptime == 10000) {
    tft.setCursor(130, 108);
    tft.print("10 Seconds");
  }
  if (sleeptime == 20000) {
    tft.setCursor(130, 108);
    tft.print("20 Seconds");
  }
  if (sleeptime == 30000) {
    tft.setCursor(130, 108);
    tft.print("30 Seconds");
  }
  if (sleeptime == 60000) {
    tft.setCursor(136, 108);
    tft.print("1 Minute");
  }
  if (sleeptime == 120000) {
    tft.setCursor(133, 108);
    tft.print("2 Minutes");
  }
  if (sleeptime == 300000) {
    tft.setCursor(133, 108);
    tft.print("5 Minutes");
  }
  if (sleeptime == 600000) {
    tft.setCursor(130, 108);
    tft.print("10 Minutes");
  }
  if (sleeptime == 1200000) {
    tft.setCursor(130, 108);
    tft.print("20 Minutes");
  }
  if (sleeptime == 1800000) {
    tft.setCursor(130, 108);
    tft.print("30 Minutes");
  }
  if (sleeptime == 3600000) {
    tft.setCursor(142, 108);
    tft.print("1 Hour");
  }
  if (sleeptime == 14400000) {
    tft.setCursor(139, 108);
    tft.print("4 Hours");
  }
  if (sleepnever == 1) {
    tft.setCursor(133, 108);
    tft.print("Always On");
  }
}

void option3down() { // adjust option 3 down in the settings screen
}
void option3up() { // adjust option 3 up in the settings screen
}
//custom defined actions - this is where you put your button functions
void m1b1action() {
  signal();
}
void m1b2action() {
  signalact();
}
void m1b3action() {
}
void m1b4action() {
}
void m1b5action() {
}
void m1b6action() {
}
void m2b1action() {
}
void m2b2action() {
}
void m2b3action() {
}
void m2b4action() {
}
void m2b5action() {
}
void m2b6action() {
}
void m3b1action() {
}
void m3b2action() {
}
void m3b3action() {
}
void m3b4action() {
}
void m3b5action() {
}
void m3b6action() {
}
void m4b1action() {
}
void m4b2action() {
}
void m4b3action() {
}
void m4b4action() {
}
void m4b5action() {
}
void m4b6action() {
}
void m5b1action() {
}
void m5b2action() {
}
void m5b3action() {
}
void m5b4action() {
}
void m5b5action() {
}
void m5b6action() {
}
void blightup() { // increase the backlight brightness
  blv = blv + 5;
  if (blv >= 255) {
    blv = 255;
  }
  analogWrite(backlight, blv);
  blbar();
}
void blightdown() { // decrease the backlight brightness
  blv = blv - 5;
  if (blv <= 5) {
    blv = 5;
  }
  analogWrite(backlight, blv);
  blbar();
}
void blbar() { // this function fills the yellow bar in the backlight brightness adjustment
  if (blv < barv) {
    tft.fillRect(111, 49, 98, 8, BLACK);
  }
  backlightbox = map(blv, 1, 255, 0, 98);
  tft.fillRect(111, 49, backlightbox, 8, YELLOW);
  barv = blv;
  delay(25);
}
void ant() {
  tft.fillRect((antpos + 5), 4, 1, 6, WHITE); // draws the "antenna" for the signal indicator
}

void boxes() { // redraw the button outline boxes
  tft.drawRect(0, 20, 150, 50, JJCOLOR);
  tft.drawRect(170, 20, 150, 50, JJCOLOR);
  tft.drawRect(0, 80, 150, 50, JJCOLOR);     
  tft.drawRect(170, 80, 150, 50, JJCOLOR);
  tft.drawRect(0, 140, 150, 50, JJCOLOR);
  tft.drawRect(170, 140, 150, 50, JJCOLOR);
     
}
void signal() { // draws a whit 'signal indicator'
  tft.drawLine((antpos + 4), 4, (antpos + 4), 5, WHITE);
  tft.drawPixel((antpos + 3), 2, WHITE);
  tft.drawPixel((antpos + 3), 7, WHITE);
  tft.drawPixel((antpos + 2), 0, WHITE);
  tft.drawLine((antpos + 2), 3, (antpos + 2), 6, WHITE);
  tft.drawPixel((antpos + 2), 9, WHITE);
  tft.drawPixel((antpos + 1), 1, WHITE);
  tft.drawPixel((antpos + 1), 8, WHITE);
  tft.drawLine(antpos, 2, antpos, 7, WHITE);
  tft.drawLine((antpos + 6), 4, (antpos + 6), 5, WHITE);
  tft.drawPixel((antpos + 7), 2, WHITE);
  tft.drawPixel((antpos + 7), 7, WHITE);
  tft.drawPixel((antpos + 8), 0, WHITE);
  tft.drawLine((antpos + 8), 3, (antpos + 8), 6, WHITE);
  tft.drawPixel((antpos + 8), 9, WHITE);
  tft.drawPixel((antpos + 9), 1, WHITE);
  tft.drawPixel((antpos + 9), 8, WHITE);
  tft.drawLine((antpos + 10), 2, (antpos + 10), 7, WHITE);
}
void signalact() { // draws a red'signal indicator'
  tft.drawLine((antpos + 4), 4, (antpos + 4), 5, RED);
  tft.drawPixel((antpos + 3), 2, RED);
  tft.drawPixel((antpos + 3), 7, RED);
  tft.drawPixel((antpos + 2), 0, RED);
  tft.drawLine((antpos + 2), 3, (antpos + 2), 6, RED);
  tft.drawPixel((antpos + 2), 9, RED);
  tft.drawPixel((antpos + 1), 1, RED);
  tft.drawPixel((antpos + 1), 8, RED);
  tft.drawLine(antpos, 2, antpos, 7, RED);
  tft.drawLine((antpos + 6), 4, (antpos + 6), 5, RED);
  tft.drawPixel((antpos + 7), 2, RED);
  tft.drawPixel((antpos + 7), 7, RED);
  tft.drawPixel((antpos + 8), 0, RED);
  tft.drawLine((antpos + 8), 3, (antpos + 8), 6, RED);
  tft.drawPixel((antpos + 8), 9, RED);
  tft.drawPixel((antpos + 9), 1, RED);
  tft.drawPixel((antpos + 9), 8, RED);
  tft.drawLine((antpos + 10), 2, (antpos + 10), 7, RED);
}
void drawhomeicon() { // draws a white home icon
  //tft.drawRect(0, 190, 150, 90, JJCOLOR);
  tft.setTextColor(WHITE);
  tft.setCursor(40, 105);
  tft.print("APPUYER SUR LE BOUTON");
  tft.drawLine(280, 219, 299, 200, WHITE);
  tft.drawLine(300, 200, 304, 204, WHITE);
  tft.drawLine(304, 203, 304, 200, WHITE);
  tft.drawLine(305, 200, 307, 200, WHITE);
  tft.drawLine(308, 200, 308, 208, WHITE);
  tft.drawLine(309, 209, 319, 219, WHITE);
  tft.drawLine(281, 219, 283, 219, WHITE);
  tft.drawLine(316, 219, 318, 219, WHITE);
  tft.drawRect(284, 219, 32, 21, WHITE);
  tft.drawRect(295, 225, 10, 15, WHITE);
}

void clearmessage() {
  tft.fillRect(0, 320, 0, 480, BLACK); // black out the inside of the message box
}
void drawbatt() {
  battv = readVcc(); // read the voltage
  if (battv < battold) { // if the voltage goes down, erase the inside of the battery
    tft.fillRect(298, 2, 18, 6, BLACK);
  }
  battfill = map(battv, 3000, 4850, 2, 18); // map the battery voltage 3000 nis the low, 4150 is the high
  if (battfill > 7) { // if the battfill value is between 8 and 18, fill with green
    tft.fillRect(298, 2, battfill, 6, GREEN);
  }
  else { // if the battfill value is below 8, fill with red
    tft.fillRect(298, 2, battfill, 6, RED);
  }
  battold = battv; // this helps determine if redrawing the battfill area is necessary
}


// Original sketch header
/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

