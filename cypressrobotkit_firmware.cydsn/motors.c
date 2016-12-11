/* ========================================
 *
 * Cypress Robot Kit
 * Written by Alvin Wu and Todd Nguyen
 * University of Illinois at Urbana-Champaign
 * Free for distribution under General Public License
 * 
 * ========================================
*/

#include <project.h>
#include "motors.h"

/*************************************************
    Function to handle motor commands from BLE
*************************************************/
void handleMotors(uint8 motorcontrol, uint8 motorspeeds){

    uint8 rightMotorSpeed = motorspeeds & 0xF;
    uint8 leftMotorSpeed = (motorspeeds >> 4) & 0xF;
    
    uint8 rightMotorDirection = motorcontrol & 1;
    uint8 leftMotorDirection = (motorcontrol >> 1) & 1;
    
    //Stop
    if(leftMotorSpeed == 0 && rightMotorSpeed == 0){
        motor_stop();
    }else{        
        leftmotor_setspeed(leftMotorSpeed);
        rightmotor_setspeed(rightMotorSpeed);
        leftMotorDirection ? leftmotor_fwd() : leftmotor_rev();
        rightMotorDirection ? rightmotor_fwd() : rightmotor_rev();
    }
}


/*************************************************
    Functions to set motor direction registers
*************************************************/
void move_fwd(){
    leftmotor_fwd();
    rightmotor_fwd();
}

void move_rev(){
    leftmotor_rev();
    rightmotor_rev();
}

void rotate_left(){
    leftmotor_rev();
    rightmotor_fwd();
}

void rotate_right(){
    leftmotor_fwd();
    rightmotor_rev();
}

void motor_stop(){
    left_control_Write(0);
    right_control_Write(0);
    leftmotor_setspeed(0);
    rightmotor_setspeed(0);
}


/*************************************************
    Functions to control individual motors
*************************************************/
void leftmotor_fwd(){
    left_control_Write(1);
}

void leftmotor_rev(){
    left_control_Write(3);
}

void rightmotor_fwd(){
    right_control_Write(1);
}

void rightmotor_rev(){
    right_control_Write(3);
}

void setspeed(uint8 speed){
    leftmotor_setspeed(speed);
    rightmotor_setspeed(speed);
}

void leftmotor_setspeed(uint8 speed){
    if(speed == 0){
        left_speedcontroller_WriteCompare(0);
    } else {
        int setspeed = (int) (speed + 8);
        left_speedcontroller_WriteCompare(setspeed);
    }
}

void rightmotor_setspeed(uint8 speed){
    if(speed == 0){
        right_speedcontroller_WriteCompare(0);
    } else {
        int setspeed = (int) (speed + 8);
        right_speedcontroller_WriteCompare(setspeed);
    }
}
    
/* [] END OF FILE */
