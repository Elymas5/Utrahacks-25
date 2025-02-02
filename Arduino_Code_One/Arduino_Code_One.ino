/*
//#include <NewPing.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo

#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 50 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

//Motors
#define ENA 11   // Enable pin for Motor A
#define IN1 10   // Control pin 1 for Motor A
#define IN2 9   // Control pin 2 for Motor A
#define ENB 3   // Enable pin for Motor B
#define IN3 6   // Control pin 1 for Motor B
#define IN4 5   // Control pin 2 for Motor B

// TCS230 or TCS3200 pins wiring to Arduino
#define S0 4
#define S1 A4
#define S2 A3
#define S3 7
#define sensorOut A5

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

int SERVO_PIN = 8; //Servo Pin
int RED_PIN =  2; //Red Light Pin

int distance;
bool isDetecting;
int clawPosition;
int pos = 0;
int counter = 0;
int colorOne = 0;
int colorTwo = 1;

//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.



const int SAMPLES = 5;          // Number of samples for averaging
const int COLOR_THRESHOLD = 50; // Minimum frequency difference to claim detection

// Calibration values (adjust based on your sensor tests)
const int RED_MIN = 50, RED_MAX = 120;
const int GREEN_MIN = 80, GREEN_MAX = 150;
const int BLUE_MIN = 100, BLUE_MAX = 200;

// ... (servo and sensor objects remain the same)

// Modified getColorFrequency with averaging
int getColorFrequency(int s2State, int s3State) {
    digitalWrite(S2, s2State);
    digitalWrite(S3, s3State);
    delay(10); // Let sensor settle
    
    int sum = 0;
    for(int i = 0; i < SAMPLES; i++) {
        sum += pulseIn(sensorOut, LOW);
        delay(5);
    }
    return sum / SAMPLES;
}

// Function to detect the dominant color
int detectColor() {
    int red = getColorFrequency(LOW, LOW);
    int blue = getColorFrequency(LOW, HIGH);
    int green = getColorFrequency(HIGH, HIGH);

    // Normalize using calibration ranges
    int redIntensity = map(red, RED_MIN, RED_MAX, 255, 0);
    int greenIntensity = map(green, GREEN_MIN, GREEN_MAX, 255, 0);
    int blueIntensity = map(blue, BLUE_MIN, BLUE_MAX, 255, 0)-100;

    Serial.print("R:");Serial.print(redIntensity);Serial.print(" G:");Serial.print(greenIntensity);Serial.print(" B:");Serial.print(blueIntensity);

    // Determine dominant color with threshold
    if(abs(redIntensity - greenIntensity) < COLOR_THRESHOLD && 
       abs(redIntensity - (blueIntensity)) < COLOR_THRESHOLD) {
        return 0;
    }
    
    if(redIntensity > greenIntensity && redIntensity > blueIntensity) {
        return 1; //Red
    } else if(greenIntensity > blueIntensity) {
        return 2; //Green
    } else if(redIntensity < 0 && greenIntensity < 0 && blueIntensity < 0){
        return 0; //Null
    }else{
        return 3; //Blue
    }
}

void clawOn(bool state){
  if(state==false){
    myservo.write(130);
    delay(1000);
    myservo.write(0);
  }else{
    myservo.write(-130);
    delay(1000);
    myservo.write(0);
  }
}

int initialSearch(){
  turnLeft(1200);
  moveForward(2000);
  int detectedColor = detectColor();
  if(detectedColor!=0){
    return detectedColor;
  }else{
    turnAround();
    moveForward(3000);
  }
}


void setup() {
  delay(5000);
  // put your setup code here, to run once:
  Serial.begin(9600);
  //myservo.write(90);
  myservo.attach(SERVO_PIN);
  pinMode(RED_PIN, OUTPUT);

  //Color Sensors
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  int colorOne = 0;
  int colorTwo = initialSearch(); //Get on to circle and record color
  delay(1000);
}

void loop() {
  int detectedColor = detectColor();
  Serial.println("Detected Color: " + detectedColor);
  if(detectedColor==colorTwo){
    moveForward(300);
  }else if(detectedColor==colorOne){
    turnLeft(500);
    moveForward(300);
  }else{
    colorOne = colorTwo;
    colorTwo = detectedColor;
    counter++;
    if(counter==5){
      myservo.write(130);
      delay(500);
      myservo.write(-130);
      delay(500);
      myservo.write(-90);
      delay(1000);
      myservo.write(0);
      delay(99999);
    }
  }

  delay(500);
}














void turnRight(int lag) {
   digitalWrite(IN1, HIGH);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, HIGH);
   analogWrite(ENA, 150); // Speed control (0-255)
   analogWrite(ENB, 150);
   delay(lag); // Adjust for 90-degree turn
   stopMotors();
}

void turnLeft(int lag) {
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN4, LOW);
   analogWrite(ENA, 150); // Speed control (0-255)
   analogWrite(ENB, 150);
   delay(lag); // Adjust for 90-degree turn
   stopMotors();
}

void turnAround() {
   digitalWrite(IN1, HIGH);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, HIGH);
   analogWrite(ENA, 150); // Speed control (0-255)
   analogWrite(ENB, 150);
   delay(4000); // Adjust for 180-degree turn
   stopMotors();
}

void moveForward(int lag) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 200); // Speed control (0-255)
    analogWrite(ENB, 200);
    delay(lag);
    stopMotors();
}

void moveBackward(int lag) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, 200);
    analogWrite(ENB, 200);
    delay(lag);
    stopMotors();
}

void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}*/





