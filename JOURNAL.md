---
title: "FloraSync"
author: "Sivanand"
description: "FloraSync is an integrated smart agriculture system developed to improve farm productivity, sustainability, and safety. It consists of five key features: an ESP32-based automatic irrigation system that optimizes water usage, a drone-powered plant disease detection system, a Raspberry Pi-based animal intrusion alert system, an AgriBot for automated fertilizer spraying, and a smart pond safety system that monitors water quality, utilizes ammonia levels for analysis, and sends real-time alerts."
created_at: "2026-09-12"
---

# September 12: Completed the Connections and Insulation

The hardware wiring and insulation work for the prototype has been successfully completed. All major components have been connected, secured, and insulated, preparing the system for the next stage of testing and integration.

![Hardware Connection and Insulation](images/WhatsApp%20Image%202026-09-12%20at%201.38.44%20PM.jpeg)

![Hardware Connection and Insulation](images/WhatsApp%20Image%202026-09-12%20at%201.38.44%20PM2.jpeg)

![Hardware Connection and Insulation](images/WhatsApp%20Image%202026-09-12%20at%201.38.45%20PM.jpeg)

![Hardware Connection and Insulation](images/WhatsApp%20Image%202026-09-12%20at%201.38.45%20PM2.jpeg)

**Total time spent:** 3 hours

---

# September 12, 3:00 PM: Worked on Raspberry Pi

Continued development and testing of the Raspberry Pi-based animal intrusion alert system as part of the FloraSync project. The Raspberry Pi 3 Model B was prepared for a headless setup so that it could be accessed from a laptop over Wi-Fi without requiring a dedicated monitor.

The Raspberry Pi OS was written to the microSD card, and the Raspberry Pi was powered on for testing. The power and activity LEDs indicated that the Raspberry Pi was receiving power and accessing the SD card.

During the setup, however, the SD card encountered several issues. Raspberry Pi Imager reported that verification of the written data failed because the contents of the SD card differed from what was written. Attempts to format the card through Windows also failed. DiskPart detected the SD card as a 31 GB removable disk, but attempting to clean the disk resulted in a **"The device is not ready"** error.

As a result, the Raspberry Pi could not yet be successfully configured for the planned Wi-Fi-based headless connection. Further testing of the SD card and card reader is required before continuing with the Raspberry Pi setup.

![Raspberry Pi SD Card Setup Failure](images/raspberry-pi-sd-card-failure.jpeg.jpeg)
![Raspberry Pi SD Card Setup Failure](images/raspberry-pi-sd-card-failure2.jpeg.png)


**Total time spent:** 4 hours

**Progress:**  In Progress — Learned the purpose of using an SD card as the Raspberry Pi's main storage for the operating system and configuration. 
Also learned how to use Raspberry Pi Imager, configure Wi-Fi and SSH, identify SD-card issues, and troubleshoot write and verification failures.

**Issue:** SD card write and verification failure

**Next step:** Already tested the SD card for many times so i am going to buy new SD card.

---

## September 12, 11:00 PM: Purchased Components for FloraSync

Purchased several electronic and mechanical components required for the continued development of the **FloraSync smart agriculture system**. These components will be used across different subsystems, particularly the **AgriBot fertilizer spraying system**, power supply setup, and LCD-based monitoring systems.

The components purchased were:

- **R385 DC 6V–12V Diaphragm Based Water Pump ET6106** — 2 units
- **100mm White Plastic Cable Ties ET8118** — 1 pack of 100
- **16×2 LCD JHD Display with Yellow-Green Backlight ET5417** — 1 unit
- **LCD I2C/IIC Serial Interface Adapter Module ET5424** — 1 unit
- **DMEGC 18650 3.7V 2600mAh EV Grade NMC Li-ion Battery ET7393** — 3 units
- **3-Cell 18650 Battery Holder in Series ET8037** — 1 unit

The two diaphragm-based water pumps will be used for the **AgriBot fertilizer spraying system**. The cable ties will help with wire management and securing components during the construction of the project.

The **16×2 LCD** and **I2C adapter module** will be used for displaying system information, sensor readings, and system status while reducing the number of microcontroller GPIO pins required for the LCD connection.

