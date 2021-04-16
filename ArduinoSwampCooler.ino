/*****************************************************************************************************************************************
CPE 301 SWAMP COOLER PROJECT - STEVEN BERRY, JUSTIN CARLSON
DECEMBER 6, 2020
******************************************************************************************************************************************
--GOALS FOR IMPLEMENTATION--

1. Monitor the water levels in a reservoir and print an alert when the level is too low
    Implementation: Controlled in program flow - automatically checks for errors before resolving the rest of the state

2. Monitor and display the current air temp and humidity on an LCD screen
    Implementation: In main loop, constantly detect air temp and humidity, print to LCD screen

3. Start and stop a fan motor as needed when the temperature falls out of a specified range (high or low)
    Implementation: In main loop, use air temp: if above a threshold, activate fan motor

4. Allow a user to use a control to adjust the angle of an output vent from the system
    Implementation: Depends on hardware (servo motor?) - Check for user input and adjust angle

5. Allow as user to enable or disable the system using an on/off button
    Implementation: Controlled in program flow - detects for button press in state

6. Record the time and date every time the motor is turned on or off. This information should be transmitted to a host computer (over USB)
    Implementation: For each activation/shutdown, record date and time, and transmit using a USB protocol
******************************************************************************************************************************************/

// *********** INCLUDES *********
#include "LiquidCrystal.h"
#include <dht.h>


//Pin Initialization - Mutable Before Runtime (p_rs: register select, p_en: enable)
const int p_rs = 8, p_en = 7, p_d4 = 6, p_d5 = 5, p_d6 = 4, p_d7 = 3;
const int l_yel = 11, l_grn = 12, l_red = 10, l_blu = 13;

#define lowWaterWarning 1
#define highTempWarning 10

#define waterSensorPower 2
#define waterSensorPin A0

#define humiditySensorPin 9

#define fanPowerPin1 40
#define fanPowerPin2 41
#define pumpPowerPin 42

#define disableButton 24

//Constant Variables Initialization
const char* empty_str = "                ";
enum STATE{OFF, IDL, ERR, RUN};
STATE state_cur = IDL;

// ********* HUMIDITY SENSOR FUNCS ***************

dht humiditySensor;

int readHumidity(){
    int chk = humiditySensor.read11(humiditySensorPin);
    int humidity = humiditySensor.humidity;
    while (humidity < -15){
      delay(25);
      chk = humiditySensor.read11(humiditySensorPin);
      humidity = humiditySensor.temperature;
    }
    return humidity;
}

int readTemp(){
    int chk = humiditySensor.read11(humiditySensorPin);
    int temperature = humiditySensor.temperature;
    while (temperature < -15){
      delay(25);
      chk = humiditySensor.read11(humiditySensorPin);
      temperature = humiditySensor.temperature;
    }
    return temperature;
}

// ********* WATER LEVEL SENSOR FUNCS ***************

int readWaterSensor() {
  digitalWrite(waterSensorPower, HIGH);
  delay(10);
  int val = analogRead(waterSensorPin);
  digitalWrite(waterSensorPower, LOW);
  return val;
}

// ********* FAN FUNCS ***************

void enableFans(){
  digitalWrite(fanPowerPin1, HIGH);
  digitalWrite(fanPowerPin2, HIGH);
}

void disableFans() {
  digitalWrite(fanPowerPin1, LOW);
  digitalWrite(fanPowerPin2, LOW);
}

// ********* PUMP FUNCS ************

void enablePump(){
  digitalWrite(pumpPowerPin, HIGH);
}

void disablePump() {
  digitalWrite(pumpPowerPin, LOW);
}

// ********* LED FUNCS ************

void handleLightsForState(STATE s){
  switch(s){
    case OFF:
    digitalWrite(l_yel, 1);
    digitalWrite(l_grn, 0);
    digitalWrite(l_red, 0);
    digitalWrite(l_blu, 0);
    break;
    
    case IDL:
    digitalWrite(l_yel, 0);
    digitalWrite(l_grn, 1);
    digitalWrite(l_red, 0);
    digitalWrite(l_blu, 0);
    break;
    
    case ERR:
    digitalWrite(l_yel, 0);
    digitalWrite(l_grn, 0);
    digitalWrite(l_red, 1);
    digitalWrite(l_blu, 0);
    break;
    
    case RUN:
    digitalWrite(l_yel, 0);
    digitalWrite(l_grn, 0);
    digitalWrite(l_red, 0);
    digitalWrite(l_blu, 1);
    break;
  }
  
}

