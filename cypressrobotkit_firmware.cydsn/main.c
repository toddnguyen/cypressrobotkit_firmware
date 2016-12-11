/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include "motors.h"
#include "bluetooth.h"
#include "state.h"

/*************************************************
    Constants
*************************************************/
#define THRESH 2000

/*************************************************
    Global Variables
*************************************************/
    
int t = 0;
uint16 dist = 0;
uint8 interruptState;
int turn_count = 0;
uint8 ledstate = 0;

/*************************************************
    Interrupt Handlers
*************************************************/

/*Ultrasonic interrupt handler*/
CY_ISR(Timer_1_ISR_handler){
    //Read timer
    t = Timer_1_ReadCounter();

    //Convert time to distance (cm)
    dist = t/58;

    //Clear interrupt
    Timer_1_ClearInterrupt(Timer_1_INTR_MASK_CC_MATCH);
}

/*Obstacle Avoidance interrupt handler*/
CY_ISR(Timer_2_ISR_handler){
    turn_count++;
}

/*************************************************
    Helper Functions
*************************************************/

/*Classify color from IR sensors*/
uint8 get_color(int16 color){
    if(color>THRESH){
        return 1;
    }else{
        return 0;
    }
}

/*************************************************
    Main Loop
*************************************************/
int main()
{
    /*Sensor Variables*/
    int16 colorR_raw = 0;
    int16 colorL_raw = 0;
    uint8 colorR;
    uint8 colorL;
    uint8 turning = 0;

    /*Motor Variables*/
    uint8 old_direction_code = 0;
    uint8 old_speed_code = 0;
    uint8 direction_code = 0;
    uint8 speed_code = 0;
    
    /* Enable interrupts. */
    CyGlobalIntEnable; 
    isr_1_StartEx(Timer_1_ISR_handler);
    isr_2_StartEx(Timer_2_ISR_handler);
   
    /*Start BLE*/   
    CyBle_Start(BleCallBack);

    /*Start timer for ultrasonic sensor*/
    Timer_1_Start();
    
    /*Start ADC*/
    IR_ADC_Start();
    IR_ADC_StartConvert();
    
    /*Start LCD*/
    LCD_display_Start();
    LCD_display_ClearDisplay();
    LCD_display_PrintString("DIST: ");

    /*Stop motors and enable PWM*/
    motor_stop();
    right_speedcontroller_Start();
    left_speedcontroller_Start();

    for(;;)
    {
        /*Read IR sensors*/
        colorR_raw = IR_ADC_GetResult16(1);
        colorL_raw = IR_ADC_GetResult16(0);
        colorR = get_color(colorR_raw);
        colorL = get_color(colorL_raw);

        /*Read ultrasonic sensor*/
        TRIGGER_pin_Write(1);
        CyDelay(10);
        TRIGGER_pin_Write(0);

        /*Update sensor readings for BLE*/
        updateUltrasonic((int16*) &dist);
        updateIR1((int16*) &colorR_raw);
        updateIR2((int16*) &colorL_raw); 
       
        /*Process BLE events*/
        interruptState = CyEnterCriticalSection();
        CyBle_ProcessEvents();
        CyExitCriticalSection(interruptState);

        switch(get_robot_mode()){

            case manual: 
                if(turning){
                    turning = 0;
                    Timer_2_Stop();
                }

                direction_code = get_direction_code();
                speed_code = get_speed_code();
                if(old_direction_code != direction_code || old_speed_code != speed_code){
                    handleMotors(direction_code,speed_code);
                    old_direction_code = direction_code;
                    old_speed_code = speed_code;
                }
                break;

            case obstacle_avoidance:
                if(!turning){
                    setspeed(15);
                    move_fwd();
                    if(dist<15){
                        turn_count = 0;
                        Timer_2_Start();
                        turning = 1;
                    }
                }else{
                    if(turn_count >= 1){
                        Timer_2_Stop();
                        turning = 0;
                    }
                    else{
                        //turn
                        setspeed(15);
                        rotate_right();
                    }
                }
                break;

            case line_following:
                if(turning){
                    turning = 0;
                    Timer_2_Stop();
                }
                setspeed(7);
                if(colorR && !colorL){
                    rotate_right();
                }
                else if(!colorR && colorL){
                    rotate_left();
                }
                else if(colorR && colorL){
                    motor_stop();
                }
                else{
                    move_fwd();
                }
                break;

            default:
                break;
        }
    }
}

/* [] END OF FILE */
