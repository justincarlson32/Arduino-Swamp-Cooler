# Arduino-Swamp-Cooler
An Arduino swamp cooler designed, manufactured, and programmed by Justin Carlson & Steven Berry for Computer Engineering 301.

The parts required for this project were:

2 square feet of Stainless Steel  
1 5v water pump  
2 5-9v Axial Fans  
1 Arduino MEGA256  
1 Arduino Compatible Water-Level Sensor  
1 Arduino Comaptible 16x2 LCD display  
1 5K ohm potentiometer  
1 Arduino Comaptible 3x1 Humidity Sensor  
1 Arduino 5v/3v Power Supply  
1 square foot of evaporative cooler filter  
1 foot of vinyl tubing  
1 Empty and Cleaned, one gallon milk container  
and Some unidentifiable amount of hot glue  

The goals and actual implementations of this project were as follows:  

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


A little more information as to how it was constructed:  

The schematic for the chasis is as shown and was cut from stainless steel and bent into shape:
![pic](https://user-images.githubusercontent.com/41803725/114960027-1515fc00-9e1b-11eb-8329-2b52fa8d3284.jpg)

After the chasis was built, a test fitting of parts to ensure proper space:
![testfit](https://user-images.githubusercontent.com/41803725/114960696-6d013280-9e1c-11eb-9a74-4340ccf89d4e.jpg)

Then a wiring diagram was generated for simplication of wiring duty:
![pinout](https://user-images.githubusercontent.com/41803725/114960010-0f201b00-9e1b-11eb-8f09-b9177ead9bc5.png)
