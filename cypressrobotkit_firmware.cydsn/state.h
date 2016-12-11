/* ========================================
 *
 * Cypress Robot Kit
 * Written by Alvin Wu and Todd Nguyen
 * University of Illinois at Urbana-Champaign
 * Free for distribution under General Public License
 * 
 * ========================================
*/

#ifndef STATE_H
#define STATE_H

#include <project.h>
    
/* Enum for robot control mode */
typedef enum{
   manual = 0,
   obstacle_avoidance,
   line_following,      
}Mode;

/* Sets robot control mode */
void set_robot_mode(Mode mode);

/* Sets state for current movement direction */
void set_direction_code(uint8 dir);

/* Sets state current speed */
void set_speed_code(uint8 spd);

/* Returns current robot mode */
Mode get_robot_mode();

/* Returns current robot movement direction */
uint8 get_direction_code();

/* Returns current robot speed */
uint8 get_speed_code();

#endif //STATE_H

/* [] END OF FILE */
