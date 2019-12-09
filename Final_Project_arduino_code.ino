#include <Servo.h>

Servo myservo;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo
Servo myservo3;  // create servo object to control a servo

int myservoCurrent = 60;  //variable for moving servos
int myservo2Current = 30;  //variable for moving servos
int myservo3Current = 40;  //variable for moving servos

const int dtime = 15; // the delay time for controlling the speed of the servos
//the higher the time, the slower the servo

const int ledPin = 13; // the pin that the LED is attached to
const int magnet = 12;  // Pin for magnet

// declare variables for setting up serial communication
const byte buffSize = 40;
unsigned int inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
byte coordinates[4]; // stores info about the shape and the three servo angles


void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(magnet, OUTPUT);
  Serial.begin(9600); // send and receive at 9600 baud

  // preset values of servo motors to the values for the home position
  myservo.write(60);
  myservo2.write(30);
  myservo3.write(40);

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(10);  // attaches the servo on pin 10 to the servo object
  myservo3.attach(11);  // attaches the servo on pin 11 to the servo object
  delay (2000);
}
void loop() {
  // put your main code here, to run repeatedly:
  getDataFromPC();
  if (newDataFromPC) {
    sendSuspendCmd();
    digitalWrite(ledPin, HIGH);
    pickAndPlace();
    digitalWrite(ledPin, LOW);
    sendEnableCmd();
    sendCoordinatesToPC();
    newDataFromPC = false;
    goHome();
  }

}
void sendSuspendCmd() {
  // send the suspend-true command
  Serial.println("<S1>");
}
void sendEnableCmd() {
  // send the suspend-false command
  Serial.println("<S0>");
}
void sendCoordinatesToPC() {
  // send the angle and shape data back to the PC
  Serial.print("<P");
  Serial.print(coordinates[0]);
  Serial.print(",");
  Serial.print(coordinates[1]);
  Serial.print(",");
  Serial.print(coordinates[2]);
  Serial.print(",");
  if (coordinates[3] == 1)
  Serial.print("Square");
  else if (coordinates[3] == 2)
  Serial.print("Triangle");
  Serial.println(">");
} 
void getDataFromPC() {
  // receive data from PC and save it into inputBuffer
  if (Serial.available() > 0) {
    char x = Serial.read();
    // the order of these IF clauses is significant
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      coordinates[0] = inputBuffer[0];
      coordinates[1] = inputBuffer[1];
      coordinates[2] = inputBuffer[2];
      coordinates[3] = inputBuffer[3];
    }
    if (readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }
    if (x == startMarker) {
      bytesRecvd = 0;
      readInProgress = true;
    }
  }
}
void pickAndPlace() {
  // Code for the application of the robot
  // Turn on the magnet before moving to shape  
  digitalWrite(magnet, HIGH);
  delay (200);
  
  // move the servos to the correct position for picking shape
  moveServos(coordinates[0], coordinates[1], coordinates[2]);
  delay (3000);

  // send the robot home before placing part
  goHome();
  delay (2000);

  // Logic for dropping off square or triangle
  if (coordinates[3] == 1)  //part is a square
  {
    // move to the right side to drop off square
    moveServos(0, 60, 40);
    delay (2000);
  }
  else if (coordinates[3] == 2) //part is a triangle
  {
    // move to the left side to drop of triangle
    moveServos(145, 60, 40); 
    delay (2000);
  }

  // turn off magnet to drop the shape
  digitalWrite(magnet, LOW);
  delay (2000);
}
void moveServos(int a, int b, int c){
  // This code moves the servos to the correct angle from the previous position
  // Each if statement moves a servo at a controlled speed and then updates the current position of the servo
  // Note: the order of the if statements is important for proper moving
  if (myservo2Current > b )
    {
      for (int i = myservo2Current; i > b; i--)
      {
        myservo2.write(i);
        delay(dtime);
      }
      myservo2Current = b;
    }
    
  if (myservoCurrent <= a )
  {
    for (int i = myservoCurrent; i < a; i++)
    {
      myservo.write(i);
      delay(dtime);
    }
    myservoCurrent = a;
  }
  if (myservoCurrent > a )
  {
    for (int i = myservoCurrent; i > a; i--)
    {
      myservo.write(i);
      delay(dtime);
    }
    myservoCurrent = a;
  }
  
  if (myservo3Current <= c )
  {
    for (int i = myservo3Current; i < c; i++)
    {
      myservo3.write(i);
      delay(dtime);
    }
    myservo3Current = c;
  }
  if (myservo3Current > c )
  {
    for (int i = myservo3Current; i > c; i--)
    {
      myservo3.write(i);
      delay(20);
    }
    myservo3Current = c;
  }
  
  if (myservo2Current <= b )
  {
    for (int i = myservo2Current; i < b; i++)
    {
      myservo2.write(i);
      delay(dtime);
    }
    myservo2Current = b;
  }
}
void goHome() {
  // Moves the servos to the home position
    moveServos(60, 30, 40);
}