// ********* LCD FUNCS *********
LiquidCrystal lcd(p_rs, p_en, p_d4, p_d5, p_d6, p_d7);

void lcd_print_top(char* ptr){
  lcd.setCursor(0,0);
  lcd.print(empty_str);
  lcd.setCursor(0,0);
  lcd.print(ptr);
}

void lcd_print_bottom(char* ptr){
  lcd.setCursor(0,1);
  lcd.print(empty_str);
  lcd.setCursor(0,1);
  lcd.print(ptr);
}

void lcd_print_data(){
    lcd.setCursor(0,1);
    int new1 = readTemp();
    lcd.print(new1);
    lcd.print((char)223);
    lcd.print("C | ");
    lcd.print("Hum: ");
    int new2 = readHumidity(); 
    lcd.print(new2);
    lcd.print("%");
}

void print_status(STATE s){
  switch(s){
    case OFF:
    lcd_print_top("  SWAMP COOLER  ");
    lcd_print_bottom("  IS DISABLED   ");
    break;
    
    case IDL:
    lcd_print_top("    FAN IDLE    ");
    lcd_print_data();
    break;
    
    case ERR:
    lcd_print_top("ERROR: WATER LOW");
    lcd_print_data();
    break;
    
    case RUN:
    lcd_print_top("   FAN IS ON    ");
    lcd_print_data();
    break;
  }
}

/****************************************************************************/

//SETUP FUNCTION
void setup(){
  lcd.begin(16, 2);
  pinMode(l_yel, OUTPUT);
  pinMode(l_grn, OUTPUT);
  pinMode(l_red, OUTPUT);
  pinMode(l_blu, OUTPUT);

  pinMode(p_rs, OUTPUT);
  pinMode(p_en, OUTPUT);
  pinMode(p_d4, OUTPUT);
  pinMode(p_d5, OUTPUT);
  pinMode(p_d6, OUTPUT);
  pinMode(p_d7, OUTPUT);

  pinMode(waterSensorPower, OUTPUT);
  digitalWrite(waterSensorPower, LOW);

  pinMode(fanPowerPin1, OUTPUT);
  pinMode(fanPowerPin2, OUTPUT);
  pinMode(pumpPowerPin, OUTPUT);

  pinMode(disableButton, INPUT_PULLUP);  

  Serial.begin(9600);

  //log_dt(); //Logs the current date and time
}

//LOOP FUNCTION
void loop(){
  print_status(state_cur); //Prints state onto the LCD display, and humidity/temp if not OFF
  handleLightsForState(state_cur);

  //if(b_lf)
  //  vent_left();
  //if(b_rt)
  //  vent_right();

  bool b_red = digitalRead(disableButton);

  switch(state_cur){
    //"OFF" State
    case OFF:
    if(b_red){
      if(readWaterSensor() < lowWaterWarning){
        state_cur = ERR;
        disablePump();
        disableFans();
      }
      else if(readTemp() > highTempWarning){
        state_cur = RUN;
        enableFans();
        enablePump();
      }
      else{
        state_cur = IDL;
        disablePump();
        disableFans();
      }
    }
    break;

    //"IDL" State
    case IDL:
    if(readWaterSensor() < lowWaterWarning){
      state_cur = ERR;
      disablePump();
      disableFans();
    }
    else if(b_red){
      state_cur = OFF;
      disablePump();
      disableFans();
    }
    else if(readTemp() > highTempWarning){
      state_cur = RUN;
      enableFans();
      enablePump();
    }
    break;

    //"ERR" State
    case ERR:
    if(readWaterSensor() > lowWaterWarning){
      state_cur = IDL;
      disablePump();
      disableFans();
    }
    break;

    //"RUN" State
    case RUN:
    if(readWaterSensor() < lowWaterWarning){
      state_cur = ERR;
      disablePump();
      disableFans();
    }
    else if(b_red == LOW){
      state_cur = OFF;
      disablePump();
      disableFans();
    }
    else if(readTemp() < highTempWarning){
      state_cur = IDL;
      disablePump();
      disableFans();
    }
  }
  
  delay(1000); // sensors have an update rate of 1 cycle per second but need to monitor for disable button
}
