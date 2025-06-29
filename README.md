PURPOSE OF THE PROJECT
It includes a comprehensive smart home security and automation system developed to meet the need to make the environments in which individuals live safer and smarter.
The main purpose of the project is to design a smart system that meets the various security needs of a house or a small-scale settlement in an integrated manner; It includes modules such as access control, intrusion detection, environmental monitoring, fire and gas detection, lighting control, audio and visual warning systems. The PIC16F877A microcontroller, which forms the core of this system, is a low-cost and high-efficiency control unit and enables effective communication with all environmental sensors and output units.
Within the scope of the project; functions such as entry verification with a 9-digit code specified in the code entered from the 4x3 keypad, detection of physical entry violations by interrupting the laser light continuously hitting the LDR, entry violation warnings on the LCD display and notifications of password entry stages, automatic lighting control and LED control depending on the ambient light with the LDR, detection of flammable gases in the environment in the event of a possible fire with a gas sensor simulated with a potentiometer, and detection of ambient temperature with the Lm35 temperature sensor have been brought together. The system reacts instantly with buzzers and red-green LEDs. Thus, the security level is maximized.
With this project, different sensors and system modules work in a compatible and integrated manner. The design of the system is modular in terms of both software and hardware; It has an expandable structure that can meet the needs of modern smart home systems such as wireless communication, mobile application support or remote control in the future. In addition, it is a structure suitable for expansion for ideas such as smart factory systems and smart workplaces and was designed for this purpose.
By developing a home security and automation system with low-cost components but high functionality; it offers a practically applicable solution. This project is planned in a structure that can be adapted to real-life scenarios, developed and integrated into various systems.
PROJECT SCENARIO
DOOR SECURITY SYSTEM OPERATION STEPS
System Startup
-When 5V power is applied to the system, PIC16F877A starts working.
-"*’a basınız" message appears on the screen.
-When the user presses * on the keypad, the system goes into password entry mode and the "Şifre Giriniz" message appears on the second line of the LCD.
Password Entry Mechanism
-User enters 9-digit password with keypad.
-At the end of entry, confirms password by pressing # key.
Software control is provided
if (strcmp(girilenSifre, dogruSifre) == 0)
-In this line, entered password is compared with the correct password registered in the system.
-If Password is Correct – Entry Confirmation
-"Giriş Başarılı" message appears on LCD.
-Pin C1 becomes logic 1. The transistor is triggered and activates the relay. The green LED lights up. 
-This continues for 5 seconds, then the system automatically locks the door and returns to standby mode, the green LED goes off.
output_high(PIN_C1); // Relay and green LED 
delay_ms(5000);
output_low(PIN_C1);
Yanlış Şifre Girişi:
- "Yanlış Şifre" and the number of remaining attempts are printed on the LCD. 
-The remaining attempts are printed on the bottom line:
printf(lcd_putc,"\nSon %d deneme", deneme);
-With each incorrect attempt, the entry right counts down from 3 with the try-- command.
-If the number of incorrect attempts reaches 3, the system is locked.
-On the LCD: System Locked and countdown (60 sec) appears.
-Password entries are blocked during this period.
-The counter is updated every second, and is displayed on the LCD 2nd line.
-The counter is displayed on the LCD screen ("Kalan: xx sn").
-When the time is up, the system automatically resets itself. Entry Violation Detection -LDR is connected to the ADC0 channel.
-If the ADC value is < 450 → the system considers it as a physical violation. 
-"GİRİŞ İHLALİ" message appears on the LCD. 
-Buzzer is activated if it is enabled.(PIN_C0) 
-Password entry is suspended, entered values are deleted.
-The violation continues to be monitored even if the system is in lock mode.
-The alarm continues until the system is back to a safe value.
-The system continues to monitor the violation even when locked for 60 seconds.
-If the violation is removed (adc_value >= 450), the screen changes to System Locked but the alarm stops.
-Password entry remains closed until the time is up.
Note: This check is performed even when the system is locked. In other words, even if the 60-second counter is running, if there is a new violation, it will give an alarm again. The most important point is that the time continues in the event of a violation and when the violation is removed, it continues not from where it left off, but by adjusting the elapsed time in the event of a violation and deducting the total elapsed time.
-When the entry violation is over, the system stops the warning and the default message appears Laser Interruption Detection 
-The LDR sensor connected to AN0 must receive continuous laser light. 
-This LDR looks directly at the laser light. 
-If the laser is interrupted, the LDR resistance increases. The voltage value going to the analog pin decreases and the ADC value also decreases. 
-In this case, the system detects a physical violation. 
-The "GİRİŞ İHLALİ" message and alarm mechanisms are activated.
-In this way, for example, an unauthorized person entering the door will be prevented by the warning system.
AMBIENT LIGHTING MECHANISM
- A different LDR sensor is also used to measure ambient light.
- If the ambient light is low, the system automatically turns on the ambient LED (PIN_C3). Here, the LED symbolizes the lighting source.
- If the light level increases, the LED turns off.
- Thanks to this feature, the lighting is automatically adjusted according to the ambient conditions.
FIRE AND GAS DETECTİON MECHANISM
-The LM35 temperature sensor is connected to ADC3 channel.
-The MQ-2 gas sensor measures gas concentration (ppm) via ADC1.
-In the code, gas and temperature values are evaluated together:
if (ppm > 600 && sicaklik > 50) {
   output_high(PIN_C2); // Buzzer
   output_high(PIN_C0); // Red LED
}
-When both values exceed the threshold, the fire/gas alarm is triggered.
-The buzzer blinks at 500ms intervals.
-When the danger is eliminated, the system automatically stops the alarm.


PROTEUS SİMULATION
Image showing the simulation output 

![image](https://github.com/user-attachments/assets/184f4b49-c66f-44bf-9979-78634a05ccac)

Image showing the section before password entry 

![image](https://github.com/user-attachments/assets/1c01cdde-356b-4416-81f3-90a74a9f6956)

Image showing the password entry section 

![image](https://github.com/user-attachments/assets/4af3e142-f20d-4b5e-9995-cbd514c77b13)

Image showing the correct password entry  

![image](https://github.com/user-attachments/assets/58ab015e-8ccb-4759-9dd4-3973be8b9465)

Image showing the screen displayed when an incorrect password is entered  

![image](https://github.com/user-attachments/assets/af668c3d-a0d6-4e70-9781-902da81a1d67)

Image showing the screen displayed after the 3rd incorrect password entry 

![image](https://github.com/user-attachments/assets/9c4a721d-b3d4-41de-a580-86e11fdb0f29)

Image showing the countdown timer operating after the system lock message triggered by the 3rd incorrect password entry 

![image](https://github.com/user-attachments/assets/366f997d-d78c-410c-9063-10bb3eb7867a)

Activation of the laser security system  

![image](https://github.com/user-attachments/assets/a3f30501-2440-4855-899a-94c236c19e17)

Image of the lighting system before activation  

![image](https://github.com/user-attachments/assets/2eb01382-e2ad-4b10-b8d9-9a55e008458a)

Image displaying the lighting system while active 

![image](https://github.com/user-attachments/assets/ac0688e4-9b2b-48fb-af12-62d8d881f161)

Image showing the fire alarm circuit in its inactive state 

![image](https://github.com/user-attachments/assets/15d2fa93-06ec-490f-b71b-d8ccccc9389f)

Image of the fire alarm circuit after it has been activated  

![image](https://github.com/user-attachments/assets/e941ef9b-9de6-4521-9b77-80f8d1f1a2de)
