# TTN Mapper Seeeduino LoRaWAN Tracker

## Overview

This is firmware I coded for the [Seeeduino LoRaWAN (with GPS)](https://www.seeedstudio.com/Seeeduino-LoRaWAN-W-GPS-p-2781.html) to help build up the [TTN Mapper](https://ttnmapper.org/) map automatically. It uses the built GPS and sends the location over the LoRa. No extra hardware or app is needed. Just plug it in, wait for it to activate and get a GPS lock, then move around and build up the map.

Please note that this is a hacked together pet project and not a solid base for an industrial project. I publish this more as a way of sharing the knowledge than a finished project. If you build on this them please consider sharing it (really you should) and do let me know what you have done.

## Adding the device to The Things Network

These instructions have been updated for TTN V3 console.

Before stating it is worth testing your Seeeduino with the help of the code on the [Seeeduino LoRaWAN Wiki page](https://wiki.seeedstudio.com/Seeeduino_LoRAWAN/).

### Getting started

First go to your Things Network [applications console](https://console.thethingsnetwork.org/applications). You will need to create an account if you do not have one already.

### Adding the application

Click the blue "Add Application" button
Enter an Application ID (It can be anything but must be universally unique. A random string is good)
Click the "Create application" button

### Adding the payload format decoder

Click "Payload Formats" from the left menu bar, and then click "custom" from the expanded options.

From the "Format Type" selector select "Javascript"

Paste the following function in to the edit field replacing anything already in there.

```javascript
function decodeUplink(input) {
  // Decode an uplink message from a buffer
  // (array) of input.bytes to an object of fields.
  var decoded = {};
  // if (port === 1) decoded.led = input.bytes[0];
  decoded.latitude = ((input.bytes[0]<<16)>>>0) + ((input.bytes[1]<<8)>>>0) + input.bytes[2];
  decoded.latitude = (decoded.latitude / 16777215.0 * 180) - 90;
  decoded.longitude = ((input.bytes[3]<<16)>>>0) + ((input.bytes[4]<<8)>>>0) + input.bytes[5];
  decoded.longitude = (decoded.longitude / 16777215.0 * 360) - 180;
  var altValue = ((input.bytes[6]<<8)>>>0) + input.bytes[7];
  var sign = input.bytes[6] & (1 << 7);
  if(sign)
  {
    decoded.altitude = 0xFFFF0000 | altValue;
  }
  else
  {
    decoded.altitude = altValue;
  }
  decoded.hdop = input.bytes[8] / 10.0;
  return {
    data: decoded,
    warnings: [],
    errors: []
  };
}
```

This encoding format is compatible with some other trackers.

Click the "Save changes" button

### Adding the TTN Mapper Integration

Click "Integrations" from the left menu bar, and then click "Webhooks" from the expanded options.
Click the blue "Add webhook" button
Click TTN Mapper icon	
Enter a Webhook ID (it can be anything)
Enter your E-mail address
Leave "Experiment name" blank or things will go wrong
Click the "Create ttn mapper webhook" button

### Adding your device

Click "Overview" from the left menu bar (not the top menu bar)
Click the blue "Add end device" button
Click "Manually" under the "Register end device" title
Select a Frequency plan ("Europe 863-870 MHz (SF9 for RX2 - recommended)" is normaly good)
Select a LoRaWAN vesrion (I presuming "MAC V1.0" going by feature specification)
Click the grey "Generate" button next to the DevEUI edit field
Click the grey "Fill with zeros" button next to the AppEUI edit field or enter a randon value
Click the grey "Generate" button next to the AppKey edit field
Enter an End device ID (This should be unique to you and will be visible on TTN Mapper)
Click the "Register end device" button

Now copy the AppEUI, DevEUI, and AppKey shown to the placeholders in the Arduino code's setup() function (just use the copy buttons to the right of the field)

### Uploading the firmware

Upload the code to the Seeeduio LoRa board using the Arduino application.

## Using the tracker

It will take a few minutes for the tracker to start.

At fires only the one green LED (the power LED) will be illuminated. This is because the device is trying to activate on the TTN network. The Yellow LED will briefly flash every time a request is sent.

Next the second green LED will light constantly. This indicates the tracking has activated on the LoRaWAN network and is continuing to look for a GPS signal. It was actually doing this all along but normally takes a while.

Then the second green LED starts to flash. This indicates everything is working and it is updating TTN Mapper. The green LED will stay illuminated for a couple of seconds before each update is sent.
