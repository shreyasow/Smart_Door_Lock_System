# Smart Door Lock System with Fingerprint Authentication

## Overview
An IoT-based smart door lock system built using ESP32 and a fingerprint sensor for secure, keyless access. The system performs real-time authentication, logs access data to the cloud, and includes additional security features such as intruder detection.

## Features
- Fingerprint-based authentication (enrollment, verification, deletion)
- Servo-controlled door locking and unlocking mechanism
- Wi-Fi enabled system using ESP32
- Cloud-based access logging using Google Sheets API
- Intruder detection with image capture for unauthorized attempts
- Audio and visual feedback using DF Player Mini and 16x2 LCD

## Hardware Used
- ESP32 microcontroller  
- R305 / R307 Fingerprint Sensor  
- SG90 Servo Motor  
- DF Player Mini + Speaker  
- 16x2 I2C LCD Display  

## Software & Tools
- Arduino IDE (C++)  
- Python  
- Google Sheets API  

## System Workflow
1. User places finger on fingerprint sensor  
2. System verifies fingerprint against stored data  
3. If authenticated, the door unlocks using a servo motor  
4. Access data is logged to the cloud via Wi-Fi  
5. If unauthorized, the system captures an image and logs the event  

## Demo Video
[Watch Demo](https://drive.google.com/drive/folders/1oUYoJzkHFoCdt_4wARLgBWtrEP2hGygu?usp=drive_link)

## Team
- Shreya Vinoj  
- Saraswathy Menon  
- Stuti Prajapati  

## Future Improvements
- Mobile app integration  
- Face recognition support  
- Enhanced security for cloud data  

## Note
This project was developed as a hands-on embedded systems and IoT application focusing on real-world hardware–software integration.
