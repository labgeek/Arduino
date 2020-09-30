//First swag at source code for 4WD robot car (2014)
//labgeek@gmail.com

#include <Servo.h>        //add Servo Motor library            
#include <NewPing.h>      //add Ultrasonic sensor library

#define TRIG_PIN 8 // Pin 8 on the Motor Drive Shield soldered to the ultrasonic sensor
#define ECHO_PIN 9 // Pin 9 on the Motor Drive Shield soldered to the ultrasonic sensor
#define MAX_DISTANCE 300 // sets maximum useable sensor measuring distance to 300cm
#define MAX_SPEED 200 // sets speed of DC traction motors to 150/250 or about 70% of full speed - to get power drain down.
#define MAX_SPEED_OFFSET 67 // this sets offset to allow for differences between the two DC traction motors
#define CRASH_DIST 40 // sets distance at which robot stops and reverses to 30cm
#define TURN_DIST COLL_DIST+20 // sets distance at which robot veers away from object
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // sets up sensor library to use the correct pins to measure distance.

 // Setup Motor A (front and rear) pins
  int enableA = 1;
  int pinA1 = 3;
  int pinA2 = 2;
  // Setup Motor B (front and rear) pins
  int enableB = 6;
  int pinB1 = 5;
  int pinB2 = 4;
  int distance = 100;
  
Servo myservo;  // create servo object to control a servo 
int leftDistance, rightDistance; //distances on either side
int curDist = 0;
String motorSet = "";
int speedSet = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(7);  // attaches the servo on pin 7
  //myservo.attach(10); //this is for later, attaching second ultrasonic sensor
  myservo.write(65); // tells the servo to position at 65 (straight ahead)
  delay(1000); // delay for one second
  //checkRoute();  //checks the route to find the best route to begin the travel

//  myservo.write(0); 
//  delay(1000);
//  distance = readPing2();
//  Serial.println("Checking Right...");
//  Serial.println(distance);
//  
//  myservo.write(65);
//  delay(1000); 
//  distance = readPing2();
//  Serial.println("Checking Center...");
//  Serial.println(distance);
//
//
//  myservo.write(145);
//  delay(1000);
//   distance = readPing2();
//  Serial.println("Checking Left...");
//  Serial.println(distance);

  for(int pos = 180; pos >= 0; pos-=10)
  {
    myservo.write(pos);
    delay(150);
  }
  
}

void loop() {
  //checkForward();  //check that if the Obstacle avoiding Robot is supposed to be moving forward
  //checkPath();
  myservo.write(65);  // move eyes forward (70 is straight ahead for my robot, not 90
  delay(50);
  curDist = readPing2();   // read distance
  Serial.println(curDist);
  moveForward();
 
  if (curDist < CRASH_DIST){
    changePath();
    //delay(500);
    //backward();
    
  }
}


void changePath() {
  totalHalt();   // stop forward movement
  myservo.write(10);  // check distance to the right
    delay(100);
    rightDistance = readPing2(); //set right distance
    delay(100);
    myservo.write(133);  // check distance to the left
    delay(100);
    leftDistance = readPing2(); //set left distance
    delay(100);
    myservo.write(65); //return to center
    delay(100);
    compareDistance();
  }

/**
 * Finds the longest distance to the left, right, and center
 */
  void compareDistance()   // find the longest distance
{
  if (leftDistance>rightDistance) //if left is less obstructed 
  {
    backward();
    totalHalt();
    left();
  }
  else if (rightDistance>leftDistance) //if right is less obstructed
  {
    backward();
    totalHalt();
    right();
  }
   else //if they are equally obstructed
  {
    Serial.println("Left and Right distances are equal");
  }
}

  void compareDistanceBetter()   // find the longest distance
{
  for (int pos = 144; pos >= 36; pos -= 18){
  if (leftDistance>rightDistance) //if left is less obstructed 
  {
    backward();
    totalHalt();
    left();
  }
  else if (rightDistance>leftDistance) //if right is less obstructed
  {
    backward();
    totalHalt();
    right();
  }
   else //if they are equally obstructed
  {
    Serial.println("Left and Right distances are equal");
  }
  }
}
void motorAon() {
 digitalWrite (enableA, HIGH);
}
void motorBon() {
 digitalWrite (enableB, HIGH);
}

