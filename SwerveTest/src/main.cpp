#include "vex.h"
#include <math.h>

using namespace vex;

controller Controller1 = controller(primary);
brain  Brain;

//motors are notated by position.  For example, BL = back left
motor DriveBL = motor(PORT5, ratio6_1, true);
motor DriveFL = motor(PORT6, ratio6_1, true);
motor DriveBR = motor(PORT7, ratio6_1, true);
motor DriveFR = motor(PORT8, ratio6_1, true);

motor TurnBL = motor(PORT1, ratio6_1, true);
motor TurnFL = motor(PORT2, ratio6_1, true);
//port 3 is broken
//sad
motor TurnBR = motor(PORT14, ratio6_1, true);
motor TurnFR = motor(PORT4, ratio6_1, true);

motor_group DriveTrain = motor_group(DriveBL, DriveFL, DriveBR, DriveFR);
motor_group TurnTrain = motor_group(TurnBL, TurnFL, TurnBR, TurnFR);

rotation RotationBL = rotation(PORT9, false);
rotation RotationFL = rotation(PORT10, false);
rotation RotationBR = rotation(PORT11, false);
//port 12 is broken
//sad
rotation RotationFR = rotation(PORT15, false);

int rc_auto_loop_function_Controller1();

competition Competition;

float targetDirection;
float rotationOffset;
float avgDif;

void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  RotationBL.setPosition(0, rev);
  RotationFL.setPosition(0, rev);
  RotationBR.setPosition(0, rev);
  RotationFR.setPosition(0, rev);
  Brain.Screen.print(color::cyan);
  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

void autonomous(void) {
  Brain.Screen.clearScreen(color::green);

  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

void correctDrive(motor currentMotor, rotation currentRot){
  float currentDirection = fmod((currentRot.position(rev)/7 * 3) * 360,360);
  float difDirection = (currentDirection - targetDirection);
  if(difDirection > 180){
    difDirection -= 360;
  } else if(difDirection < -180){
    difDirection += 360;
  }
  avgDif += fabs(difDirection);
  if(fabs(difDirection) > 1){
    currentMotor.setVelocity(fabs(difDirection), percent);
    if(difDirection > 0){
      currentMotor.spin(forward);
    } else {
      currentMotor.spin(reverse);
    }
  } else {
    currentMotor.stop();
  }
}

void usercontrol(void) {
  Brain.Screen.clearScreen(color::red);
  // User control code here, inside the loop
  while(1) {
    float xInput = Controller1.Axis3.position();
    float yInput = Controller1.Axis4.position();
    float magnitude = ((xInput/100) * (xInput/100) + (yInput/100) * (yInput/100)) * 100;
    //FIX THIS MATH ON SITE JONAH DO IT OR ELSE
    //actually it's a problem for later
    targetDirection = atan2(xInput,yInput)* 180.0 / 3.14159265 + 180;
    Brain.Screen.clearLine(0);
    Brain.Screen.print(RotationBR.position(rev));
    avgDif = 0;
    correctDrive(TurnBL, RotationBL);
    correctDrive(TurnFL, RotationFL);
    correctDrive(TurnBR, RotationBR);
    correctDrive(TurnFR, RotationFR);
    avgDif /= 4;
    if(magnitude > 10){
      //turn motors to face the joystick direction
      
      //move motors
      DriveTrain.setVelocity((magnitude - avgDif)/4, percent);
      //magnitude - avgDif is potentially problematic because the cutoff changes based on magnitude
      DriveTrain.spin(forward);
    } else {
      DriveTrain.stop();
      TurnTrain.stop();
    }
    wait(100, msec); // 1000 divided by wait duration = refresh rate of the code
                    // for example, 20 msec = 50hz refresh rate
  }
}

// Main will set up the competition functions and callbacks.

int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}