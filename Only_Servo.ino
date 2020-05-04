#include <Arduino_FreeRTOS.h>
#include <Servo.h>
#include <Keypad.h>
#define buzzer 10

void TaskDoor_LockUnlock( void *pvParameters );
void TaskBlink( void *pvParameters );

Servo ServoMotor;
char* password = "888";
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

int position = 0;
byte rowPins[ROWS] = { 8, 7, 6, 9 };
byte colPins[COLS] = { 5, 4, 3, 2 };
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int RedpinLock = 12;
int GreenpinUnlock = 13;

void setup()
{
  Serial.begin (9600);
  ServoMotor.attach(11);

  xTaskCreate(
    TaskDoor_LockUnlock
    ,  (const portCHAR *)"Door LockUnlock"   // A name just for humans
    ,  256 // Stack size
    ,  NULL
    ,  1 // priority
    ,  NULL );

  xTaskCreate(
    TaskBlink
    ,  (const portCHAR *) "Led Blink"
    ,  128 // This stack size can be checked & adjusted by reading Highwater
    ,  NULL
    ,  2  // priority
    ,  NULL );
}

void loop()
{
  //Empty. Work is in Task
}

//------------------------------------------------
//---------------------Task----------------------
//-----------------------------------------------


void TaskDoor_LockUnlock(void *pvParameters) {

  (void) pvParameters;

  for (;;) {
    char key = keypad.getKey();
    if (key == '*' || key == '#')
    {
      position = 0;
      digitalWrite(RedpinLock, HIGH);
      digitalWrite(GreenpinUnlock, LOW);
      ServoMotor.write(11);
      tone(buzzer, 700, 100);
    }
    if (key == password[position])
    {
      position ++;
    }
    if (position == 3)
    {
      digitalWrite(RedpinLock, LOW);
      digitalWrite(GreenpinUnlock, HIGH);
      ServoMotor.write(90);
      tone(buzzer, 700, 100);
    }
    delay(100);
  }
}
void TaskBlink(void *pvParameters) {

  (void) pvParameters;
 pinMode(LED_BUILTIN, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
  }
}



