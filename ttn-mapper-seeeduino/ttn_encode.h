/*
 * TTN Mapper data encoder
 *
 * Encode the location data for sending over LoRa to TTN for TTN Mappper
 * Built to be compatable with JP Meijers's TheThingsUno GPSshiel TTN Mapper library
 *
 * Alistair MacDonald 20201
 *
 */


/*

// JavaScript for the TTN console
// From https://github.com/jpmeijers/RN2483-Arduino-Library/blob/master/examples/TheThingsUno-GPSshield-TTN-Mapper-binary/TheThingsUno-GPSshield-TTN-Mapper-binary.ino

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

*/

// A buffer of at least 9 bytes is required
void ttnEncode( char *outBuffer, double inLat, double inLong, float inAlt, float inHDOP ) {
  
  unsigned long latitude = ( ( inLat + 90 ) / 180.0 ) * 16777215;
  unsigned long longitude = ( ( inLong + 180 ) / 360.0 ) * 16777215;
  
  // Latitude
  outBuffer[0] = ( latitude >> 16 ) & 0xFF;
  outBuffer[1] = ( latitude >> 8 ) & 0xFF;
  outBuffer[2] = latitude & 0xFF;
  
  // Longitude
  outBuffer[3] = ( longitude >> 16 ) & 0xFF;
  outBuffer[4] = ( longitude >> 8 ) & 0xFF;
  outBuffer[5] = longitude & 0xFF;
  
  // Altitude
  int alt = round(inAlt);
  outBuffer[6] = ( alt >> 8 ) & 0xFF;
  outBuffer[7] = alt & 0xFF;
  
  // Horizontal Dilution of Precision
  outBuffer[8] = round( inHDOP * 10 ) & 0xFF;
  
}
