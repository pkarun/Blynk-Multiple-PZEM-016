/////////////////////////////////////////////////////////////////
//                           Settings                          //
/////////////////////////////////////////////////////////////////

#define BUILD_NUMBER          01                    // Build number is to know latest code is running or not when we do FOTA update

/***************************************************
          NodeMCU Pin Assignment
 **************************************************/

#define RX_PIN_NODEMCU        14                  // D5 or GPIO 14 || RX or GPIO 03 -->  Nodemcu pin, used has RX pin
#define TX_PIN_NODEMCU        12                  // D6 or GPIO 12 || TX or GPIO 01 -->  Nodemcu pin, used has TX pin

#define MAX485_DE             5                   // D1 or GPIO 5
#define MAX485_RE_NEG         4                   // D2 or GPIO 4

/* Relay to NodeMCU Connection */

#define RELAY_PIN_1           2                   // D4 or GPIO 2
#define RELAY_PIN_2           16                  // D0 or GPIO 16

/* NodeMCU to Push Button switch  */

#define PUSH_BUTTON_1         13                  // D7 or GPIO 13
#define PUSH_BUTTON_2         0                   // D3 or GPIO 0

/***************************************************
          PZEM address value assignment
 **************************************************/

#define PZEM_SLAVE_1_ADDRESS     1                       // Make sure you assign address to pzem first before you use
#define PZEM_SLAVE_2_ADDRESS     2
#define PZEM_SLAVE_3_ADDRESS     3

/***************************************************
How often you would like to call function? Enter time in milliseconds  
 **************************************************/

#define GET_PZEM_DATA_TIME          500L                         // How often you wish to get PZEM data. Enter time in milliseconds. Keep 500L minimum or else Blynk disconnect frequently
#define SEND_TO_BLYNK_TIME          1000L                        // How often you would like to send data to blynk app. Enter time in milliseconds  
#define AUTO_MODE_TIME              20000L
#define PHYSICAL_BUTTON_TIME        300L

/***************************************************
          variable value assignment
 **************************************************/

#define LOW_VOLTAGE_1_CUTOFF         170                           // Low Voltage cut off value. If voltage is less than this value, then it switch off the relay 
#define LOW_VOLTAGE_2_CUTOFF         170
#define LOW_VOLTAGE_3_CUTOFF         170

#define HIGH_VOLTAGE_1_CUTOFF        300                           // High Voltage cut off value. If voltage is less than this value, then it switch off the relay 
#define HIGH_VOLTAGE_2_CUTOFF        300
#define HIGH_VOLTAGE_3_CUTOFF        300

/***************************************************
          Blynk Virtual Pin Assignment
 **************************************************/

/* Blynk button vpin */

#define VPIN_BUTTON_1                 V1
#define VPIN_BUTTON_2                 V2

#define VPIN_AUTO_MODE_BUTTON_1       V5                     // This button is to switch on motor in auto mode
#define VPIN_FIRMWARE_UPDATE          V6
#define VPIN_UPDATE_LED               V8
#define VPIN_BUILD_NUMBER             V9

#define VPIN_PHASE_FAIL_NOTIFICATION  V50                    // Phase failure alert notification button on/off
#define VPIN_LOW_V_NOTIFICATION       V51                    // Low voltage alert notification button on/off

/* Blynk Labeled Value vpin */

#define vPIN_VOLTAGE_1                V11
#define vPIN_CURRENT_USAGE_1          V12
#define vPIN_ACTIVE_POWER_1           V13
#define vPIN_ACTIVE_ENERGY_1          V14
#define vPIN_FREQUENCY_1              V15
#define vPIN_POWER_FACTOR_1           V16

#define vPIN_VOLTAGE_2                V21
#define vPIN_CURRENT_USAGE_2          V22
#define vPIN_ACTIVE_POWER_2           V23
#define vPIN_ACTIVE_ENERGY_2          V24
#define vPIN_FREQUENCY_2              V25
#define vPIN_POWER_FACTOR_2           V26

#define vPIN_VOLTAGE_3                V31
#define vPIN_CURRENT_USAGE_3          V32
#define vPIN_ACTIVE_POWER_3           V33
#define vPIN_ACTIVE_ENERGY_3          V34
#define vPIN_FREQUENCY_3              V35
#define vPIN_POWER_FACTOR_3           V36

#define vPIN_SUM_VOLTAGE              V41
#define vPIN_SUM_CURRENT_USAGE        V42
#define vPIN_SUM_ACTIVE_POWER         V43
#define vPIN_SUM_ACTIVE_ENERGY        V44
#define vPIN_SUM_FREQUENCY            V45
#define vPIN_SUM_POWER_FACTOR         V46



 
