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
Basic Shuffle Code - http://forum.arduino.cc/index.php?topic=43424.0

*/

// Paint application - Demonstate both TFT and Touch Screen
#include <stdint.h>
#include <SeeedTouchScreen.h>
#include <SPI.h>
#include <TFTv2-Text_Direction.h>


int ColorPaletteHigh = 30;
int color = WHITE;  //Paint brush color
unsigned int colors[8] = {BLACK, RED, GREEN, BLUE, CYAN, YELLOW, WHITE, GRAY1};

//Timer related variables
unsigned long Watch, _micro, time = micros();
unsigned int Clock = 0, R_clock;
boolean Reset = false, Stop = false, Paused = false;
volatile boolean timeFlag = false;

//List of 'buzzwords' and the current 'buzzword'
char* listOfWords[]={"test", "puzzle", "watermelon", "tree", "boat", "squirrel", "Miley Cyrus", "America", "Facebook", "twerk", "hashtag", "Bye Felicia", "swerve", "on point", "Rihanna", "North West", "Donald Trump", "Lil' Wayne", "Hellen Keller", "wrecking ball", "Einstein", "Tech Nine", "Queen", "Bohemian", "Rwanda", "Iraq", "Selma", "76ers", "Knicks", "scrub", "Cardinals", "Mariners", "Brewers", "Orioles", "Iggy Azalea", "The Clash", "Katy Perry", "John Legend", "Las Vegas", "Portland", "Milwaukee", "cookie jar", "Goldschlager", "Fireball", "Jagermeister", "bench press", "hangover", "munchies"};
int listOfWordsLength = sizeof(listOfWords) / sizeof(int);
char* currentWord = "";
int currentWordIndex = 0;
boolean needNewWord = false;
int score = 0;
boolean gameStarted = false;

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate

TouchScreen ts = TouchScreen(XP, YP, XM, YM); //init TouchScreen port pins

void setup()
{
    Tft.TFTinit();  //init TFT library
    Serial.begin(9600);
    
    Tft.setDisplayDirect(UP2DOWN);
    
    // Sample static noise from Analog pin 0 in order to seed the random number generator
    randomSeed(analogRead(0));
    
    // shuffle the list of words
    long randomLong = random(listOfWordsLength);
    Serial.print("Random number is ");
    Serial.println(randomLong);
    currentWord = listOfWords[randomLong];
    Serial.println(currentWord);
    for (int a=0; a<listOfWordsLength; a++)
    {
      long r = random(a,(listOfWordsLength-1)); // dont remember syntax just now, random from a to 8 included.
      char* temp = listOfWords[a];
      listOfWords[a] = listOfWords[r];
      listOfWords[r] = temp;
    }
    
    // Show start button
    Tft.fillRectangle(80, 60, 80, 200,BLUE);
    Tft.drawString("START!", 130, 70, 4, WHITE);
    
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
  // Intentionally commented out - want to start the timer as a separate function
  // Stop = false;
  time = micros();
}

boolean TimeHasChanged()
{
  return timeFlag;
}

void getNewWord()
{
  Tft.fillRectangle(160, 0, 80, 320,BLACK);
  currentWordIndex++;
  currentWordIndex = currentWordIndex % listOfWordsLength;
  currentWord = listOfWords[currentWordIndex];
  Serial.println(currentWord);
  
  // Get length of currentWord
  int currentWordLength = strlen(currentWord);

  // calculate ideal x-axis positioning based on size of word
  int xAxisPlacement = (160 - ((currentWordLength * 23) / 2));
  
  // Note: Since we are using the TFT shield in the "landscape" orientation,
  // the x and y values (as humans understand them) have been transposed; 
  // the second variable in the drawString function below
  // actually references the vertical axis (0 being the bottom, 240 being the top).
  // The third variable is the position on the horizontal access (0 being the 
  // left-most value, 320 being the right-most value).
  Tft.drawString(currentWord,200,xAxisPlacement,3,WHITE);
}

void drawScore()
{
  Tft.fillRectangle(110, 0, 60, 160,BLACK);
  
  char scoreString[50];
  sprintf (scoreString, "Score: %d", score);
  Serial.println(scoreString);
  Tft.drawString(scoreString,145,20,2,WHITE);
  
}

void drawTimer()
{
  if(gameStarted)
  {
      Tft.fillRectangle(110, 260, 60, 80,BLACK);
  
      char clockString[50];
      sprintf (clockString, "Time: %d", Clock);
      Serial.println(clockString);
      Tft.drawString(clockString,145,180,2,WHITE);
  }
  
}

void startGame()
{
  SetTimer(0,0,60);
  score = 0;
  Tft.fillScreen();
  drawScore();
  needNewWord = true;
  
  // draw +1 button
  Tft.fillRectangle(40, 40, 60, 100,GREEN);
  Tft.drawString("+1", 80, 65, 3, WHITE);
  
  // draw pass button
  Tft.fillRectangle(40, 180, 60, 100,RED);
  Tft.drawString("PASS", 80, 185, 3, WHITE); 
  
  gameStarted = true;
  StartTimer();
}

void gameOver()
{
  gameStarted = false;
  Tft.fillScreen();
  Tft.drawString("GAME", 200, 60, 6, RED);
  Tft.drawString("OVER", 140, 60, 6, RED);
  
  char scoreString[50];
  sprintf (scoreString, "Final Score: %d", score);
  Serial.println(scoreString);
  Tft.drawString(scoreString,60,45,2,WHITE);
}

boolean plus1ButtonPressed()
{
    // a point object holds x y and z coordinates.
    Point p = ts.getPoint();

    //map the ADC value read to into pixel co-ordinates

    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
    
    return ((p.x > 40) & (p.x < 100) & (p.y >= 40) & (p.y <= 140));
}

boolean passButtonPressed()
{
    // a point object holds x y and z coordinates.
    Point p = ts.getPoint();

    //map the ADC value read to into pixel co-ordinates

    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
    
    return ((p.x > 40) & (p.x < 100) & (p.y >= 180) & (p.y <= 280));
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
        // Check if any of the buttons have been pushed
        if(!gameStarted)
        {
          startGame(); 
        }
        else if (passButtonPressed())
        {
            needNewWord = true;
        }
        else if (plus1ButtonPressed())
        {
            // Increment and re-draw score
            score++;
            drawScore();
            needNewWord = true;
        }
    }
    
    if(needNewWord)
    {
      getNewWord();
      needNewWord = false;
    }
    

    if(gameStarted)
    {
        if(!CountDownTimer())  // run the timer and execute gameOver if returns false
        {
            gameOver();
        } 
    }

    // this prevents the time from being constantly shown.
    if (TimeHasChanged() ) 
    {
      //Show timer/clock
      drawTimer();
    }
    
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
