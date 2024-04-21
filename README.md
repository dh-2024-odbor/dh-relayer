# Hopper relayed

The entry point of relayer firmware is in the `main.c` file. 
It includes an `app_main` function which is the first function that gets called when the device boots up.

It contains initialization code for LoRa radio and tasks for receiving and sending LoRa packets.

The `lora.c` file contains initialization code for the LoRa radio and deduplication logic.

The firmware is developed using ESP-IDF v5.1-dirty (stable) (Espressif IoT Development Framework) which is a standard for the ESP32 series of microcontrollers that we use.

The source code is well documented, so if you feel an unsatiable need for reading code feel free to do so.

There is a `components` folder that includes the driver code for LoRa device.

Finally the `sdkconfig` file includes all the configuration for the esp32 device. Some of this information is included in the bootloader binary to make the microcontroller function properly, some is used merely as configuration for interfacing with the device via serial.

## Overview

This is the LoRa relayer device functional overview.

This device could be also understoond as a range extender with some added deduplication logic.

Its purpose is to bridge distances too large for normal LoRa transmissions to handle. In production environments it would be placed between the sender and the gateway and its only purpose is to relay sender packets forward.

It is also possible to put multiple relayers in between senders and the gateway to increase distance traveled even further.

Deduplication logic is fairly simple. It relies on our custom packet header which includes the node and message ids which are guaranteed to be unique when put together. Relayer that keeps last 100 headers it has seen in memory and will ignore packets with headers it has already seen. This effectively ensures that every relay transmits a forward packet only once and prevents infinite ping-pong matches between relays.

If you want a better description of LoRa radio operation see the Hopper Sender (dh-sender) repository 