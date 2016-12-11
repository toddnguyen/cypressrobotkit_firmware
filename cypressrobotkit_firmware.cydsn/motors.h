/* ========================================
 *
 * Cypress Robot Kit
 * Written by Alvin Wu and Todd Nguyen
 * University of Illinois at Urbana-Champaign
 * Free for distribution under General Public License
 * 
 * ========================================
*/

#ifndef MOTORS_H
#define MOTORS_H
    
    #include <project.h>
    
    /* Handles motor comamands sent from BLE */
    void handleMotors(uint8 motorcontrol, uint8 motorspeeds);
        
    /* Sets control registers for forward movement */
    void move_fwd();
    
    /* Sets control registers for revrse movement */
    void move_rev();
    
    /* Sets control registers for left rotataion */
    void rotate_left();
    
    /* Sets control registers for right rotataion */
    void rotate_right();
    
    /* Sets control registers/PWM modules for halt */
    void motor_stop();

    /* Sets left control register for forward movement */
    void leftmotor_fwd();
    
    /* Sets left control register for reverse movement */
    void leftmotor_rev();

    /* Sets right control register for forward movement */
    void rightmotor_fwd();
    
    /* Sets right control register for reverse movement */
    void rightmotor_rev();

    /* Sets PWM module speed for both motors */
    void setspeed(uint8 speed);
    
    /* Sets PWM module speed for left motor */
    void leftmotor_setspeed(uint8 speed);
    
    /* Sets PWM module speed for right motor */
    void rightmotor_setspeed(uint8 speed);


#endif //MOTORS_H