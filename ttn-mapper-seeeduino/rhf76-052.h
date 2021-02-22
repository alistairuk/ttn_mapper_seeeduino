/*
 * RHF76-052 Basic interface library
 *
 * Developed for the Seeeduino LoRa
 * Tested with firmware version 2.1.19
 * Very much work in progress
 *
 * Alistair MacDonald 20201
 *
 */


#define SerialLoRa Serial1

#define DEFAULT_LORA_READ_TIMEOUT 1000

// Return if the respoce was positive or negative
// Note that RHF76-052 failiours are treated as positive
int loraIsPositiveResponce( String inResponce ) {
  return ( inResponce.charAt(0) == '+' );
}

// Return the trailing data only if the respoce was positive
String loraResponceIfPositive( String inResponce ) {
  if ( inResponce.charAt(0) == '+' ) {
    int startPos = inResponce.indexOf( ' ' ) + 1;
    return inResponce.substring( startPos );
  }
  else {
    return "";
  }
}

// Clear out any queued data coming from the module
void loraClearBuffer() {
  while ( SerialLoRa.available() > 0 ) {
    SerialLoRa.read();
  }
}

// Read one line for the module
String loraReadBuffer( unsigned long inTimeout = DEFAULT_LORA_READ_TIMEOUT ) {

  String result = "";
  char theChar;
  char done = false;

  unsigned long startTime = millis();

  while ( ( !done ) && ( ( millis() - startTime ) < inTimeout ) ) {
    while ( SerialLoRa.available() > 0 ) {
      theChar = SerialLoRa.read();
      if ( theChar == '\n' ) {
        done = true;
      }
      else if ( theChar != '\r' ) {
        result += theChar;
      }
    }
    delay(1);
  }

  return result;
}

// Send an AT command to the module and return one line of the result
String loraSendCommand( String inCommand, unsigned long inTimeout = DEFAULT_LORA_READ_TIMEOUT ) {
  loraClearBuffer();
  SerialLoRa.println( inCommand );
  return loraReadBuffer( inTimeout );
}

// Initialise the LoRa module - Shoud be called before use
int loraReset() {
  SerialLoRa.begin( 9600 );
  String result = loraSendCommand( "AT+RESET" );
  delay(1000);
  return loraIsPositiveResponce( result );
}

// The general functions mirriring those in the RHF76-052 user manual

String loraGetVersion() {
  String responce = loraSendCommand( "AT+VER=?" );
  if ( loraIsPositiveResponce( responce ) ) {
    return responce.substring( 6 );
  }
  else {
    return "";
  }
}

String loraGetDevAddr() {
  String responce = loraSendCommand( "AT+ID=?" );
  loraReadBuffer();
  loraReadBuffer();
  return loraResponceIfPositive( responce ).substring( 9 );
}

String loraGetDevEui() {
  loraSendCommand( "AT+ID=?" );
  String responce = loraReadBuffer();
  loraReadBuffer();
  return loraResponceIfPositive( responce ).substring( 8 );
}

String loraGetAppEui() {
  loraSendCommand( "AT+ID=?" );
  loraReadBuffer();
  String responce = loraReadBuffer();
  return loraResponceIfPositive( responce ).substring( 8 );
}



int loraSetDevAddr( String inDevAddr ) {
  String responce = loraSendCommand( "AT+ID=DevAddr,\"" + inDevAddr + "\"" );
  return loraIsPositiveResponce( responce );
}    

int loraSetDevEUI( String inDevEUI ) {
  String responce = loraSendCommand( "AT+ID=DevEUI,\"" + inDevEUI + "\"" );
  return loraIsPositiveResponce( responce );
}    
    
int loraSetAppEUI( String inAppEUI ) {
  String responce = loraSendCommand( "AT+ID=AppEUI,\"" + inAppEUI + "\"" );
  return loraIsPositiveResponce( responce );
}