The three **18650 Li-ion batteries** and the **3-cell series battery holder** will be used to develop a portable power supply for suitable FloraSync hardware components.

These components will help continue the hardware development and integration of the different subsystems of the FloraSync project.

![Components Purchased](images/components-purchased.png)

**Total time spent:** 1 hour

**Progress:** Purchased important hardware components required for the continued development of FloraSync.

**Components purchased:** 2 diaphragm water pumps, 100 cable ties, 1 16×2 LCD, 1 LCD I2C adapter, 3× 18650 Li-ion batteries, and 1 three-cell series battery holder.

**Next step:** Test the newly purchased components and begin integrating them into their respective FloraSync subsystems.\

---

## September 13, 1:00 PM: Completed Soldering and Wiring of Desktop Companion

Completed the hardware assembly of the **FloraSync Desktop Companion** by soldering the required components and wires onto a **dotted PCB**. All the planned wire connections were made and securely soldered to create a more permanent and organized circuit.

The components were positioned on the dotted PCB according to the planned arrangement, and the necessary **power, ground, and signal wires** were connected. Care was taken while soldering to ensure that the connections were secure and properly arranged.

With the soldering and wiring completed, the Desktop Companion hardware is now ready for **testing and integration with the FloraSync system**.

![Desktop Companion Soldering](images/desktop-companion.jpeg)
![Desktop Companion Soldering](images/desktop-companion3.jpeg)

**Total time spent:** 5 hours and 30 minutes

---

# September 27: Completed Turbidity Sensor Connections and Voltage Divider

The turbidity monitoring hardware for the smart pond safety system has been successfully completed. The turbidity sensor was connected to the ESP32 using a dotted PCB, with a two-resistor voltage divider implemented to safely interface the sensor's analog output with the ESP32 ADC.

The voltage divider was constructed using two 10kΩ resistors, reducing the sensor's analog voltage before connecting it to GPIO34. All required connections were soldered securely on the dotted PCB, completing the hardware interface between the turbidity sensor and ESP32.

This completes the turbidity sensing hardware setup and prepares the system for the next stage of testing, calibration, and water-quality monitoring.

Next Step is to complete the whole coding and make it work using mqtt

![Turbidity Sensor and ESP32 Connection](images/aquasync_1.jpeg)

![Turbidity Sensor Voltage Divider](images/aquasync_2.jpeg)

**Components completed:** Turbidity Sensor, ESP32, Dotted PCB, 2 × 10kΩ Resistors

**Total time spent:** 2 hours

**Progress:** Completed the soldering and wiring of the Desktop Companion circuit on the dotted PCB.

**Hardware completed:** Dotted PCB assembly with all required component and wire connections.

---

# September 27 1:30: Connected ESP32-CAM with the Website

Successfully completed the connection between the ESP32-CAM and the FloraSync web dashboard. The ESP32-CAM was configured to connect to the local Wi-Fi network and provide a live video stream from the robot.

The camera stream was integrated into the React-based FloraSync website using the ESP32-CAM stream URL. After troubleshooting the camera IP address and network connection, the live camera feed was successfully displayed directly on the dashboard.

![ESP32-CAM Setup](images/esp32_cam3.jpeg)

![ESP32-CAM Live Feed on Website](images/esp32_cam2.jpeg)

![ESP32-CAM Live Feed on Website](images/esp32_cam1.jpeg)

**Total time spent:** 5 hours and 30 minutes

---

# September 28: Completed the BMS and Motor System For Agribot

The battery management and motor system for the agribot pesticide spraying Unit has been successfully completed. A 3-cell (3S) lithium-ion battery pack was integrated with a 3S BMS for battery protection and power management. The motor/pump system was connected through the relay module and tested successfully. The motors operated properly during testing, confirming that the power distribution and switching system are functioning as expected.

![BMS and Motor System](images/bms.jpeg)

![BMS and Motor System](images/sprayingsystem.jpeg)

**Total time spent:** 5 hours and 30 minutes

---

# October 6: Completed the AgriBot Mechanical Assembly

