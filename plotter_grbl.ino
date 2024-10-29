

// libraries for motors
#include <AccelStepper.h>
#include <Servo.h>

// Function prototypes
void autoHome();
void penUp();
void penDown();
float extractCoordinate(String gcode, char axis);
void moveToCoordinates(float x_mm, float y_mm);
void processGCode(String gcode);

//pinouts
  //stepper motors
  #define X_STEP_PIN 3
  #define X_DIR_PIN 2
  #define Y_STEP_PIN 10
  #define Y_DIR_PIN 9
  //servo motor
  #define SERVO_PIN 4 /
  //limit switches
  #define X_LIMIT_PIN 11  //tbd
  #define Y_LIMIT_PIN 12 //tbd

//objects creation for motors
  //stepper motors
  AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
  AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
  //servo motor
  Servo penServo;

//stpes needed to plot one millimeter on page
  int stepsPerMM_X = 100;  //tbc (to be calculated)
  int stepsPerMM_Y = 100;  //tbc

//pen positions
  int penUpPos = 90;   //turns pen up
  int penDownPos = 0;  //turns pen down

//initialization
void setup() {
  //setting up baud rate for communicating with Universal G-Code Sender
  Serial.begin(115200);

  //setting up servo and setting up inital position
  penServo.attach(SERVO_PIN);
  penUp();  // function that sets pin to 90 degree

  //max speed and and acceleration rate for steppers
    // max spped allows us to control a limt for motor speed 
    stepperX.setMaxSpeed(500);       // Set maximum speed for X-axis
    stepperY.setMaxSpeed(500);       // Set maximum speed for Y-axis

    //sets acceleration so that motor doesnt get high spped in a second and avoids 
    //sudden movemnts thus adds to precise plotting 
    stepperX.setAcceleration(200);    // Set acceleration for X-axis
    stepperY.setAcceleration(200);    // Set acceleration for Y-axis  

  //pin modes of limit switches 
    pinMode(X_LIMIT_PIN, INPUT_PULLUP);
    pinMode(Y_LIMIT_PIN, INPUT_PULLUP);

  //fuction that calibrates homing position 
    autoHome();
}

//loop - this runs constantluy in loop the entire time - this is how thw actaula working starts
void loop()
{
 //checks if any data is coming in serial form
  if (Serial.available() > 0) 
  {
    String gcode = Serial.readStringUntil('\n');  // reads one line at at time
    processGCode(gcode);  // Process the G-code function
  }

// Auto-homing function to move to the (0,0) position using limit switches
void autoHome() {
  // Move towards the X-axis limit switch
  stepperX.setSpeed(-200);  // Move in the negative direction towards the limit
  while (digitalRead(X_LIMIT_PIN) == HIGH) 
  {  // Run until the switch is pressed (LOW)
    stepperX.runSpeed();
  }
  stepperX.setCurrentPosition(0);  // Set X-axis current position to 0 (home)
  
  // Move towards the Y-axis limit switch
  stepperY.setSpeed(-200);  // Move in the negative direction towards the limit
  while (digitalRead(Y_LIMIT_PIN) == HIGH) 
  {  // Run until the switch is pressed (LOW)
    stepperY.runSpeed();
  }
  stepperY.setCurrentPosition(0);  // Set Y-axis current position to 0 (home)

  //setCurrentPosition is another built in function that sets the hime postion using data from limit switches


// function to process the incoming G-code
void processGCode(String gcode) 
{
  gcode.trim();  //fileters leading or trailing whitespace characters in line that is being taken
  //G0: positioning
  //G1: controlled feed rate movement
  if (gcode.startsWith("G0") || gcode.startsWith("G1")) 
  {
    float x = extractCoordinate(gcode, 'X'); //fuction to find cordinates
    float y = extractCoordinate(gcode, 'Y'); //function to find cordinates
   
    moveToCoordinates(x, y); // calls move to function that forms exact target cordinate for the plotter to move
  }

  // handle Z-axis commands (used for pen up/down via the servo)
  if (gcode.indexOf('Z') != -1) // becuse the generated z code will have dept of -1 as max dept
  { 
    float z = extractCoordinate(gcode, 'Z'); //function to find cordinates
    if (z > 0) 
    {
      penUp();  // Z > 0 means pen up
    } else 
    {
      penDown();  // Z <= 0 means pen down
    }
  
  }
}

// extract the X, Y, or Z coordinate from the G-code command
// function aims to extract the numerical coordinate associated 
// with the specified axis (X, Y, or Z) from the G-code string.
float extractCoordinate(String gcode, char axis) 
{
  int idx = gcode.indexOf(axis);  // Find the position of the mentioned axis letter 
  if (idx != -1) 
  {
    return gcode.substring(idx + 1).toFloat();
    //value is send to variables mentioned in above function (float x, y, z)
  }
  return NAN;  // Return NaN if the axis isn't found
}


// move the stepper motors to the specified X and Y coordinates
// forms exact target cordinate for the plotter to move
void moveToCoordinates(float x_mm, float y_mm) // takes in x,y value from extracted g code line
{
  long targetX = x_mm * stepsPerMM_X;  // Convert X coordinate to steps
  long targetY = y_mm * stepsPerMM_Y;  // Convert Y coordinate to steps

  stepperX.moveTo(targetX);  // Move the X stepper motor to the target position (built in function of library)
  stepperY.moveTo(targetY);  // Move the Y stepper motor to the target position (built in function of library)

  // The distanceToGo() method returns the number of steps remaining to reach the target position.
  // If either motor still has steps to take, the loop will run.
  // this again is a built in function of libarary
  while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0) 
  {
    stepperX.run();
    stepperY.run();
  }
}

// raise the pen using the servo
void penUp() 
{
  penServo.write(penUpPos);  // Move the servo to the "pen up" position
}

// lower the pen using the servo
void penDown() 
{
  penServo.write(penDownPos);  // Move the servo to the "pen down" position
}



