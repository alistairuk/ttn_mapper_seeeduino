/*
 * TTN Mapper Client for the Seeeduino LoRa GPS
 *
 * Alistair MacDonald 20201
 *
 */

#include "rhf76-052.h"
#include "ttn_encode.h"

#define SerialGPS Serial2

#define FREQUENCY_DURATION 15000
#define READY_LED_DURATION 2000


void flushLoRa() {
  delay(100);
  while( SerialLoRa.available() ) {
    SerialLoRa.read();
  }
}

void setup(void) {
  // Setup USB serial and LED for debugging
  pinMode(LED_BUILTIN, OUTPUT);  
  SerialUSB.begin(115200);
  delay(2000);

  SerialUSB.println("Starting...");

  // Set up the GPS
  SerialGPS.begin(9600);
  SerialGPS.setTimeout(40);

  // Start the LoRa Module
  loraReset();
  // Set the LoRa keys for OTA activation
  loraSetDevEUI( "################" );
  loraSetAppEUI( "################" );
  loraSetAppKey( "################################" );
  // Set up the channels, datarates, dutycycle limits and power
  loraSetModeOTAA();
  loraSetDataRate( "EU868" );
  loraSetDataRate( "DR5" ); // SF7
  loraSetChannel( 0, 868.1 );
  loraSetChannel( 1, 868.3 );
  loraSetChannel( 2, 868.5 );
  loraSetChannel( 3, 867.1 );
  loraSetChannel( 4, 867.3 );
  loraSetChannel( 5, 867.5 );
  loraSetChannel( 6, 867.7 );
  loraSetChannel( 7, 867.9 );
  loraSetDutyCycle(false);
  loraSetJoinDutyCycle(false);
  loraSetPower(14); // 14 Maximum allowed for the UK/EU

  // Activate / loin the network
  SerialUSB.println( "Joining..." );
  randomSeed(analogRead(0));
  loraJoin();
  SerialUSB.println( "Joined" );
}

unsigned long lastSend = 0;
char txBuffer[9];
int  txBufferValid = false;
    
void loop(void) {   

  String newString;

  // Loop thought all the incoming DPS data
  while ( SerialGPS.available() ) {
    digitalWrite( LED_BUILTIN, HIGH );

    // Read one line (and skip any reaingin control chars)
    newString = SerialGPS.readStringUntil( '\r' );
    SerialGPS.readStringUntil( '\n' );

    // We are only interested in decoding the Global Positioning System Fix Data
    if ( newString.startsWith("$GPGGA") ) {

      // Locate where the elements of data are
      int startUTC    = newString.indexOf( ',' ) + 1;
      int startLat    = newString.indexOf( ',', startUTC ) + 1;
      int startLatNS  = newString.indexOf( ',', startLat ) + 1;
      int startLong   = newString.indexOf( ',', startLatNS ) + 1;
      int startLongEW = newString.indexOf( ',', startLong ) + 1;
      int startFix    = newString.indexOf( ',', startLongEW ) + 1;
      int startSats   = newString.indexOf( ',', startFix ) + 1;
      int startHDOP   = newString.indexOf( ',', startSats ) + 1;
      int startAlt    = newString.indexOf( ',', startHDOP ) + 1;
      int startFixed1 = newString.indexOf( ',', startAlt ) + 1;

      // Decode the elements in to values
      double gpsLat = newString.substring( startLat, startLat+2 ).toDouble() + ( newString.substring( startLat+2, startLatNS-1 ).toDouble() / 60 );
      if ( newString.charAt( startLatNS ) == 'S' ) { gpsLat = 0 - gpsLat; }
      double gpsLong = newString.substring( startLong, startLong+3 ).toDouble() + ( newString.substring( startLong+3, startLongEW-1 ).toDouble() / 60 );
      if ( newString.charAt( startLongEW ) == 'W' ) { gpsLong = 0 - gpsLong; }
      float gpsAlt = newString.substring( startAlt, startFixed1-1 ).toFloat();
      float gpsHDOP = newString.substring( startHDOP, startAlt-1 ).toFloat();

      // Is the data we have valid? Encode it for sending and store it if so.
      if ( newString.charAt( startFix ) != '0' ) {
        ttnEncode( txBuffer, gpsLat, gpsLong, gpsAlt, gpsHDOP );
        txBufferValid = true;
      }
      else {
        txBufferValid = false;
      }

    }
  }

  // Send the data ( if we have some and not sent recently )
  unsigned long now = millis();
  if ( ( txBufferValid ) && ( now-lastSend >= FREQUENCY_DURATION ) ) {
      SerialUSB.println( "Sending..." );
      loraTransferPacket( txBuffer, sizeof(txBuffer) );
      lastSend = now;
  }    

  digitalWrite( LED_BUILTIN, ( now-lastSend >= FREQUENCY_DURATION-READY_LED_DURATION ) );

}
