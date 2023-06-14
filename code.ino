#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

Servo myservo;

//for led
int Contrast = 75;
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5); //pin Digital

//for ultrasonic
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;


int car = 0;
int temp = 0;

//for buzzer
int buzzer = 4;

//for gas sensor MQ-2
int gasSensor = A8;
int sensorThresh = 400;
int firePrint = 0;

//for light sensor
int ledPin = 12;
int ldrPin = A7;

#define Password_Lenght 7 // Give enough room for six chars + NULL char

int pos = 0;    // variable to store the servo position

char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
char Master[Password_Lenght] = "123456";
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

//for keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3' , 'A'},
  {'4', '5', '6' , 'B'},
  {'7', '8', '9' , 'C'},
  {'*', '0', '#' , 'D'}
};
byte rowPins[ROWS] = {36, 34, 32, 30}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {28, 26, 24, 22}; //connect to the column pinouts of the keypad

bool door = true;

//for Motor A & B
int enA = 31;
int in1 = 33;
int in2 = 35;

int enB = 37;
int in3 = 39;
int in4 = 41;

int speeed = 100;

Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); //initialize an instance of class NewKeypad

void setup()
{
  myservo.attach(13);
  analogWrite(7, Contrast);
  lcd.begin(16, 2);
  lcd.print("   MSD PROJECT  ");
  lcd.setCursor(0, 1);
  lcd.print("Ultimate Garage ");
  delay(3000);
  lcd.clear();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(gasSensor, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);

}
void loop()
{
  if (firePrint == 0)
  {
    fire();
  }

  if (temp == 0)
  {
    countDis();
    temp++;
  }
  if (door == 0)
  {
    light();
    customKey = customKeypad.getKey();

    if (customKey == '#')

    {
      lcd.clear();
      ServoClose();
      lcd.clear();
      lcd.print("  Door is close");
      delay(3000);
      door = 1;
      temp = 0;
    }

    else if (customKey == 'B' || customKey == 'A')
    {
      if (customKey == 'A')
      {
        tradeIn();
      }
      else
      {
        tradeOut();
      }
    }
  }

  else Open();
}

void clearData()
{
  while (data_count != 0)
  { // This can be used for any array size,
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}

void ServoOpen()
{
  print();
  for (pos = 90; pos >= 0; pos -= 5) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void ServoClose()
{
  for (pos = 0; pos <= 90; pos += 5) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void Open()
{

  lcd.setCursor(0, 0);
  lcd.print(" Enter Password");



  customKey = customKeypad.getKey();
  if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
  {
    Data[data_count] = customKey; // store char into data array
    lcd.setCursor(data_count, 1); // move cursor to show each new char
    lcd.print(Data[data_count]); // print char at said cursor
    data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
  }

  if (data_count == Password_Lenght - 1) // if the array index is equal to the number of expected chars, compare data to master
  {
    if (!strcmp(Data, Master)) // equal to (strcmp(Data, Master) == 0)
    {
      lcd.clear();
      ServoOpen();
      lcd.clear();
      lcd.print("  Door is Open");
      door = 0;
      delay(3000);


    }
    else
    {
      lcd.clear();
      lcd.print("  Wrong Password");
      delay(1000);
      door = 1;
    }
    clearData();
  }
}

void countDis()
{
  //Let the garage length is 25cm

  digitalWrite(trigPin, LOW); //clear trigger pin
  delay(2);
  digitalWrite(trigPin, HIGH);
  delay(10); // Set trigPin on HIGH state for 10micro seconds
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the
  //echoPin, returns the sound wave travel time in
  //microseconds
  distance = (duration * 0.034) / 2;
  Serial.print("Distance(cm): ");
  Serial.println(distance);
  delay(1000);
  if (distance < 2)
  {
    car = 1;

  }
  else
  {
    car = 0;
  }
}

void print()
{

  if (car == 1)
  {
    lcd.clear();
    lcd.print(" Car is here ");
    delay(3000);
  }
  else
  {
    lcd.clear();
    lcd.print(" Car is not here ");
    delay(3000);
  }
}

void fire()
{
  int analogValue = analogRead(gasSensor);
  Serial.print("Gas value= ");
  Serial.print(analogValue);
  Serial.print("\n");
  //delay(1000);
  if (analogValue > sensorThresh)
  {

    tone(buzzer, 5000);

    if (firePrint == 0)
    {
      firePrint = 1;
      for (int i = 0; i < 2; i++)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("      ALERT     ");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("    EVACUATE    ");
        delay(1000);
        lcd.clear();
      }
    }
  }
  else
  {
    noTone(buzzer);
    firePrint = 0;
  }
}

void light()
{
  int ldrStatus = analogRead(ldrPin);

  if (ldrStatus >= 800) {

    digitalWrite(ledPin, HIGH);
    Serial.println(ldrStatus);
    Serial.println("LDR is DARK, LED is ON");
  }

  else {

    digitalWrite(ledPin, LOW);
    Serial.println("LED OFF");

  }
}

void tradeOut()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, speeed);
  digitalWrite(in4, HIGH);
  digitalWrite(in3, LOW);
  analogWrite(enB, speeed);
  delay(5000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
}


void tradeIn()
{
  digitalWrite(in2, HIGH);
  digitalWrite(in1, LOW);
  analogWrite(enA, speeed);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, speeed);
  delay(5000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
}