int loraSetNwkSKey( String inNwkSKey ) {
  String responce = loraSendCommand( "AT+KEY=NWKSKEY,\"" + inNwkSKey + "\"" );
  return loraIsPositiveResponce( responce );
}

int loraSetAppSKey( String inAppSKey ) {
  String responce = loraSendCommand( "AT+KEY=APPSKEY,\"" + inAppSKey + "\"" );
  return loraIsPositiveResponce( responce );
}    

int loraSetAppKey( String inAppKey ) {
  String responce = loraSendCommand( "AT+KEY=APPKEY,\"" + inAppKey + "\"" );
  return loraIsPositiveResponce( responce );
}    

int loraSetMode( String inMode ) {
  String responce = loraSendCommand( "AT+MODE=" + inMode );
  return loraIsPositiveResponce( responce );
}    

int loraSetModeABP() {
  return loraSetMode( "LWABP" );
}

int loraSetModeOTAA() {
  return loraSetMode( "LWOTAA" );
}

int loraSetDataRate( String inDataRate ) {
  String responce = loraSendCommand( "AT+DR=" + inDataRate );
  return loraIsPositiveResponce( responce );
}

int loraSetChannel( int inChannel, float inFrequency ) {
  String responce = loraSendCommand( "AT+CH=" + String( inChannel ) + "," + String( inFrequency ) );
  return loraIsPositiveResponce( responce );
}

int loraSetReceiceWindowFirst( int inChannel, float inFrequency ) {
  String responce = loraSendCommand( "AT+RXWIN1=" + String(inChannel) + "," + String(inFrequency) );
  return loraIsPositiveResponce( responce );
}

int loraSetReceiceWindowSecond( float inFrequency, int inDatarate) {
  String responce = loraSendCommand( "AT+RXWIN2=" + String(inFrequency) + "," + String(inDatarate) );
  return loraIsPositiveResponce( responce );
}

int loraSetDutyCycle( int inState ) {
  String responce = loraSendCommand( "AT+LW=DC, " + ( inState ) ? "ON" : "OFF" );
  return loraIsPositiveResponce( responce );
}

int loraSetJoinDutyCycle( int inState ) {
  String responce = loraSendCommand( "AT+LW=JDC," + ( inState ) ? "ON" : "OFF" );
  return loraIsPositiveResponce( responce );
}

int loraSetPower( int inPower ) {
  String responce = loraSendCommand( "AT+POWER=" + inPower );
  return loraIsPositiveResponce( responce );
}

int loraJoin( unsigned long inTimeout = -1) {

  unsigned long startTime = millis();
  String responce = loraSendCommand( "AT+JOIN" );

  while ( ( millis() - startTime ) < inTimeout  ) {

    if ( responce.startsWith( "+JOIN: Join failed" ) ) {
      delay( random(4000) );
      responce = loraSendCommand( "AT+JOIN" );
    }

    if ( responce.startsWith( "+JOIN: Network joined" ) ) {
      return true;
    }

    responce = loraReadBuffer();

  }

  return false;
}

int loraTransferPacket( String inString ) {
  String hexString = "";
  char nextChar;
  
  for ( int loopit=0; loopit<inString.length(); loopit++ ) {
    nextChar = inString.charAt(loopit);
    if ( nextChar < 0x10 ) { hexString += "0"; }
    hexString += String( nextChar, HEX ) + " ";
  }

  hexString.trim();
  
  String responce = loraSendCommand("AT+MSGHEX=\"" + hexString + "\"");

  return loraIsPositiveResponce( responce );
}

int loraTransferPacket( char* inBuffer, int inBufferSize ) {
  String hexString = "";
  char nextChar;
  
  for ( int loopit=0; loopit<inBufferSize; loopit++ ) {
    nextChar = inBuffer[loopit];
    if ( nextChar < 0x10 ) { hexString += "0"; }
    hexString += String( nextChar, HEX ) + " ";
  }

  hexString.trim();
  
  String responce = loraSendCommand("AT+MSGHEX=\"" + hexString + "\"");

  return loraIsPositiveResponce( responce );
}
