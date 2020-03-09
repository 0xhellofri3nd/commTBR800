#include <MsTimer2.h>
#include <AltSoftSerial.h>
#include <stdio.h>
#define Wreset 10

int index  = 0;

byte sb;   
byte pkgRead[400];      
byte pkgWrite[ ] = {0x05,0x64,0x0b,0xc4,0x03,0x00,0x01,0x00,0x6b,0xe6,0xc0,0xc0,0x01,0x3c,0x01,0x06,0xff,0x50};

String VariavelGlobal;
String numHex0 = "";
String numHex1 = "";
String numHex2 = "";
String numHex3 = "";
String numHex4 = "";
String numHex5 = "";
String numHex6 = "";
String numHex7 = "";
String numHex8 = "";
String numHex9 = "";
String numHex10 = "";
String numHex11 = "";

AltSoftSerial mySerial;
String rx_wisol;
String Sigfox = "";

void setup () {
    Serial.begin(9600);
    MsTimer2::set( 10000, requestData ); 
    MsTimer2::start();

    pinMode(Wreset, OUTPUT);           
    digitalWrite(Wreset, HIGH);        
    mySerial.begin(9600);
    delay(1000);
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
    if ( index >= 246 ) {
        sendToSigfox();
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
void sendToSigfox() {  
    delay(100);
    convertVariables();
    String Sigfox = "";
    mySerial.println("AT");
    mySerial.println("AT$RC");
    Sigfox = "AT$SF=";
    delay(100);
    Serial.println("Enviando para nuvem : " + VariavelGlobal);
    Sigfox += VariavelGlobal;
    mySerial.println(Sigfox);
    Serial.println( "***************************************************" );
    Serial.println("numHex0: " + numHex0);
    Serial.println("numHex1: " + numHex1);
    Serial.println("numHex2: " + numHex2);
    Serial.println("numHex3: " + numHex3);
    Serial.println("numHex4: " + numHex4);
    Serial.println("numHex5: " + numHex5);
    Serial.println("numHex6: " + numHex6);
    Serial.println("numHex7: " + numHex7);
    Serial.println("numHex8: " + numHex8);
    Serial.println("numHex9: " + numHex9);
    Serial.println("numHex10: " + numHex10);
    Serial.println("numHex11: " + numHex11);
    Serial.println( "***************************************************" );
}

void  convertVariables() {
    //Serial.print( "VLC = " ); tensao lado da carga
    numHex0 = String(( convert16bits( pkgRead[32], pkgRead[31] ) ), HEX);
    //Serial.print( "ILC = " ); corrente lado da carga
    numHex1 = String(( convert16bits( pkgRead[35], pkgRead[34] ) ), HEX);
    //Serial.print( "VLF = " ); tensao lado da fonte
    numHex2 = String(( convert16bits( pkgRead[38], pkgRead[37] ) ), HEX);
    //Serial.print( "ILF = " ); corrente lado da fonte
    numHex3 = String(( convert16bits( pkgRead[41], pkgRead[40] ) ), HEX);
    //Serial.print( "VLLC = " ); tensao na linha
    numHex4 = String(( convert16bits( pkgRead[46], pkgRead[43] ) ), HEX);
    //Serial.print( "ILLC = " ); corrente na linha
    numHex5 = String(( convert16bits( pkgRead[49], pkgRead[48] ) ), HEX);
    //Serial.print( "Tap Atual = " );
    numHex6 = String(( convert16bits( pkgRead[115], pkgRead[114] ) ), HEX);
    //Serial.print( "Memória tap mínimo = " );
    numHex7 = String(( convert16bits( pkgRead[120], pkgRead[119] ) ), HEX);   
    //Serial.print( "Memória tap máximo = " );
    numHex8 = String(( convert16bits( pkgRead[140], pkgRead[139] ) ), HEX);
    numHex9 = "00";
    numHex10 = "00";
    numHex11 = "00";

    switch ( numHex0.length() ) {
        case 1:
            numHex0 = "0" + numHex0;
            break;
        case 2:
            break;
    }

    switch ( numHex1.length() ) {
        case 1:
            numHex1 = "0" + numHex1;
            break;
        case 2:
            break;
    }

    switch ( numHex2.length() ) {
        case 1:
            numHex2 = "0" + numHex2;
            break;
        case 2:
            break;
    }

    switch ( numHex3.length() ) {
        case 1:
            numHex3 = "0" + numHex3;
            break;
        case 2:
            break;
    }

    switch ( numHex4.length() ) {
        case 1:
            numHex4 = "0" + numHex4;
            break;
        case 2:
            break;
    }

     switch ( numHex5.length() ) {
        case 1:
            numHex5 = "0" + numHex5;
            break;
        case 2:
            break;
    }

    switch ( numHex6.length() ) {
        case 1:
            numHex6 = "0" + numHex6;
            break;
        case 2:
            break;
    }

    switch ( numHex7.length() ) {
        case 1:
            numHex7 = "0" + numHex7;
            break;
        case 2:
            break;
    }

    switch ( numHex8.length() ) {
        case 1:
            numHex8 = "0" + numHex8;
            break;
        case 2:
            break;
    }

    switch ( numHex9.length() ) {
        case 1:
            numHex9 = "0" + numHex9;
            break;
        case 2:
            break;
    }

  switch ( numHex10.length() ) {
        case 1:
            numHex10 = "0" + numHex10;
            break;
        case 2:
            break;
    }

    switch ( numHex11.length() ) {
        case 1:
            numHex11 = "0" + numHex11;
            break;
        case 2:
            break;
    }

    VariavelGlobal = numHex0 + numHex1 + numHex2 + numHex3 + numHex4 + numHex5 + numHex6 + numHex7 + numHex8 + numHex9 + numHex10 + numHex11;
}

void enviaSGF() {
    String Sigfox = "";
    ATcmd();                         
    ATRC();                       
    delay(10);                      
    ATcmd();   
    Sigfox = "AT$SF=";
    Sigfox += pkgRead[32];
}

void ATcmd() {
    mySerial.println("AT");// writing AT command
    delay(100);
}

void ATRC() {
    mySerial.println("AT$RC");// writing AT command
    delay(100);
}

void loop() {

}
