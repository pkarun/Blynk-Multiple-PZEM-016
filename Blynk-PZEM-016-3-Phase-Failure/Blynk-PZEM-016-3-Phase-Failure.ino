/* Name:    Blynk with Multiple Pzem 016 on NodeMCU
 * Source:  https://github.com/pkarun/Blynk-Multiple-PZEM-016
 * Wiring Connections:
 * 
 * PZEM 016 to NodeMCU Connection: 
 * 
 *    GND to GND
 *    5V to Vin
 * 
 * Connect 3 PZEM-016 to RS485 Connection: 
 * 
 *    A to A
 *    B to B
 * 
 *  RS-485 TTL to NodeMCU Connection:
 *  
 *    VCC to 3V
 *    GND to GND
 *    DI to D6/GPIO12
 *    DE to D1/GPIO5
 *    RE to D2/GPIO4
 *    RO to D5/GPIO14
 *  
 *  NodeMCU to 2 channel Relay Module
 *  
 *    D4 to Relay Pin 1
 *    D0 to Relay Pin 2
 *    Vin to VCC
 *    GND to GND
 *  
 */
 
//#define BLYNK_PRINT Serial           // Uncomment for debugging 
//#define BLYNK_DEBUG                  // Optional, this enables more detailed prints

#include "settings.h"           
//#include "secret.h"                   // <<--- UNCOMMENT this before you use and change values on config.h tab
#include "my_secret.h"                  // <<--- COMMENT-OUT or REMOVE this line before you use. This is my personal settings.

#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <ModbusMaster.h>
#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h> 

#include <SoftwareSerial.h>  //  ( NODEMCU ESP8266 )

SoftwareSerial pzemSerial(RX_PIN_NODEMCU, TX_PIN_NODEMCU, false,256);    //(RX,TX) NodeMCU connect to (TX,RX) of PZEM
 
ModbusMaster node1;
ModbusMaster node2;
ModbusMaster node3;

BlynkTimer timer;

double voltage_usage_1      = 0;
double current_usage_1      = 0;
double active_power_1       = 0;
double active_energy_1      = 0;
double frequency_1          = 0;
double power_factor_1       = 0;

double voltage_usage_2      = 0;
double current_usage_2      = 0;
double active_power_2       = 0;
double active_energy_2      = 0;
double frequency_2          = 0;
double power_factor_2       = 0;

double voltage_usage_3      = 0;
double current_usage_3      = 0;
double active_power_3       = 0;
double active_energy_3      = 0;
double frequency_3          = 0;
double power_factor_3       = 0;

double sum_of_voltage       = 0;
double sum_of_current       = 0;
double sum_of_power         = 0;
double sum_of_active_energy = 0;
double sum_of_frequency     = 0;
double sum_of_power_factor  = 0;


/* Relay */

int relay1State             = HIGH;                 // When power is back, keep relays in OFF condition HIGH is LOW in nodemcu
int pushButton1State        = HIGH;

int relay2State             = HIGH;
int pushButton2State        = HIGH;

int auto_mode_state_1       = HIGH;

int ReCnctFlag;               // Reconnection Flag
int ReCnctCount = 0;          // Reconnection counter

int firmwarestate = HIGH;     // Used for firmware update process

int underVoltageAlertOnOffState;         // Under voltage one time alert on off state variable
int phaseFailureAlertOnOffState;         // Phase failure voltage one time alert on off state variable
bool underVoltageAlertFlag = true;       // Under voltage alert flag set to true 

bool blynkConnectionStatusForNotification = false;
bool lowvoltagenotificationflag   = true;
bool phasefailurenotificationflag = true;

bool phasefailureflag   = false;
bool lowvoltageflag     = false;
bool highvoltageflag    = false;

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE,     1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE,     0);
}

void setup() {

  pinMode(MAX485_RE_NEG,  OUTPUT);
  pinMode(MAX485_DE,      OUTPUT);
  
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE,     0);

  Serial.begin(9600);
  pzemSerial.begin(9600);