The mechanical development of the AgriBot has been successfully completed. The robot chassis was assembled with four wheels, DC motors, and the required hardware components mounted securely on the wooden base.

A T-shaped PVC pipe structure was also fabricated and attached to the AgriBot using M-Seal to create a strong and stable connection. This structure will serve as the main support for the fertilizer spraying mechanism.

The pumps, motor driver, relay module, battery pack, and other components were positioned on the chassis. With the mechanical structure completed, the AgriBot is now ready for further wiring, programming, and integration of the fertilizer spraying system.

![AgriBot Mechanical Assembly](images/agribot_1.jpeg)

![AgriBot Mechanical Assembly](images/agribot_2.jpeg)

![AgriBot Mechanical Assembly](images/agribot_3.jpeg)

![AgriBot Mechanical Assembly](images/agribot_4.jpeg)

**Total time spent:** 10 hours

---


## October 10: Tested and Replaced the GSM Power Adapter

The power supply for the SIM800A GSM module was tested as part of the FloraSync animal intrusion alert system. During testing, the 9V 2A adapter failed to provide a measurable voltage output, indicating a possible fault with the adapter or its cable.

To resolve this issue, a replacement **ERD 9V 2A Adapter ET5616** was selected for purchase. The replacement adapter is intended to power the SIM800A GSM module through its DC input jack, which is specified for a 9–12V supply.

The GSM module's power supply will be tested again after obtaining the replacement adapter. Once stable power is confirmed, the next step is to verify GSM network registration and test SMS alerts before integrating the module with the Arduino UNO and the laptop-based YOLO animal detection model.

**Total time spent:** 1 hour

**Progress:** Identified a possible fault with the original power adapter and selected a replacement.

**Issue:** No measurable voltage output from the original adapter during testing.

![Adapter](images/adapter.jpeg)
![Adapter](images/adapter1.png)

**Next step:** Test the replacement adapter's output and polarity, power the SIM800A safely, and verify GSM communication and SMS delivery.

---

# October 18: Completed Plant Disease Detection AI and Website Integration

The Plant Disease Detection AI system has been successfully developed and deployed as part of the FloraSync smart agriculture project. The model is accessible through a web application, allowing users to upload plant images and obtain AI-based disease predictions.

The deployed application was then integrated into the main FloraSync website through the Plant Disease AI section, making it accessible alongside the existing AgriBot dashboard and other smart agriculture features. This milestone brings FloraSync one step closer to becoming a unified platform for intelligent crop monitoring and agricultural automation.

![Plant Disease Detection AI Web Application](images/plant_disease_1.png)

![Plant Disease AI Integrated into the FloraSync Dashboard](images/plant_disease_2.png)

![Plant Disease Detection Model Training Results](images/plant_disease_3.png)

![Plant Disease Detection Model Training Results](images/plant_disease_4.png)

**Total time spent:** 7 Hours and 15 minutes

---


# October 19: Completed Sensor Connections on Dotted PCB

The soldering and hardware connections for the AquaSync water-quality monitoring system have been successfully completed on the dotted PCB. The turbidity sensor, pH sensor, and waterproof DS18B20 temperature sensor have been connected to the ESP32, preparing the system for the next stage of testing, calibration, and integration with the AquaSync dashboard.

![AquaSync Sensor Connections](images/aquasync_3.jpeg)

![AquaSync Dotted PCB Soldering](images/aquasync_4.jpeg)

**Total time spent:** 3 hours and 45 minutes

---
# October 29: Completed Relay Integration, Automatic Pump Control, and OLED Display

The relay module has been successfully integrated into the AquaSync water-quality monitoring system. The Arduino code for automatic turbidity-based pump control has been completed, allowing the submersible pump to turn ON when the water becomes very cloudy and turn OFF when the water is clear.

The OLED display has also been successfully integrated with the ESP32 to display the water-quality sensor readings, including pH, water temperature, and turbidity. The display layout has been adjusted to show the readings in the working lower portion of the OLED screen.

These improvements complete the implementation of the automatic pump control logic and local OLED monitoring, preparing AquaSync for further testing, calibration, and dashboard integration.

![AquaSync Relay Module Integration](images/aquasync_5.jpeg)

