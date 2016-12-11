/* ========================================
 *
 * Cypress Robot Kit
 * Written by Alvin Wu and Todd Nguyen
 * University of Illinois at Urbana-Champaign
 * Free for distribution under General Public License
 * 
 * ========================================
*/

#include "bluetooth.h"
#include "motors.h"
#include "state.h"

int ultrasonicNotify;
int irNotify;
uint16 ultrasonicOld = 0xFFFF;
uint16 ir1Old = 0xFFFF;
uint16 ir2Old = 0xFFFF; 

/********************************************
    Function to handle BLE stack
********************************************/
void BleCallBack(uint32 event, void* eventParam){
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    
    switch(event){
        //If there is a disconnect or stack just turned on, start advertising, turn off motors.
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            ultrasonicNotify = 0;
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            pwm_Start();
            set_robot_mode(manual);
            motor_stop();
            clearMotors();
        break;
            
        //If a connection is made, update capsense and led states
        case CYBLE_EVT_GATT_CONNECT_IND:
            updateLed();
            updateUltrasonic(NULL);
            updateIR1(NULL);
            clearMotors();
            pwm_Stop();
            right_speedcontroller_Start();
            left_speedcontroller_Start();
        break;
            
        //Handle a write request
        case CYBLE_EVT_GATTS_WRITE_REQ:
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T*) eventParam;
            
            //Request write the LED value
            if(wrReqParam->handleValPair.attrHandle == CYBLE_ROBOTKIT_LED_CHAR_HANDLE){
                if(CYBLE_GATT_ERR_NONE == CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED)){
                    red_Write(!wrReqParam->handleValPair.value.val[0]);
                    CyBle_GattsWriteRsp(cyBle_connHandle);
                }
            }
            
            //Request to write Green LED Value
            if(wrReqParam->handleValPair.attrHandle == CYBLE_ROBOTKIT_LED2_CHAR_HANDLE){
                if(CYBLE_GATT_ERR_NONE == CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED)){
                    green_Write(!wrReqParam->handleValPair.value.val[0]);
                    CyBle_GattsWriteRsp(cyBle_connHandle);
                }
            }
            
            //Request to write Motor Value
            if(wrReqParam->handleValPair.attrHandle == CYBLE_ROBOTKIT_MOTOR_CHAR_HANDLE){
                if(CYBLE_GATT_ERR_NONE == CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED)){
                    uint8 mode = (wrReqParam->handleValPair.value.val[0] >> 2) & (0x3);
                    switch(mode){
                        case 0x0:
                            set_robot_mode(manual);
                            break;
                        case 0x1:
                            set_robot_mode(line_following);
                            break;
                        case 0x2:
                            set_robot_mode(obstacle_avoidance);
                            break;
                        default:
                            set_robot_mode(manual);
                    }
                    set_direction_code(wrReqParam->handleValPair.value.val[0]);
                    set_speed_code(wrReqParam->handleValPair.value.val[1]);
                    CyBle_GattsWriteRsp(cyBle_connHandle);
                }
            }
            
            //Request to update CapSense Notification
            if(wrReqParam->handleValPair.attrHandle == CYBLE_ROBOTKIT_ULTRASONIC_ULTRASONICCCCD_DESC_HANDLE){
                CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle,CYBLE_GATT_DB_PEER_INITIATED);
                ultrasonicNotify = wrReqParam->handleValPair.value.val[0] & 0x01;
                CyBle_GattsWriteRsp(cyBle_connHandle);
            }
            
            //Request to update IR1 Notification
            if(wrReqParam->handleValPair.attrHandle == CYBLE_ROBOTKIT_IR_PIN1_IR1CCCD_DESC_HANDLE){
                CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle,CYBLE_GATT_DB_PEER_INITIATED);
                irNotify = wrReqParam->handleValPair.value.val[0] & 0x01;
                CyBle_GattsWriteRsp(cyBle_connHandle);
            }
            
            //Request to update IR1 Notification
            if(wrReqParam->handleValPair.attrHandle == CYBLE_ROBOTKIT_IR_PIN2_IR2CCCD_DESC_HANDLE){
                CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle,CYBLE_GATT_DB_PEER_INITIATED);
                irNotify = wrReqParam->handleValPair.value.val[0] & 0x01;
                CyBle_GattsWriteRsp(cyBle_connHandle);
            }
        break;           
            
        default:
        break;
    }
}