/* start Modbus/RS-485 serial communication */
  node1.begin(PZEM_SLAVE_1_ADDRESS, pzemSerial);
  node2.begin(PZEM_SLAVE_2_ADDRESS, pzemSerial);
  node3.begin(PZEM_SLAVE_3_ADDRESS, pzemSerial);
  
  // Callbacks allow us to configure the RS485 transceiver correctly
  
  node1.preTransmission(preTransmission);
  node1.postTransmission(postTransmission);

  node2.preTransmission(preTransmission);
  node2.postTransmission(postTransmission);

  node3.preTransmission(preTransmission);
  node3.postTransmission(postTransmission);

  /*********************************************************************************************\
      Change PZEM address
  \*********************************************************************************************/

  /*
      changeAddress(OldAddress, Newaddress)
      By Uncomment the function in the below line you can change the slave address from one of the nodes (pzem device),
      only need to be done onces. Preverable do this only with 1 slave in the network.
      If you forgot or don't know the new address anymore, you can use the broadcast address 0XF8 as OldAddress to change the slave address.
      Use this with one slave ONLY in the network.
      This is the first step you have to do when connecting muliple pzem devices. If you haven't set the pzem address, then this program won't
      works.
     1. Connect only one PZEM device to nodemcu and powerup your PZEM
     2. uncomment the changeAddress function call below i.e., changeAddress(OldAddress, Newaddress)
     3. change the Newaddress value to some other value. Ex: 1, 2, 3 etc.,
     4. upload this program to nodemcu 
     5. if you see "Changing Slave Address" on serial monitor, then it successfully changed address 
     6. if you don't see that message, then click on RESET button on nodemcu
     7. Once this done you have successfully assigned address to pzem device.
     8. do the same steps for as many devices as you want. 
  */


 // changeAddress(0XF8, 3);                 // uncomment to set pzem address


  /* By Uncomment the function in the below line you can reset the energy counter (Wh) back to zero from one of the slaves.
  */

   //resetEnergy(0x01);
 
#if defined(USE_LOCAL_SERVER)
  WiFi.begin(WIFI_SSID, WIFI_PASS);         // Non-blocking if no WiFi available
  Blynk.config(AUTH, SERVER, PORT);
  Blynk.connect();
#else
  WiFi.begin(WIFI_SSID, WIFI_PASS);         // Non-blocking if no WiFi available
  Blynk.config(AUTH);
  Blynk.connect();
#endif   

  /*********************************************************************************************\
      RELAY code
  \*********************************************************************************************/

  pinMode(RELAY_PIN_1,   OUTPUT);  
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);  
  digitalWrite(RELAY_PIN_1, relay1State);
    
  pinMode(RELAY_PIN_2,   OUTPUT);  
  pinMode(PUSH_BUTTON_2, INPUT_PULLUP);  
  digitalWrite(RELAY_PIN_2, relay2State);
  
  timer.setInterval(GET_PZEM_DATA_TIME,       get_pzem_data);                   // How often you would like to call the function
  timer.setInterval(AUTO_MODE_TIME,           auto_mode);    
  timer.setInterval(PHYSICAL_BUTTON_TIME,     checkPhysicalButton);             // Setup a Relay function to be called every 100 ms
  timer.setInterval(SEND_TO_BLYNK_TIME,       sendtoBlynk);                     // Send PZEM values blynk server every 10 sec
}

void sendtoBlynk()                                                              // Here we are sending PZEM data to blynk
{
  Blynk.virtualWrite(vPIN_VOLTAGE_1,               voltage_usage_1);
  Blynk.virtualWrite(vPIN_CURRENT_USAGE_1,         current_usage_1);
  Blynk.virtualWrite(vPIN_ACTIVE_POWER_1,          active_power_1);
  Blynk.virtualWrite(vPIN_ACTIVE_ENERGY_1,         active_energy_1);
  Blynk.virtualWrite(vPIN_FREQUENCY_1,             frequency_1);
  Blynk.virtualWrite(vPIN_POWER_FACTOR_1,          power_factor_1);

  Blynk.virtualWrite(vPIN_VOLTAGE_2,               voltage_usage_2);
  Blynk.virtualWrite(vPIN_CURRENT_USAGE_2,         current_usage_2);
  Blynk.virtualWrite(vPIN_ACTIVE_POWER_2,          active_power_2);
  Blynk.virtualWrite(vPIN_ACTIVE_ENERGY_2,         active_energy_2);
  Blynk.virtualWrite(vPIN_FREQUENCY_2,             frequency_2);
  Blynk.virtualWrite(vPIN_POWER_FACTOR_2,          power_factor_2);

  Blynk.virtualWrite(vPIN_VOLTAGE_3,               voltage_usage_3);
  Blynk.virtualWrite(vPIN_CURRENT_USAGE_3,         current_usage_3);
  Blynk.virtualWrite(vPIN_ACTIVE_POWER_3,          active_power_3);
  Blynk.virtualWrite(vPIN_ACTIVE_ENERGY_3,         active_energy_3);
  Blynk.virtualWrite(vPIN_FREQUENCY_3,             frequency_3);
  Blynk.virtualWrite(vPIN_POWER_FACTOR_3,          power_factor_3);

  Blynk.virtualWrite(vPIN_SUM_VOLTAGE,             sum_of_voltage);
  Blynk.virtualWrite(vPIN_SUM_CURRENT_USAGE,       sum_of_current);
  Blynk.virtualWrite(vPIN_SUM_ACTIVE_POWER,        sum_of_power);
  Blynk.virtualWrite(vPIN_SUM_ACTIVE_ENERGY,       sum_of_active_energy);
  Blynk.virtualWrite(vPIN_SUM_FREQUENCY,           sum_of_frequency);
  Blynk.virtualWrite(vPIN_SUM_POWER_FACTOR,        sum_of_power_factor);

  Blynk.virtualWrite(VPIN_BUILD_NUMBER,            BUILD_NUMBER);  
}