![AquaSync Automatic Pump Control](images/aquasync_6.jpeg)

**Total time spent:** 6 hours

---

# October 29: Completed Water Pump Wiring, Servo Motor Mounting, and Control Integration

The hardware integration of the AgriBot fertilizer spraying system has progressed significantly. The wiring for both water pumps and their relay modules has been completed, and the components have been mounted in their appropriate positions on the robot chassis. Two servo motors have also been installed to support the spraying mechanism, with their wiring connected to the ESP32 controller.

The Arduino code has been implemented to control both water pumps and servo motors through the MQTT-based web dashboard. Each pump is controlled independently through its corresponding relay, while the associated servo motor moves to its configured position when the pump is switched ON or OFF. The ESP32 also handles the robot's wheel movement commands, including forward, backward, left, right, and stop, through MQTT communication.

A motor safety timeout has also been implemented to stop the wheel motors when movement commands are no longer received within the configured time limit. With the pump wiring, servo mounting, and control code integrated, the AgriBot is ready for further testing of the fertilizer spraying mechanism and complete system operation.

![AgriBot Water Pump Wiring](images/agribot_5.jpeg)

![AgriBot Servo Motor Installation](images/agribot_6.jpeg)



**Total time spent:** 8 hours

---

# November 1: Completed MQTT Motor Testing and Security-Based Aquarium Pump Integration

The AgriBot motor control system was successfully tested, confirming that the robot's wheel motors could be operated through the MQTT-based web dashboard. Following the successful test, work continued on integrating the aquarium pump control outputs into the existing ESP32 controller while maintaining the keypad-based security system.

During the integration process, an issue occurred with the modified ESP32 code. Although the original motor-control code had worked correctly, the motors stopped responding after the security system was integrated. Initial testing confirmed that MQTT messages were reaching the ESP32, but the movement commands were not always being executed. This required debugging the MQTT message handling, command recognition, and security authorization logic.

One of the main difficulties was related to the security communication between the Arduino Uno and ESP32. The ESP32 required continuous authorization messages from the Uno to maintain the unlocked state. When the authorization heartbeat was interrupted, the system entered a locked state and rejected movement commands. The security logic was therefore improved to handle heartbeat interruptions more reliably. MQTT command processing was also improved to handle unwanted spaces and newline characters in incoming messages.

After making the necessary corrections, the AgriBot motor test was successfully completed again, confirming that the existing movement controls were functioning correctly alongside the keypad security system.

The next stage involved integrating the aquarium pump outputs into the same ESP32 controller. Two relay outputs were assigned for independent pump control: `PUMP1_RELAY` on GPIO 32 and `PUMP2_RELAY` on GPIO 33. Both outputs are controlled through the existing MQTT communication system using the topics `agribot/motor1` and `agribot/motor2`.

An additional security condition was implemented to ensure that both aquarium pumps can only be switched ON when the robot is unlocked through the keypad. Whenever the system is locked or the security communication is interrupted, both pumps are automatically switched OFF. OFF commands remain available even while the system is locked to ensure that the pumps can always be stopped.

With these improvements, the AgriBot movement controls, aquarium pump relay outputs, MQTT communication, and keypad-based security system are integrated into a common ESP32 control system. This provides a more secure and coordinated control structure for the FloraSync platform.

## Challenges and Difficulties Faced

- Motor commands stopped working after integrating the keypad security system.
- Debugging MQTT message reception and movement command recognition.
- Resolving authorization heartbeat interruptions between the Arduino Uno and ESP32.
- Correcting the security logic so that the system could recover from temporary communication interruptions.
- Ensuring that motor movement and aquarium pump commands were blocked whenever the system was locked.
- Maintaining the existing motor, pump, servo, and MQTT functionality without disrupting previously completed features.

## Final Status

**AgriBot motor testing:** Successfully completed.

**Aquarium pump integration:** Integrated with the ESP32 through two relay outputs.

**Security integration:** Both aquarium pumps are protected by the keypad-based authentication system.

**MQTT communication:** Used for independent pump and robot movement control.

**Total time spent:** 10 hours


![AgriBot Motor Testing](images/lockingsystem1.jpeg)