//#include <NewPing.h>
#include <Servo.h>




Servo myservo;  // create servo object to control a servo




#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 50 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.




//Motors
#define ENA 11   // Enable pin for Motor A
#define IN1 10   // Control pin 1 for Motor A
#define IN2 9   // Control pin 2 for Motor A
#define ENB 3   // Enable pin for Motor B
#define IN3 6   // Control pin 1 for Motor B
#define IN4 5   // Control pin 2 for Motor B




// TCS230 or TCS3200 pins wiring to Arduino
#define S0 4
#define S1 A4
#define S2 A3
#define S3 7
#define sensorOut A5




int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;




int SERVO_PIN = 8; //Servo Pin
int RED_PIN =  2; //Red Light Pin




int distance;
bool isDetecting;
int clawPosition;
int pos = 0;




//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.












const int SAMPLES = 5;          // Number of samples for averaging
const int COLOR_THRESHOLD = 50; // Minimum frequency difference to claim detection




// Calibration values (adjust based on your sensor tests)
const int RED_MIN = 50, RED_MAX = 120;
const int GREEN_MIN = 80, GREEN_MAX = 150;
const int BLUE_MIN = 100, BLUE_MAX = 200;




// ... (servo and sensor objects remain the same)




// Modified getColorFrequency with averaging
int getColorFrequency(int s2State, int s3State) {
   digitalWrite(S2, s2State);
   digitalWrite(S3, s3State);
   delay(10); // Let sensor settle
 
   int sum = 0;
   for(int i = 0; i < SAMPLES; i++) {
       sum += pulseIn(sensorOut, LOW);
       delay(5);
   }
   return sum / SAMPLES;
}


// Function to detect the dominant color
String detectColor() {
   int red = getColorFrequency(LOW, LOW);
   int blue = getColorFrequency(LOW, HIGH);
   int green = getColorFrequency(HIGH, HIGH);




   // Normalize using calibration ranges
   int redIntensity = map(red, RED_MIN, RED_MAX, 255, 0);
   int greenIntensity = map(green, GREEN_MIN, GREEN_MAX, 255, 0);
   int blueIntensity = map(blue, BLUE_MIN, BLUE_MAX, 255, 0)-100;




   // Serial.print("R:");Serial.print(redIntensity);Serial.print(" G:");Serial.print(greenIntensity);Serial.print(" B:");Serial.print(blueIntensity);




   // Determine dominant color with threshold
   if(abs(redIntensity - greenIntensity) < COLOR_THRESHOLD &&
      abs(redIntensity - (blueIntensity)) < COLOR_THRESHOLD) {
       return "Null";
   }
 
   if(redIntensity > greenIntensity && redIntensity > blueIntensity) {
       return "Red";
   } else if(greenIntensity > blueIntensity) {
       return "Green";
   } else if(redIntensity < 0 && greenIntensity < 0 && blueIntensity < 0){
       return "Null";
   }else{
       return "Blue";
   }
}




void clawOn(bool state){
 if(state==false){
   myservo.write(130);
   delay(1000);
   myservo.write(0);
 }else{
   myservo.write(-130);
   delay(1000);
   myservo.write(0);
 }
}