void pzemdevice1()                                                            // Function to get PZEM device 1 data
{
  Serial.println("====================================================");     // PZEM Device 1 data fetching code starts here
  Serial.println("Now checking PZEM Device 1");
  uint8_t result1;
  
  ESP.wdtDisable();                                                           // Disable watchdog during modbus read or else ESP crashes when no slave connected
  result1 = node1.readInputRegisters(0x0000, 9);
  ESP.wdtEnable(1);                                                           // Enable watchdog during modbus read

  if (result1 == node1.ku8MBSuccess)
  {
    voltage_usage_1      = (node1.getResponseBuffer(0x00) / 10.0f);
    current_usage_1      = (node1.getResponseBuffer(0x01) / 1000.000f);
    active_power_1       = (node1.getResponseBuffer(0x03) / 10.0f);
    active_energy_1      = (node1.getResponseBuffer(0x05) / 1000.0f);
    frequency_1          = (node1.getResponseBuffer(0x07) / 10.0f);
    power_factor_1       = (node1.getResponseBuffer(0x08) / 100.0f);

    Serial.print("VOLTAGE:           ");   Serial.println(voltage_usage_1);       // V
    Serial.print("CURRENT_USAGE:     ");   Serial.println(current_usage_1, 3);    // A
    Serial.print("ACTIVE_POWER:      ");   Serial.println(active_power_1);        // W
    Serial.print("ACTIVE_ENERGY:     ");   Serial.println(active_energy_1, 3);    // kWh
    Serial.print("FREQUENCY:         ");   Serial.println(frequency_1);           // Hz
    Serial.print("POWER_FACTOR:      ");   Serial.println(power_factor_1);
    Serial.println("====================================================");
  }
  else {
    Serial.println("Failed to read PZEM Device 1");
    Serial.println("PZEM Device 1 Data");
    voltage_usage_1      = 0;
    current_usage_1      = 0;
    active_power_1       = 0;
    active_energy_1      = 0;
    frequency_1          = 0;
    power_factor_1       = 0;
    Serial.print("VOLTAGE:           ");   Serial.println(voltage_usage_1);       // V
    Serial.print("CURRENT_USAGE:     ");   Serial.println(current_usage_1, 3);    // A
    Serial.print("ACTIVE_POWER:      ");   Serial.println(active_power_1);        // W
    Serial.print("ACTIVE_ENERGY:     ");   Serial.println(active_energy_1, 3);    // kWh
    Serial.print("FREQUENCY:         ");   Serial.println(frequency_1);           // Hz
    Serial.print("POWER_FACTOR:      ");   Serial.println(power_factor_1);
    Serial.println("====================================================");
    swith_off();                                                                  // Calling swith_off() to turn off relays
    delay(6000);                                                                  // Delay is required or else blynk keeps disconnecting
  }  
}

