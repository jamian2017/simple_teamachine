/*

Datum: 10.12.2018

Titel: The Simple Tea Bag Maschine

Sprache: C++/Arduino

Erstellt von: Jamian Rajakone, Jonathan Carona, David Riggenberg

Version: V1.2

 

Beschreibung:

Der Servomotr bewegt den Teebeutel. Mit den Tasten kann man den Timer einstellen und mit der Start-Taste kann man die Operation starten.

Um den Timer einstellen zu können, muss die Drucktaste aktiviert sein. D.h. eine Taste muss auf dem Plattform sein.

*/

 

// Servo settings

// https://www.arduino.cc/en/Reference/ServoWrite

#include <Servo.h>

Servo ServoMotor;

int ServoPin = A0;

 

// InputTimerMin

byte InputTimerMin = 2; // Default -> 2 Min

 

// TimerLoop -> set true after pressing the StartButton

bool TimerLoop = false;

 

// ButtonPin pins

int StopButtonPin = 2;

int StartButtonPin = 9;

int IncreaseButtonPin = 8;

int DecreaseButtonPin = 7;

 

// DruckButtonPin

int PressedButtonPin = 6;

 

// timer interrupt helping variable

int timerCounter = 0;

 

// current System millis

unsigned long timer0millis = 0;

 

// Lcd Config

// https://www.arduino.cc/en/Tutorial/HelloWorld

#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 0;

LiquidCrystal Lcd(rs, en, d4, d5, d6, d7);

 

void setup()

{

  // ServoMotor Configuration

  ServoMotor.attach(A0);

  ServoMotor.write(0);

 

  // ButtonPin initialization

  pinMode(StartButtonPin, INPUT_PULLUP);

  pinMode(IncreaseButtonPin, INPUT_PULLUP);

  pinMode(DecreaseButtonPin, INPUT_PULLUP);

 

  // PressedButtonPin initialization

  pinMode(PressedButtonPin, INPUT_PULLUP);

 

  // set up the Lcd's number of columns and rows:

  Lcd.begin(16, 2);

 

  // TimerInterrupt Configuration

  cli(); //stop interrupts

 

  //set timer0 interrupt at 2kHz

  TCCR0A = 0; // set entire TCCR0A register to 0

  TCCR0B = 0; // same for TCCR0B

  TCNT0 = 0;  //initialize counter value to 0

  // set compare match register for 2khz increments

  OCR0A = 248; // = (16*10^6) / (2000*64) - 1 (must be <256)

  // turn on CTC mode

  TCCR0A |= (1 << WGM01);

  // Set CS01 and CS00 bits for 64 prescaler

  TCCR0B |= (1 << CS01) | (1 << CS00);

  // enable timer compare interrupt

  TIMSK0 |= (1 << OCIE0A);

  sei();

}

 

// Main Program

void loop()

{

  // PressedButtonPin pressed -> the Programm run only if the Press-Button is pressed

  if (digitalRead(PressedButtonPin) == LOW)

  {

    // Timer Output Lcd

    Lcd.clear();

    Lcd.print(InputTimerMin);

 

    // StartButton pressed

    if (digitalRead(StartButtonPin) == LOW)

    {

      // Set servo to 180 degree

      ServoMotor.write(180);

      delay(200);

 

      Lcd.clear();

      Lcd.write("Start");

 

      TimerLoop = true; // set TimerLoop

    }

 

    // Increase Button pressed

    if (digitalRead(IncreaseButtonPin) == LOW)

    {

      InputTimerMin++;

      delay(20000);

    }

 

    // DecreaseButton pressed

    if (digitalRead(DecreaseButtonPin) == LOW)

    {

      InputTimerMin--;

      delay(20000);

    }

 

    // Loop for waiting => get and print temperature

    while (TimerLoop)

    {

      // print seconds

      Lcd.setCursor(0, 1);

      Lcd.print(timer0millis / 1000);

    }

 

    // wait 100 ms

    delay(1000);

  }

}

 

// Timer interrupt -> cancel the programm if -> InterruptCounter equal to InputTimerMin in second

void Interrupt_CancelProgram()

{

  ServoMotor.write(0);

  TimerLoop = false;

  delay(200);

  Lcd.clear();

  Lcd.write("Cancel");

}

 

ISR(TIMER0_COMPA_vect)

{ //timer0 interrupt 2kHz toggles pin 8

  if (TimerLoop)

  {

    timer0millis++;

    if (timer0millis / 1000 > InputTimerMin * 60)

    {

      // Timer Interrupt

      Interrupt_CancelProgram();

      timer0millis = 0;

    }

  }

}