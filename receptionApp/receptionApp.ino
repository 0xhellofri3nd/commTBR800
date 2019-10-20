/*
 *  Correct reading of desired parameters of TBR800 Control
 *  
 *  Developed by: 
 *      Cleber Izidoro
 *      Marco Aurélio
 * 
 */
 
#include <MsTimer2.h>

int btn = 8;  
int aux = 0;
int index  = 0;

byte sb;   
byte pkgRead[400];      
byte pkgWrite[ ] = {0x05,0x64,0x0b,0xc4,0x03,0x00,0x01,0x00,0x6b,0xe6,0xc0,0xc0,0x01,0x3c,0x01,0x06,0xff,0x50};

void setup () {
    Serial.begin(9600);
    pinMode(btn, INPUT);
    MsTimer2::set( 5000, requestData ); // interruption
    MsTimer2::start();
}

// Convert data to 16-bit integer
int convert16bits ( char msb, char lsb ) {
  int convertData = ( int( msb * 256 ) + ( int( lsb ) ) );
  return convertData;
}
 
// Interruption to receive serial data
void serialEvent() { 
    while ( Serial.available() ) {
        sb = Serial.read();     
        pkgRead[ index ] = sb;
        index++;  
    }
    if ( index >= 246 ){
      showPkgRead();
      index = 0;
    }
}

// Read the Voltage Regulator values
void requestData() { 
    index = 0;
    Serial.println("Sending...");
    Serial.write( pkgWrite, sizeof( pkgWrite ) );
}

// Show received packet parameters
void showPkgRead() {  
    Serial.println( "***************************************************" );
    Serial.print( "VLC = " );
    Serial.println( convert16bits( pkgRead[32], pkgRead[31] ) );
    Serial.print( "ILC = " );
    Serial.println( convert16bits( pkgRead[35], pkgRead[34] ) );
    Serial.print( "VLF = " );
    Serial.println( convert16bits( pkgRead[38], pkgRead[37] ) );
    Serial.print( "ILF = " );
    Serial.println( convert16bits( pkgRead[41], pkgRead[40] ) );
    Serial.print( "VLLC = " );
    Serial.println(convert16bits( pkgRead[46], pkgRead[43] ) );
    Serial.print( "ILLC = " );
    Serial.println( convert16bits( pkgRead[49], pkgRead[48] ) );
    Serial.print( "Tap Atual = " );
    Serial.println( convert16bits( pkgRead[115], pkgRead[114] ) );
    Serial.print( "Memória tap mínimo = " );
    Serial.println( convert16bits( pkgRead[120], pkgRead[119] ) );    
    Serial.print( "Memória tap máximo = " );
    Serial.println( convert16bits( pkgRead[140], pkgRead[139] ) );
    Serial.print( "Freq = " );
    Serial.println( convert16bits( pkgRead[109], pkgRead[108] ) );
    
    Serial.print( "Hora = " );
    Serial.print( convert16bits( pkgRead[146], pkgRead[145] ) );
    Serial.print( ":" );
    Serial.println( convert16bits( pkgRead[143], pkgRead[142] ) );

    Serial.print( "Data = " );
    Serial.print( convert16bits( pkgRead[149], pkgRead[148] ) );
    Serial.print( "/" );
    Serial.print( convert16bits( pkgRead[154], pkgRead[151] ) );
    Serial.print( "/" );
    Serial.println( convert16bits( pkgRead[157], pkgRead[156] ) );
    Serial.println( "***************************************************" );
}

void loop() {

}
