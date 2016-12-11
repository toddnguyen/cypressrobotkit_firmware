/* ========================================
 *
 * Cypress Robot Kit
 * Written by Alvin Wu and Todd Nguyen
 * University of Illinois at Urbana-Champaign
 * Free for distribution under General Public License
 * 
 * ========================================
*/

#include "state.h"

static Mode robot_mode = manual;
static uint8 direction;
static uint8 speed;

/*************************************************
    Function to set robot state from BLE Commands
*************************************************/
void set_robot_mode(Mode mode){
    robot_mode = mode;   
}

void set_direction_code(uint8 dir){
	direction = dir & 0x3;
}

void set_speed_code(uint8 spd){
	speed = spd;
}

Mode get_robot_mode(){
	return robot_mode;
}


/*************************************************
    Function to get robot state
*************************************************/
uint8 get_direction_code(){
	return direction;
}

uint8 get_speed_code(){
	return speed;
}

/* [] END OF FILE */