void pzemdevice2()                                                                // Function to get PZEM device 2 data
{
  Serial.println("====================================================");
  Serial.println("Now checking PZEM Device 2");
  uint8_t result2;
  
  ESP.wdtDisable();
  result2 = node2.readInputRegisters(0x0000, 9);
  ESP.wdtEnable(1);

  if (result2 == node2.ku8MBSuccess)
  {
    voltage_usage_2      = (node2.getResponseBuffer(0x00) / 10.0f);
    current_usage_2      = (node2.getResponseBuffer(0x01) / 1000.000f);
    active_power_2       = (node2.getResponseBuffer(0x03) / 10.0f);
    active_energy_2      = (node2.getResponseBuffer(0x05) / 1000.0f);
    frequency_2          = (node2.getResponseBuffer(0x07) / 10.0f);
    power_factor_2       = (node2.getResponseBuffer(0x08) / 100.0f);

    Serial.print("VOLTAGE:           ");   Serial.println(voltage_usage_2);         // V
    Serial.print("CURRENT_USAGE:     ");   Serial.println(current_usage_2, 3);      // A
    Serial.print("ACTIVE_POWER:      ");   Serial.println(active_power_2);          // W
    Serial.print("ACTIVE_ENERGY:     ");   Serial.println(active_energy_2, 3);      // kWh
    Serial.print("FREQUENCY:         ");   Serial.println(frequency_2);             // Hz
    Serial.print("POWER_FACTOR:      ");   Serial.println(power_factor_2);
    Serial.println("====================================================");
  }
    else {
    Serial.println("Failed to read PZEM Device 2");
    Serial.println("PZEM Device 2 Data");
    voltage_usage_2      = 0;
    current_usage_2      = 0;
    active_power_2       = 0;
    active_energy_2      = 0;
    frequency_2          = 0;
    power_factor_2       = 0;

    Serial.print("VOLTAGE:           ");   Serial.println(voltage_usage_2);         // V
    Serial.print("CURRENT_USAGE:     ");   Serial.println(current_usage_2, 3);      // A
    Serial.print("ACTIVE_POWER:      ");   Serial.println(active_power_2);          // W
    Serial.print("ACTIVE_ENERGY:     ");   Serial.println(active_energy_2, 3);      // kWh
    Serial.print("FREQUENCY:         ");   Serial.println(frequency_2);             // Hz
    Serial.print("POWER_FACTOR:      ");   Serial.println(power_factor_2);
    Serial.println("====================================================");
    swith_off(); 
    delay(6000);                                                                    // Delay is required or else blynk keeps disconnecting
  }
}

void pzemdevice3()                                                            // Function to get PZEM device 1 data
{
  Serial.println("====================================================");     // PZEM Device 1 data fetching code starts here
  Serial.println("Now checking PZEM Device 3");
  uint8_t result3;
  
  ESP.wdtDisable();                                                           // Disable watchdog during modbus read or else ESP crashes when no slave connected
  result3 = node3.readInputRegisters(0x0000, 9);
  ESP.wdtEnable(1);                                                           // Enable watchdog during modbus read

  if (result3 == node3.ku8MBSuccess)
  {
    voltage_usage_3      = (node3.getResponseBuffer(0x00) / 10.0f);
    current_usage_3      = (node3.getResponseBuffer(0x01) / 1000.000f);
    active_power_3       = (node3.getResponseBuffer(0x03) / 10.0f);
    active_energy_3      = (node3.getResponseBuffer(0x05) / 1000.0f);
    frequency_3          = (node3.getResponseBuffer(0x07) / 10.0f);
    power_factor_3       = (node3.getResponseBuffer(0x08) / 100.0f);

    Serial.print("VOLTAGE:           ");   Serial.println(voltage_usage_3);       // V
    Serial.print("CURRENT_USAGE:     ");   Serial.println(current_usage_3, 3);    // A
    Serial.print("ACTIVE_POWER:      ");   Serial.println(active_power_3);        // W
    Serial.print("ACTIVE_ENERGY:     ");   Serial.println(active_energy_3, 3);    // kWh
    Serial.print("FREQUENCY:         ");   Serial.println(frequency_3);           // Hz
    Serial.print("POWER_FACTOR:      ");   Serial.println(power_factor_3);
    Serial.println("====================================================");
  }
  else {
    Serial.println("Failed to read PZEM Device 3");
    Serial.println("PZEM Device 3 Data");
    voltage_usage_3      = 0;                                                     // Assigning 0 if it fails to read PZEM device
    current_usage_3      = 0;
    active_power_3       = 0;
    active_energy_3      = 0;
    frequency_3          = 0;
    power_factor_3       = 0;
    Serial.print("VOLTAGE:           ");   Serial.println(voltage_usage_3);       // V
    Serial.print("CURRENT_USAGE:     ");   Serial.println(current_usage_3, 3);    // A
    Serial.print("ACTIVE_POWER:      ");   Serial.println(active_power_3);        // W
    Serial.print("ACTIVE_ENERGY:     ");   Serial.println(active_energy_3, 3);    // kWh
    Serial.print("FREQUENCY:         ");   Serial.println(frequency_3);           // Hz
    Serial.print("POWER_FACTOR:      ");   Serial.println(power_factor_3);
    Serial.println("====================================================");
    swith_off();
    delay(6000);                                                                  // Delay is required or else blynk keeps disconnecting
  }  
}

