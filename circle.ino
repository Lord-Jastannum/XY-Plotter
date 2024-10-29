#include <AccelStepper.h>
#include <Servo.h>

#define SERVO_PIN 4
// Create a Servo object
Servo penServo;

// Define motor connections and motor interface type
#define motorInterfaceType 1

// Define stepper motors
AccelStepper stepper1(motorInterfaceType, 10, 9);  // Step and direction pins for motor 1
AccelStepper stepper2(motorInterfaceType, 3, 2);  // Step and direction pins for motor 2

// Circle parameters
const float radius = 100;    // Radius of the circle in steps
const int stepsPerRevolution = 200; // Adjust based on your motor
const int numSteps = 360; // Number of steps to complete a circle
const float angleStep = 360.0 / numSteps; // Angle step in degrees
const int penUpPosition = 0;   
const int penDownPosition = 90;  

void setup() {
  // Set the maximum speed and acceleration for the stepper motors
  stepper1.setMaxSpeed(1000);
  stepper1.setAcceleration(100);
  
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(100);
  penServo.attach(SERVO_PIN);

  
  penServo.write(penUpPosition);
  delay(1000);
}

void loop() {
  penServo.write(penDownPosition);
  delay(2000); 
  for (int i = 0; i < numSteps; i++) {
    float angle = i * angleStep; // Calculate the current angle
    float x = radius * cos(radians(angle)); // X position
    float y = radius * sin(radians(angle)); // Y position

    // Convert the x and y positions to steps
    long step1 = x * (stepsPerRevolution / (2 * PI * radius)); // Steps for motor 1
    long step2 = y * (stepsPerRevolution / (2 * PI * radius)); // Steps for motor 2

    stepper1.moveTo(step1);
    stepper2.moveTo(step2);

    // Move motors to the calculated position
    while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0) {
      stepper1.run();
      stepper2.run();
    }

    // Small delay to slow down the drawing
    delay(10);
  }
   penServo.write(penUpPosition);
    delay(2000); 

  // Optionally, stop after one circle
  while (true);
}
