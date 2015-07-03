/* 

Description:   Prototype sketch for catchprhase device
Author:        Alex D Glover (alexdglover@gmail.com)
Initial Date:  21-Jun-2015
License:       GPLv2.1 (due to libraries being licensed under GPLv2.1)

Credits:
Code for timer functionality - http://playground.arduino.cc/Main/CountDownTimer
Original TFT library - https://github.com/Seeed-Studio/TFT_Touch_Shield_V2
Original Touchscreen Library - https://github.com/Seeed-Studio/Touch_Screen_Driver
Modified TFT Library for screen orientation - http://forum.arduino.cc/index.php?topic=150900.0

*/

// Paint application - Demonstate both TFT and Touch Screen
#include <stdint.h>
#include <SeeedTouchScreen.h>
#include <TFTv2.h>
#include <SPI.h>

int ColorPaletteHigh = 30;
int color = WHITE;  //Paint brush color
unsigned int colors[8] = {BLACK, RED, GREEN, BLUE, CYAN, YELLOW, WHITE, GRAY1};

//Timer related variables
unsigned long Watch, _micro, time = micros();
unsigned int Clock = 0, R_clock;
boolean Reset = false, Stop = false, Paused = false;
volatile boolean timeFlag = false;

//List of 'catchphrases' and the current 'catchphrase
char* listOfWords[]={"test", "puzzle", "watermelon","tree", "boat","squirrel"};
char* currentWord = "";
boolean needNewWord = true;

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate

TouchScreen ts = TouchScreen(XP, YP, XM, YM); //init TouchScreen port pins

void setup()
{
    Tft.TFTinit();  //init TFT library
    Serial.begin(9600);
    //Draw the pallet
    /*for(int i = 0; i<8; i++)
    {
        Tft.fillRectangle(i*30, 0, 30, ColorPaletteHigh, colors[i]);
    }*/
    
    randomSeed(analogRead(0));
}

boolean CountDownTimer()
{
  static unsigned long duration = 1000000; // 1 second
  timeFlag = false;

  if (!Stop && !Paused) // if not Stopped or Paused, run timer
  {
    // check the time difference and see if 1 second has elapsed
    if ((_micro = micros()) - time > duration ) 
    {
      Clock--;
      timeFlag = true;

      if (Clock == 0) // check to see if the clock is 0
        Stop = true; // If so, stop the timer

     // check to see if micros() has rolled over, if not,
     // then increment "time" by duration
      _micro < time ? time = _micro : time += duration; 
    }
  }
  return !Stop; // return the state of the timer
}

void ResetTimer()
{
  SetTimer(0,0,60);
  Stop = false;
}

void StartTimer()
{
  Watch = micros(); // get the initial microseconds at the start of the timer
  Stop = false;
  Paused = false;
}

void SetTimer(unsigned int hours, unsigned int minutes, unsigned int seconds)
{
  // This handles invalid time overflow ie 1(H), 0(M), 120(S) -> 1, 2, 0
  unsigned int _S = (seconds / 60), _M = (minutes / 60);
  if(_S) minutes += _S;
  if(_M) hours += _M;

  Clock = (hours * 3600) + (minutes * 60) + (seconds % 60);
  R_clock = Clock;
  Stop = false;
}

boolean TimeHasChanged()
{
  return timeFlag;
}

void getNewWord()
{
  int listOfWordsLength = sizeof(listOfWords) / sizeof(int);
  long randomLong = random(listOfWordsLength);
  Serial.print("Random number is ");
  Serial.println(randomLong);
  currentWord = listOfWords[randomLong];
  Serial.println(currentWord);
}

void loop()
{
    // a point object holds x y and z coordinates.
    Point p = ts.getPoint();

    //map the ADC value read to into pixel co-ordinates

    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);

    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!

    if (p.z > __PRESURE) {
        // Detect  paint brush color change
        if(p.y < ColorPaletteHigh+2)
        {
            color = colors[p.x/30];
        }
        else
        {
            Tft.fillCircle(p.x,p.y,2,color);
        }
    }
    
    if(needNewWord)
    {
      getNewWord();
      needNewWord = false;
    }
    
    //show phrase
    Tft.drawString(currentWord,60,100  ,3,WHITE);
    //Tft.drawString("World!!",60,220,4,WHITE);
    
    CountDownTimer(); // run the timer

    // this prevents the time from being constantly shown.
    if (TimeHasChanged() ) 
    {
      //Show timer/clock 
    }
    
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