void sumofpzem()
{
    Serial.println("Sum of all 3 PZEM devices");
    sum_of_voltage        =   (voltage_usage_1 + voltage_usage_2 + voltage_usage_3);
    sum_of_current        =   (current_usage_1 + current_usage_2 + current_usage_3);
    sum_of_power          =   (active_power_1 + active_power_2 + active_power_3);
    sum_of_active_energy  =   (active_energy_1 + active_energy_2 + active_energy_3); 
    sum_of_frequency      =   (frequency_1 + frequency_2 + frequency_3);
    sum_of_power_factor   =   (power_factor_1 + power_factor_2 + power_factor_3); 
    
    Serial.print("SUM of VOLTAGE:           ");   Serial.println(sum_of_voltage);             // V
    Serial.print("SUM of CURRENT_USAGE:     ");   Serial.println(sum_of_current, 3);          // A
    Serial.print("SUM of ACTIVE_POWER:      ");   Serial.println(sum_of_power);               // W
    Serial.print("SUM of ACTIVE_ENERGY:     ");   Serial.println(sum_of_active_energy, 3);    // kWh
    Serial.print("SUM of FREQUENCY:         ");   Serial.println(sum_of_frequency);           // Hz
    Serial.print("SUM of POWER_FACTOR:      ");   Serial.println(sum_of_power_factor);
    Serial.println("====================================================");      
    low_voltage_check();
    high_voltage_check();    
 }

void changeAddress(uint8_t OldslaveAddr, uint8_t NewslaveAddr)                    // Function to change/assign pzem address
{
  static uint8_t SlaveParameter = 0x06;
  static uint16_t registerAddress = 0x0002;                                       // Register address to be changed
  uint16_t u16CRC = 0xFFFF;
  u16CRC = crc16_update(u16CRC, OldslaveAddr);
  u16CRC = crc16_update(u16CRC, SlaveParameter);
  u16CRC = crc16_update(u16CRC, highByte(registerAddress));
  u16CRC = crc16_update(u16CRC, lowByte(registerAddress));
  u16CRC = crc16_update(u16CRC, highByte(NewslaveAddr));
  u16CRC = crc16_update(u16CRC, lowByte(NewslaveAddr));  
  Serial.println("Changing Slave Address");
  preTransmission();
  pzemSerial.write(OldslaveAddr);
  pzemSerial.write(SlaveParameter);
  pzemSerial.write(highByte(registerAddress));
  pzemSerial.write(lowByte(registerAddress));
  pzemSerial.write(highByte(NewslaveAddr));
  pzemSerial.write(lowByte(NewslaveAddr));
  pzemSerial.write(lowByte(u16CRC));
  pzemSerial.write(highByte(u16CRC));
  delay(10);
  postTransmission();
  delay(100);
  Serial.println("Changing Slave Address is done"); 
  delay(1000);
}

void resetEnergy(uint8_t slaveAddr)    //Reset the slave's energy counter
{
  uint16_t u16CRC = 0xFFFF;
  static uint8_t resetCommand = 0x42;
  u16CRC = crc16_update(u16CRC, slaveAddr);
  u16CRC = crc16_update(u16CRC, resetCommand);
  Serial.println("Resetting Energy");
  preTransmission();
  pzemSerial.write(slaveAddr);
  pzemSerial.write(resetCommand);
  pzemSerial.write(lowByte(u16CRC));
  pzemSerial.write(highByte(u16CRC));
  delay(10);
  postTransmission();
  delay(100);
  while (pzemSerial.available()) {         // Prints the response from the Pzem, do something with it if you like
    Serial.print(char(pzemSerial.read()), HEX);
    Serial.print(" ");
  }
}