void motorAforward() {
 //digitalWrite (pinA1, LOW);
 //digitalWrite (pinA2, HIGH);
 analogWrite(pinA1, 150);
 analogWrite(pinA2, 150);
}
void motorBforward() {
 //digitalWrite (pinB1, LOW);
 //digitalWrite (pinB2, HIGH);
  analogWrite(pinB1, 150);
 analogWrite(pinB2, 150);
}

void motorAstop() {
 digitalWrite (pinA1, HIGH);
 digitalWrite (pinA2, HIGH);
}

int readPing2(){ 
  delay(70); 
  int cm = sonar.ping_cm(); 
  if (cm==0){ 
    cm=250; } 
    return cm; }

void motorBstop() {
 digitalWrite (pinB1, HIGH);
 digitalWrite (pinB2, HIGH);
}


//correct
void moveForward() {
  Serial.println("Moving forward");
  motorAon();
  motorBon();
  motorAforward();
  motorBforward();
  delay(200);
}

//void turnLeft()
//{
//  digitalWrite (pinA1, LOW);
// digitalWrite (pinA2, HIGH);
//digitalWrite (pinB1, HIGH);
// digitalWrite (pinB2, LOW);
//}




void backward () {
 Serial.println("Moving backwards");
 motorAbackward();
 motorBbackward();
 delay(500);
}

void right () {
   Serial.println("Turning Right");
   motorAbackward();
   motorBforward();
   delay (500);
}
void left () {
   Serial.println("Turning Left");
 motorAforward();
 motorBbackward();
 delay (500);
}

void totalHalt()
{
 Serial.println("Halting");
 digitalWrite (pinA1, HIGH);
 digitalWrite (pinA2, HIGH);
 digitalWrite (pinB1, HIGH);
 digitalWrite (pinB2, HIGH);
 myservo.write(65);
 delay(500);
}


void disableMotors() {
 motorAoff();
 motorBoff();
}

void motorAoff() {
 digitalWrite (enableA, LOW);
}
void motorBoff() {
 digitalWrite (enableB, LOW);
}

void motorAbackward() {
 digitalWrite (pinA1, HIGH);
 digitalWrite (pinA2, LOW);
}
void motorBbackward() {
 digitalWrite (pinB1, HIGH);
 digitalWrite (pinB2, LOW);
}
//  for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }

//void checkCourse() { // we're about to hit something so move backwards, stop, find where the empty Route is.
//  moveBackward();
//  delay(500);
//  moveStop();
//  setCourse();
//}

//void checkRoute() {
//  int curLeft = 0;
//  int curFront = 0;
//  int curRight = 0;
//  int curDist = 0;
//  myservo.write(144); // set servo to face left 54-degrees from forward
//  delay(120); // wait 120milliseconds for servo to reach position
//  for (pos = 144; pos > = 36; pos -= 18) // loop to sweep the servo (& sensor) from 144-degrees left to 36-degrees right at 18-degree intervals.
//  {
//    myservo.write(pos);  // tell servo to go to position in variable 'pos'
//    delay(90); // wait 90ms for servo to get to position
//    checkForward(); // check the Obstacle avoiding Robot is still moving forward
//    curDist = readPing(); // get the current distance to any object in front of sensor
//    if (curDist < COLL_DIST) { // if the current distance to object is less than the collision distance
//      checkCourse(); // run the checkCourse function
//      break; // jump out of this loop
//    }
//    if (curDist < TURN_DIST) { // if current distance is less than the turn distance
//      changeRoute(); // run the changeRoute function
//    }
//    if (curDist > curDist) {
//      maxAngle = pos;
//    }
//    if (pos > 90 && curDist > curLeft) {
//      curLeft = curDist;
//    }
//    if (pos == 90 && curDist > curFront) {
//      curFront = curDist;
//    }
//    if (pos < 90 && curDist > curRight) {
//      curRight = curDist;
//    }
//  }
//  maxLeft = curLeft;
//  maxRight = curRight;
//  maxFront = curFront;
//}
//
//void setCourse() { // set direction for travel based on a very basic distance map, simply which direction has the greatest distance to and object - turning right or left?
//  if (maxAngle < 90) {
//    turnRight();
//  }
//  if (maxAngle > 90) {
//    turnLeft();
//  }
//  maxLeft = 0;
//  maxRight = 0;
//  maxFront = 0;
//}


