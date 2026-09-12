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

**Total time spent:** 2 hours 30 minutes

---

# September 12, 3:00 PM: Worked on Raspberry Pi

Continued development and testing of the Raspberry Pi-based animal intrusion alert system as part of the FloraSync project. The Raspberry Pi 3 Model B was prepared for a headless setup so that it could be accessed from a laptop over Wi-Fi without requiring a dedicated monitor.

The Raspberry Pi OS was written to the microSD card, and the Raspberry Pi was powered on for testing. The power and activity LEDs indicated that the Raspberry Pi was receiving power and accessing the SD card.

During the setup, however, the SD card encountered several issues. Raspberry Pi Imager reported that verification of the written data failed because the contents of the SD card differed from what was written. Attempts to format the card through Windows also failed. DiskPart detected the SD card as a 31 GB removable disk, but attempting to clean the disk resulted in a **"The device is not ready"** error.

As a result, the Raspberry Pi could not yet be successfully configured for the planned Wi-Fi-based headless connection. Further testing of the SD card and card reader is required before continuing with the Raspberry Pi setup.

![Raspberry Pi SD Card Setup Failure](images/raspberry-pi-sd-card-failure.jpeg)

**Progress:** 🚧 In Progress

**Issue:** SD card write and verification failure

**Next step:** Test the SD card/card reader and successfully install Raspberry Pi OS before continuing with the Wi-Fi and headless Raspberry Pi setup.