/***************************************************
          Relay Functions
 **************************************************/

BLYNK_CONNECTED() {                                           // Every time we connect to the cloud...
  Serial.println("Blynk Connected");
  ReCnctCount = 0;
  blynkConnectionStatusForNotification = true;
  Blynk.syncVirtual(VPIN_BUTTON_1);                           // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_2);
  Blynk.syncVirtual(VPIN_AUTO_MODE_BUTTON_1);
  Blynk.syncVirtual(VPIN_LOW_V_NOTIFICATION);
  Blynk.syncVirtual(VPIN_PHASE_FAIL_NOTIFICATION);   
  Blynk.virtualWrite(VPIN_UPDATE_LED,         0);             // Turn off FOTA Led
  Blynk.virtualWrite(VPIN_FIRMWARE_UPDATE, HIGH);             // Turn off Firmware update button on app  
}

/* When App button is pushed - switch the state */

BLYNK_WRITE(VPIN_BUTTON_1) {  
  relay1State = param.asInt();
  if(lowvoltageflag == false && highvoltageflag == false && phasefailureflag == false){   // Don't activate relay if voltage issue is there
  digitalWrite(RELAY_PIN_1, relay1State);
  }
  else{
   Blynk.virtualWrite(VPIN_BUTTON_1, HIGH);       // Update Button Widget 
  }
}
BLYNK_WRITE(VPIN_BUTTON_2) {
  relay2State = param.asInt();
  digitalWrite(RELAY_PIN_2, relay2State);
}
BLYNK_WRITE(VPIN_AUTO_MODE_BUTTON_1) {                      // Get auto mode button status value
  auto_mode_state_1 = param.asInt();
}
BLYNK_WRITE(VPIN_FIRMWARE_UPDATE) {                         // Get update button enabled or not
  firmwarestate = param.asInt();  
  if(firmwarestate == LOW){
    checkforupdate();
     }
}

/* Under Voltage one time alert */  

BLYNK_WRITE(VPIN_LOW_V_NOTIFICATION)                        // Get button value from blynk app
{
  underVoltageAlertOnOffState = param.asInt();
}
BLYNK_WRITE(VPIN_PHASE_FAIL_NOTIFICATION)                   // Get button value from blynk app
{
  phaseFailureAlertOnOffState = param.asInt();
}

void checkPhysicalButton()                                  // Here we are going to check push button pressed or not and change relay state
{
  if (digitalRead(PUSH_BUTTON_1) == LOW) {
    if (pushButton1State != LOW && (lowvoltageflag == false && highvoltageflag == false && phasefailureflag == false) ) {                          // pushButton1State is used to avoid sequential toggles  
      relay1State = !relay1State;                           // Toggle Relay state
      digitalWrite(RELAY_PIN_1, relay1State);            
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);       // Update Button Widget
    }
    pushButton1State = LOW;
  } else {
    pushButton1State = HIGH;
  }
  
  if (digitalRead(PUSH_BUTTON_2) == LOW) {
    if (pushButton2State != LOW) {                        // pushButton2State is used to avoid sequential toggles     
      relay2State = !relay2State;                         // Toggle Relay state
      digitalWrite(RELAY_PIN_2, relay2State);
      Blynk.virtualWrite(VPIN_BUTTON_2, relay2State);     // Update Button Widget
    }
    pushButton2State = LOW;
  } else {
    pushButton2State = HIGH;
  }
}

void get_pzem_data()                                       // Function to check time to see if it reached mentioned time to fetch PZEM data
{
    pzemdevice1(); 
    pzemdevice2();
    pzemdevice3();
    sumofpzem();
}

void low_voltage_check()
{
  if(voltage_usage_1 == 0 || voltage_usage_2 == 0 || voltage_usage_3 == 0){
    Serial.println("Phase failure detected...");
    phasefailureflag = true;
    phasefailurenotification();
  } else if(voltage_usage_1 < LOW_VOLTAGE_1_CUTOFF || voltage_usage_2 < LOW_VOLTAGE_2_CUTOFF || voltage_usage_3 < LOW_VOLTAGE_3_CUTOFF){
    Serial.println("Low voltage detected...");
    lowvoltageflag = true;
    swith_off();
    low_volt_alert();
  } else {
      Serial.println("Voltage back to normal");
      lowvoltagenotificationflag = true;
      phasefailurenotificationflag = true;
      lowvoltageflag = false;
      phasefailureflag = false;      
  }
}

