# TTN Mapper Seeeduino LoRaWAN Tracker

## Overview

This is firmware I coded for the [Seeeduino LoRaWAN (with GPS)](https://www.seeedstudio.com/Seeeduino-LoRaWAN-W-GPS-p-2781.html) to help build up the [TTN Mapper](https://ttnmapper.org/) map automatically. It uses the built GPS and sends the location over the LoRa. No extra hardware or app is needed. Just plug it in, wait for it to activate and get a GPS lock, then move around and build up the map.

Please note that this is a hacked together pet project and not a solid base for an industrial project. I publish this more as a way of sharing the knowledge than a finished project. If you build on this them please consider sharing it (really you should) and do let me know what you have done.

## Adding the device to The Things Network

These instructions are for TTN V2 as at the time of creation TTN Mapper is not yet compatible with TTN V3. This will no doubt change in time.

Before stating it is worth testing your Seeeduino with the help of the code on the [Seeeduino LoRaWAN Wiki page](https://wiki.seeedstudio.com/Seeeduino_LoRAWAN/).

### Getting started

First go to your Things Network [applications console](https://console.thethingsnetwork.org/applications). You will need to create an account if you do not have one already.

### Adding the application

Click "Add Application" (Green + to the right on the Applications section title)
Enter an Application ID (It can be anything but must be universally unique. A random string is good)
Click the "Add application" button

### Adding the payload format decoder

Click "Payload Formats" from the menu bar
"Custom" and the "Decoder" tab should already be selected

Paste the following function in to the edit field replacing anything already in there.

```javascript
function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};
  // if (port === 1) decoded.led = bytes[0];
  decoded.lat = ((bytes[0]<<16)>>>0) + ((bytes[1]<<8)>>>0) + bytes[2];
  decoded.lat = (decoded.lat / 16777215.0 * 180) - 90;
  decoded.lon = ((bytes[3]<<16)>>>0) + ((bytes[4]<<8)>>>0) + bytes[5];
  decoded.lon = (decoded.lon / 16777215.0 * 360) - 180;
  var altValue = ((bytes[6]<<8)>>>0) + bytes[7];
  var sign = bytes[6] & (1 << 7);
  if(sign)
  {
    decoded.alt = 0xFFFF0000 | altValue;
  }
  else
  {
    decoded.alt = altValue;
  }
  decoded.hdop = bytes[8] / 10.0;
  return decoded;
}
```

This encoding format is compatible with some other trackers.

Click the "Save the payload functions" button

### Adding the TTN Mapper Integration

Click "Integrations" from the menu bar
Click Add Integration (Grey + to the right on the Applications section title)
Click TTN Mapper icon	
Enter a Process ID (it can be anything)
Enter your E-mail address
Leave everything else blank
Click the "Add Integration" button

### Adding your device

Click "Overview" from the menu bar
Click Register Device (Green + to the right on the Devices section title)
Enter an Application ID (This should be unique to you and will be visible on TTN Mapper)
Click the generate Device EUI button (Crossed arrows to the left of the Device EUI field)
Click the "Register" button

Copy the Device EUI, Application EUI, and App Key now shown to the placeholders in the Arduino code's setup() function (just use the copy buttons to the right of the field)

### Uploading the firmware

Upload the code to the Seeeduio LoRa board using the Arduino application.

## Using the tracker

It will take a few minutes for the tracker to start.

At fires only the one green LED (the power LED) will be illuminated. This is because the device is trying to activate on the TTN network. The Yellow LED will briefly flash every time a request is sent.

Next the second green LED will light constantly. This indicates the tracking has activated on the LoRaWAN network and is continuing to look for a GPS signal. It was actually doing this all along but normally takes a while.

Then the second green LED starts to flash. This indicates everything is working and it is updating TTN Mapper. The green LED will stay illuminated for a couple of seconds before each update is sent.