void setup() {
 // put your setup code here, to run once:
 Serial.begin(9600);
 myservo.write(90);
 myservo.attach(SERVO_PIN);
 pinMode(RED_PIN, OUTPUT);




 //Color Sensors
 pinMode(S0, OUTPUT);
 pinMode(S1, OUTPUT);
 pinMode(S2, OUTPUT);
 pinMode(S3, OUTPUT);
 pinMode(sensorOut, INPUT);
 digitalWrite(S0, HIGH);
 digitalWrite(S1, HIGH);




 pinMode(ENA, OUTPUT);
 pinMode(IN1, OUTPUT);
 pinMode(IN2, OUTPUT);
 pinMode(ENB, OUTPUT);
 pinMode(IN3, OUTPUT);
 pinMode(IN4, OUTPUT);




 // clawOn(true);
}


int currentColor = -1;
int lastColor = -1;
int ringCount = 0;


int getColorIndex(String color) {
   if (color == "Red") return 0;
   if (color == "Green") return 1;
   if (color == "Blue") return 2;
   return -1;
}


void loop() {
 // put your main code here, to run repeatedly:
 String detectedColor = detectColor();
 int detectedIndex = getColorIndex(detectedColor);
 Serial.println("."+ringCount);
 Serial.println("Detected Color: " + detectedColor);
 if (detectedIndex == -1) {
       Serial.println("No valid color detected.");
       return; // Ignore if no valid color
     }
// If first color, initialize as 0
   if (currentColor == -1) {
       currentColor = detectedIndex;
       lastColor = -1;
       Serial.println("forward Initializing with color: " + detectedColor + detectedIndex);
       moveForward();
       return;
   }


   // Check the expected next color


   if (detectedIndex != currentColor) {
       Serial.println(" forward." + detectedColor + detectedIndex);
       ringCount++;
       lastColor = currentColor;
       currentColor = detectedIndex;
       moveForward();
       moveForward();
       delay(250);


   } else if (detectedIndex == currentColor) {
       Serial.println("Turning left." + detectedColor + detectedIndex);
       turnLeft();
       moveForward();
       moveForward();
       delay(200);
      if (ringCount == 5){
        myservo.write(130);
        delay(500);
        myservo.write(-130);
        delay(500);
        myservo.write(-90);
        delay(1000);
        myservo.write(0);
        delay(99999);
      }
      
       // Re-check color after turn
       detectedColor = detectColor();
       detectedIndex = getColorIndex(detectedColor);
       if (detectedIndex == currentColor) {
           Serial.println(" Turning around."+ detectedColor + detectedIndex);
           delay(50);
           turnLeft();
           moveForward();
         moveForward();
         moveForward();
           delay(750);


           moveForward();




       }
   } else {
       Serial.println("Unexpected color. Adjusting.");
       ringCount = max(0, ringCount - 1);
       turnLeft();
               moveForward();


   }




}
































void goTillWall(){
 while(true){
   distance = getDistance();
   if(0<distance && distance<20){
     digitalWrite(RED_PIN, HIGH);
     isDetecting = true;
     stopMotors();
     return;
   }else{
     digitalWrite(RED_PIN, LOW);
     isDetecting = false;
     moveForward();
   }
 }
}








































void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 150); // Speed control (0-255)
  analogWrite(ENB, 150);
  delay(2000); // Adjust for 90-degree turn
  stopMotors();
}




void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 100); // Speed control (0-255)
  analogWrite(ENB, 100);
  delay(2000); // Adjust for 90-degree turn
  stopMotors();
}




void turnAround() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 100); // Speed control (0-255)
  analogWrite(ENB, 100);
  delay(4000); // Adjust for 180-degree turn
  stopMotors();
}




void moveForward() {
   digitalWrite(IN1, HIGH);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, HIGH);
   digitalWrite(IN4, LOW);
   analogWrite(ENA, 190); // Speed control (0-255)
   analogWrite(ENB, 190);
}




void moveBackward() {
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, HIGH);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, HIGH);
   analogWrite(ENA, 200);
   analogWrite(ENB, 200);
}




void stopMotors() {
   digitalWrite(IN1, LOW);
   digitalWrite(IN2, LOW);
   digitalWrite(IN3, LOW);
   digitalWrite(IN4, LOW);
   analogWrite(ENA, 0);
   analogWrite(ENB, 0);
}




long getDistance() {
   digitalWrite(TRIGGER_PIN, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIGGER_PIN, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIGGER_PIN, LOW);
   long duration = pulseIn(ECHO_PIN, HIGH);
   return duration * 0.034 / 2;
}



