void high_voltage_check()
{
  if(voltage_usage_1 > HIGH_VOLTAGE_1_CUTOFF || voltage_usage_2 > HIGH_VOLTAGE_2_CUTOFF || voltage_usage_3 > HIGH_VOLTAGE_3_CUTOFF){
    Serial.println("High voltage detected...");
    highvoltageflag = true;
    swith_off();
  } else {
     highvoltageflag = false;
  }
}

void low_volt_alert()                              // Function to send blynk push notifiction if low voltage is detected
{
  if(lowvoltagenotificationflag == true && underVoltageAlertOnOffState == 0 && blynkConnectionStatusForNotification == true){ 
    Serial.println("Sending Under voltage Blynk notification");
    Blynk.notify("Low Voltage Detected!");
    lowvoltagenotificationflag = false;
  }
}

void phasefailurenotification()
{
   if(phasefailurenotificationflag == true && phaseFailureAlertOnOffState == 0 && blynkConnectionStatusForNotification == true){
     Serial.println("Sending Phase Failure Blynk notification");
     Blynk.notify("Phase Failure Detected!");
     phasefailurenotificationflag = false;
   }
}
 
void swith_off()                                              // Function to switch off relays
{
    Serial.println("Switching off relay now..");
    digitalWrite(RELAY_PIN_1, HIGH);                // Turnoff Relay 1
    Serial.println("Relay 1 OFF..");       
    Blynk.virtualWrite(VPIN_BUTTON_1, HIGH);        // Update Relay Off status on Blynk app   
}
  
void auto_mode()                                    // Function to check if auto mode is ON and all voltage value is greater than voltage cutoff value, then turn on 2 relays
{    
  if(auto_mode_state_1 == LOW && lowvoltageflag == false && highvoltageflag == false && phasefailureflag == false){  //checks if auto mode is ON and voltage values is greater than min value
    Serial.println("All condition is TRUE...swtiching on relay now.");    
    digitalWrite(RELAY_PIN_1, LOW);                 // Turn on Relay 1         
    Blynk.virtualWrite(VPIN_BUTTON_1, LOW);         // Update Blynk button status to ON    
    Serial.println("RELAY 1 Turned ON"); 
  }
}
  
void checkforupdate()
{ 
  Serial.println( "FOTA Update Request Received" );
  Serial.print( "Firmware URL: " );
  Serial.println( FIRMWARE_URL );

  HTTPClient httpClient;
  httpClient.begin( FIRMWARE_URL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
  Serial.println( "Update file found, starting update" );
  Blynk.virtualWrite(VPIN_UPDATE_LED, 1023);
  
  t_httpUpdate_return ret = ESPhttpUpdate.update( FIRMWARE_URL );
   
  switch(ret) {
    case HTTP_UPDATE_FAILED:
        Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok.");      // may not called we reboot the ESP
        break;
              }
           }  else {
    Serial.print( "Firmware check failed, got HTTP response code " );
    Serial.println( httpCode );
           }
  httpClient.end();  
  Blynk.virtualWrite(VPIN_UPDATE_LED, 0);
  Blynk.virtualWrite(VPIN_FIRMWARE_UPDATE, HIGH);  
}

void loop()
{
  timer.run();
  if (Blynk.connected()) {                                                    // If connected run as normal
    Blynk.run();
  } 
  else if (ReCnctFlag == 0) {                                                      // If NOT connected and not already trying to reconnect, set timer to try to reconnect in 30 seconds
      blynkConnectionStatusForNotification = false;
      ReCnctFlag = 1;                                                         // Set reconnection Flag
      Serial.println("Starting reconnection timer in 30 seconds...");
      timer.setTimeout(30000L, []() {                                         // Lambda Reconnection Timer Function
      ReCnctFlag = 0;                                                         // Reset reconnection Flag
      ReCnctCount++;                                                          // Increment reconnection Counter
      Serial.print("Attempting reconnection #");
      Serial.println(ReCnctCount);
      Blynk.connect();                                                        // Try to reconnect to the server
    });                                                                       // END Timer Function
    } 
}