/********************************************
    Function to update Motors in GATT database
********************************************/
void clearMotors(){
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED){
        return;
    }
    
    //Clear motor state in GATT Database
    uint16 motors_State = 0;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T tempHandle;
    tempHandle.attrHandle = CYBLE_ROBOTKIT_MOTOR_CHAR_HANDLE;
    tempHandle.value.val = (uint8*) &motors_State;
    tempHandle.value.len = 2; //two bytes
    
    //write to GAT database
    CyBle_GattsWriteAttributeValue(&tempHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
}


/********************************************
    Function to update Ultrasonic in GATT database
********************************************/
void updateUltrasonic(int16* value){
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED || value == NULL){
        return;
    }
    
    CYBLE_GATTS_HANDLE_VALUE_NTF_T tempHandle;
    tempHandle.attrHandle = CYBLE_ROBOTKIT_ULTRASONIC_CHAR_HANDLE;    
    tempHandle.value.val = (uint8*) value;
    tempHandle.value.len = 2; //2 bytes (16 bits)
    
    //write to GATT database
    CyBle_GattsWriteAttributeValue(&tempHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED); 
    
    //notify if position has changed
    if(ultrasonicNotify && (ultrasonicOld != *value)){
        CyBle_GattsNotification(cyBle_connHandle, &tempHandle);
        ultrasonicOld = *value;
    }
}

/********************************************
    Functions to update IR in GATT database
********************************************/
void updateIR1(int16* value){
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED){
        return;
    }
    
    if(value == NULL){
        return;
    }
    
    CYBLE_GATTS_HANDLE_VALUE_NTF_T tempHandle;

    tempHandle.attrHandle = CYBLE_ROBOTKIT_IR_PIN1_CHAR_HANDLE;    
    tempHandle.value.val = (uint8*) value;
    tempHandle.value.len = 2; //2 bytes (16 bits)
    
    //write to GATT database
    CyBle_GattsWriteAttributeValue(&tempHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED); 
    
    //notify if position has changed
    if(irNotify && (ir1Old != *value)){
        CyBle_GattsNotification(cyBle_connHandle, &tempHandle);
        ir1Old = *value;
    }
}

void updateIR2(int16* value){
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED){
        return;
    }
    
    if(value == NULL){
        return;
    }
    
    CYBLE_GATTS_HANDLE_VALUE_NTF_T tempHandle;

    tempHandle.attrHandle = CYBLE_ROBOTKIT_IR_PIN2_CHAR_HANDLE;    
    tempHandle.value.val = (uint8*) value;
    tempHandle.value.len = 2; //2 bytes (16 bits)
    
    //write to GATT database
    CyBle_GattsWriteAttributeValue(&tempHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED); 
    
    //notify if position has changed
    if(irNotify && (ir2Old != *value)){
        CyBle_GattsNotification(cyBle_connHandle, &tempHandle);
        ir2Old = *value;
    }
}


/********************************************
    Function to update LED in GAT database
********************************************/
void updateLed(){
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED){
        return;
    }
    
    CYBLE_GATTS_HANDLE_VALUE_NTF_T tempHandle;
    
    //Read red led state and reverse it (led is active low)
    uint8 red_State = !red_Read();
    
    tempHandle.attrHandle = CYBLE_ROBOTKIT_LED_CHAR_HANDLE;
    tempHandle.value.val = (uint8*) &red_State;
    tempHandle.value.len = 1; //one byte
    
    //write to GAT database
    CyBle_GattsWriteAttributeValue(&tempHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
    
    //Read green led state and reverse it (led is active low)
    uint8 green_State = !green_Read();
    
    tempHandle.attrHandle = CYBLE_ROBOTKIT_LED2_CHAR_HANDLE;
    tempHandle.value.val = (uint8*) &green_State;
    tempHandle.value.len = 1; //one byte
    
    //write to GAT database
    CyBle_GattsWriteAttributeValue(&tempHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
}




/* [] END OF FILE */