![Aquarium Pump Relay Integration](images/lockingsystem2.jpeg)

---


# November 3: Completed FloraSync Outer Enclosure Construction and Exterior Finishing

The outer covering and physical enclosure construction of the FloraSync smart farming system were successfully completed. This development marks an important step in transforming the previously assembled electronic components into a structured and organized physical prototype.

The main objective of this stage was to construct an external protective enclosure for the FloraSync system, providing a dedicated space for the electronic components, display unit, power connections, and other integrated hardware while improving the overall appearance and organization of the prototype.

During the construction process, the enclosure was assembled using lightweight sheet materials. The individual panels were measured, cut, and joined together to create the main box-shaped structure. Additional structural supports were added along the edges to improve the stability of the enclosure and maintain its overall shape.

The exterior surfaces were painted green to represent the agricultural theme of FloraSync. Red-colored finishing strips were added along the upper edges to create a contrasting appearance and improve the visual presentation of the prototype. A black rectangular panel was also fitted across the top section of the enclosure.

The front panel was prepared to accommodate the LCD display, which is used to display system-related information. The FloraSync name was also added to the front surface to establish the project's identity. The LCD display was successfully positioned within the front enclosure, providing an integrated interface for displaying messages such as the system startup status.

Additional openings were created on the side panels to accommodate necessary hardware connections, wiring, and external components. The water pump was also positioned alongside the enclosure as part of the existing hardware arrangement.

During this stage, attention was given to joining the individual panels, applying adhesive, securing the edges, and ensuring that the enclosure could accommodate the existing electronic hardware without requiring major modifications to the internal arrangement.

With the completion of the outer covering, FloraSync now has a dedicated physical enclosure that provides a more organized appearance and a foundation for the remaining hardware integration work.

## Major Construction Activities Completed

**Outer Enclosure Construction:**

- Assembled the main box-shaped outer structure.
- Measured and fitted the individual enclosure panels.
- Joined the panels using adhesive.
- Added structural supports along the enclosure edges.
- Prepared openings for hardware connections and wiring.

**Exterior Design and Finishing:**

- Applied green paint to the external surfaces.
- Added red-colored finishing strips along the upper edges.
- Installed the black top panel.
- Added the FloraSync project name to the front surface.
- Completed the initial exterior finishing and visual presentation.

**Display and Hardware Arrangement:**

- Prepared the front panel for LCD installation.
- Integrated the LCD display into the front enclosure.
- Verified the placement of the display within the front panel.
- Positioned the water pump alongside the enclosure.
- Accommodated openings for external wiring and hardware connections.

## Challenges and Difficulties Faced

- Measuring and cutting the enclosure panels accurately.
- Maintaining proper alignment while joining the individual panels.
- Securing the edges and corners to improve structural stability.
- Achieving a reasonably consistent exterior finish using paint and adhesive.
- Creating suitable openings for the LCD display and external hardware connections.
- Arranging the external components without interfering with the enclosure structure.
- Managing the available internal space for the existing electronic components and wiring.

## Final Status

**Outer Enclosure Construction:** Successfully completed.

**Main Structural Assembly:** Completed.

**Green Exterior Painting:** Completed.

**Red Edge Finishing:** Completed.

**Black Top Panel Installation:** Completed.

**FloraSync Front Branding:** Completed.

**LCD Display Integration:** Physically installed.

**External Hardware Openings:** Prepared.

**Water Pump Placement:** Completed.

**Internal Component Organization:** Further arrangement and securing required.

**Final Wiring Management:** Pending.

**Enclosure Protection and Sealing:** Further improvements required.

**Next Development Stage:** Organize and secure the internal electronic components, improve cable management, complete the remaining enclosure finishing, and conduct a final hardware integration test of the FloraSync system.


![FloraSync Completed Outer Enclosure - Front View](images/outercovering1.jpeg)

![FloraSync Completed Outer Enclosure - Side View](images/outercovering2.jpeg)

![FloraSync Completed Outer Enclosure - Top View](images/outercovering3.jpeg)


**Total time spent:** [Add your working hours]

---
# November 4: Completed ESP32 and NRF24L01+PA+LNA Initial Integration and Hardware Testing

