/* ========================================
 *
 * Cypress Robot Kit
 * Written by Alvin Wu and Todd Nguyen
 * University of Illinois at Urbana-Champaign
 * Free for distribution under General Public License
 * 
 * ========================================
*/

#ifndef BLUETOOTH_H
#define BLUETOOTH_H
    
    #include <project.h>
    
    void BleCallBack(uint32 event, void* eventParam);
    void clearMotors();    
    void updateUltrasonic(int16* value);
    void updateIR1(int16* value);
    void updateIR2(int16* value);

    void updateLed();

#endif //BLUETOOTH_H


/* [] END OF FILE */