The development of the AgriBot wireless joystick control system was initiated to establish a dedicated wireless communication system using ESP32 and NRF24L01+PA+LNA modules. The main objective is to replace the previous MQTT-based movement control approach with a direct wireless joystick controller, providing more responsive and independent control over the AgriBot movement and spraying mechanisms.

The proposed system consists of two ESP32 boards communicating through NRF24L01 wireless transceiver modules. The first ESP32 acts as the transmitter in the handheld controller, while the second ESP32 will act as the receiver installed on AgriBot.

The handheld controller is planned to include two joysticks and multiple physical buttons. The first joystick will be dedicated to robot movement, providing forward, reverse, left, and right controls. The second joystick will control the spraying mechanism, allowing adjustment of the spraying height and nozzle direction.

During the initial hardware integration, the NRF24L01+PA+LNA module was connected to the ESP32 30-pin DevKit. The SPI communication pins were configured along with the CE and CSN control pins. Since GPIO 16 and GPIO 17 were unavailable on the selected ESP32 board, alternative GPIO pins were assigned for the NRF24L01 connections.

The RF24 library by TMRh20 was installed in the Arduino IDE, and an initial hardware detection program was uploaded to the ESP32. The Serial Monitor successfully displayed the message `NRF24 DETECTED!`, confirming that the ESP32 could communicate with the NRF24L01 module through the SPI interface.

The module configuration was also successfully read through the Serial Monitor, displaying a data rate of 250 KBPS, 16-bit CRC, and PA_LOW transmission power configuration. This confirmed that the initial hardware connections and SPI communication were functioning correctly.

An initial power test was also performed by powering the NRF24L01 module through the ESP32's 3.3V output. The module was successfully detected during this test. However, further stability and wireless transmission testing are still required before finalizing the power arrangement for the PA+LNA modules.

The second ESP32 receiver has not yet been connected to the AgriBot hardware. The next stage will involve connecting the second NRF24L01 module, establishing wireless communication between both ESP32 boards, and testing reliable data transmission before integrating the motor driver, servos, and fertilizer pump controls.

## Planned Joystick and Button Control Architecture

**Joystick 1 – Movement Control:**

- Forward and reverse movement.
- Left and right steering.
- Dedicated movement enable control.

**Joystick 2 – Spraying System Control:**

- Raising and lowering the spraying mechanism.
- Adjusting the spray nozzle direction.
- Dedicated spraying pump control.

**Additional Physical Buttons:**

- Fertilizer pump ON/OFF control.
- Camera control.
- Headlight control.
- Warning buzzer control.
- Emergency stop.

## Challenges and Difficulties Faced

- Selecting suitable GPIO pins for the NRF24L01 module on the 30-pin ESP32 DevKit.
- Establishing the correct SPI communication connections between the ESP32 and NRF24L01.
- Configuring the RF24 library and verifying successful hardware detection.
- Understanding the power requirements of the NRF24L01+PA+LNA module.
- Planning the transmitter and receiver architecture for independent joystick-based movement and spraying control.
- Ensuring reliable wireless communication before integrating the existing AgriBot hardware.

## Final Status

**ESP32 Transmitter Integration:** Successfully connected and tested.

**NRF24L01 Hardware Detection:** Successfully completed.

**SPI Communication:** Successfully established.

**RF24 Library Configuration:** Completed.

**NRF24L01 Power Testing:** Initial detection through ESP32 3.3V output successful; further stability testing pending.

**Second ESP32 Receiver:** Not yet connected.

**Wireless Communication:** Pending transmitter-to-receiver testing.

**Joystick Control System:** Architecture planned; implementation pending.

**AgriBot Hardware Integration:** Pending wireless communication testing.

**Next Development Stage:** Connect the second ESP32 and NRF24L01 module, establish wireless communication, and begin implementing the dual-joystick control system.


![NRF24L01 ESP32 Hardware Testing](images/nrf1.jpeg)
![NRF24L01 ESP32 Hardware Testing](images/nrf2.jpeg)
![NRF24L01 ESP32 Hardware Testing](images/nrf3.jpeg)



**Total time spent:** 6 hours
---
