/*
* Portas Seriais
* GSM:    TX = 16  RX = 17
* SigFox: TX = 15  RX = 14
* RS232: TX = 1    RX = 0
*/

#include <MsTimer2.h>
#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <stdio.h>
#define Wreset 10

int index  = 0;

byte sb;   
byte pkgRead[400];      
byte pkgWrite[ ] = {0x05,0x64,0x0b,0xc4,0x01,0x00,0x01,0x00,0x6b,0xe6,0xc0,0xc0,0x01,0x3c,0x01,0x06,0xff,0x50};

String VariavelGlobal;
String numHex0, numHex1, numHex2, numHex3, numHex4, numHex5,
           numHex6, numHex7, numHex8, numHex9, numHex10, numHex11, var1, var2, var3 = "";
        
String msgGSM = "";

AltSoftSerial sigfoxSerial; 
SoftwareSerial gsmSerial(11, 10);



String rx_wisol;
String Sigfox = "";

void setup () {
    Serial.begin(9600);
    sigfoxSerial.begin(9600);
    gsmSerial.begin(4800);
    //MsTimer2::set( 600000, requestData ); 
    //MsTimer2::start();

    pinMode(Wreset, OUTPUT);           
    digitalWrite(Wreset, HIGH);        

    Serial.println("Initializing..."); 
    delay(5000);
    gsmSerial.println("AT+CREG?"); //Once the handshake test is successful, it will back to OK
    updateSerial();
    delay(2000);  
    gsmSerial.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();
    delay(2000);
    gsmSerial.println("AT+CNMI=2,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
    updateSerial();
    delay(2000);

}

// Convert data to 16-bit integer
int convert16bits ( char msb, char lsb ) {
    int convertData = ( int( msb * 256 ) + ( int( lsb ) ) );
    return convertData;
}
 
// Interruption to receive serial data
void serialEvent() {
    /*
    while ( Serial.available() ) {
        sb = Serial.read();
        pkgRead[ index ] = sb;
        index++;
    }
    if ( index >= 246 ) {
        sendToSigfox();
        index = 0;
    }
*/
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
    sigfoxSerial.println("AT");
    sigfoxSerial.println("AT$RC");
    Sigfox = "AT$SF=";
    delay(100);
    Serial.println("Enviando para nuvem : " + VariavelGlobal);
    Sigfox += VariavelGlobal;
    sigfoxSerial.println(Sigfox);
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

void convertVariables(){
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


void updateSerial() {
    delay(500);
    while (Serial.available()) {
        gsmSerial.write(Serial.read());
    }
  
    while(gsmSerial.available()) {
        Serial.write(gsmSerial.read());
    }

}

void receivedMSG() {
    delay(500);
    String comp = "";

    if (gsmSerial.available() > 0) {

        String msgGSM = "";
        
        while (gsmSerial.available() > 0) {
            char letra = gsmSerial.read();
            if ( isAlphaNumeric(letra) ){
                msgGSM += letra;
            }   
       }

       Serial.print("MSG: ");
       Serial.println(msgGSM);
       Serial.print("LETRA 30: ");
       Serial.println(msgGSM[29]);

       if (msgGSM[29] == 'R') {
            // Remove o numero de quem esta eviando
            String valuesToChange = msgGSM.substring(30);
            Serial.print("Cortada: ");
            Serial.println(valuesToChange);
            
            digitalWrite(4, 1);
            changeParameters( valuesToChange );

         }
    }
}

String getValue( String data, char separator, int index ) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length()-1;

    for( int i=0; i<=maxIndex && found<=index; i++ ) {
        if(data.charAt(i)==separator || i==maxIndex){
            found++;
            strIndex[0] = strIndex[1]+1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
  }
    return found > index ? data.substring( strIndex[0], strIndex[1] ) : "";
}

void changeParameters(String valuesToChange) {
    int address, valueMSB, valueLSB = 0;
    byte pkgWrite[ ] = {5, 100, 16, 196, 1, 0, 1, 0, 38, 151, 192 ,192, 5, 41, 2, 23,  1, address, valueMSB, valueLSB, 0 ,47, 104 };    
                                     
    String lsbRLV, msbRLV, lsbRLI, msbRLI = "";

    String vRef               = getValue( valuesToChange, 'F', 0 );
    String insensitivity  = getValue( valuesToChange, 'F', 1 );
    String bloqTapMax  = getValue( valuesToChange, 'F', 2) ;
    String bloqTapMin   = getValue( valuesToChange, 'F', 3 );
    String timing            = getValue( valuesToChange, 'F', 4 );
    String RLV                = getValue( valuesToChange, 'F', 5 ); //  relationship TP
    String RLI                 = getValue( valuesToChange, 'F', 6 ); //  relationship TC


    Serial.print("vRef: ");
    Serial.println(vRef);
    Serial.println("");

    Serial.print("insensitivity: ");
    Serial.println(insensitivity);
    Serial.println("");

    Serial.print("bloqTapMax: ");
    Serial.println(bloqTapMax);
    Serial.println("");
            
    Serial.print("bloqTapMin: ");
    Serial.println(bloqTapMin);
    Serial.println("");

    Serial.print("timing: ");
    Serial.println(timing);
    Serial.println("");

    Serial.print("RLV: ");
    Serial.println(RLV);
    Serial.println("");
            
    Serial.print("RLI: ");
    Serial.println(RLI);
    Serial.println("");



    // Change reference voltage
    pkgWrite[17] = 1;
    pkgWrite[18] = vRef.toInt();
    pkgWrite[19] = 0;
    Serial.write( pkgWrite, sizeof( pkgWrite ) );
    delay(3000);
    Serial.print("17: ");
    Serial.println( pkgWrite[17] );
    Serial.print("18: ");
    Serial.println( pkgWrite[18] );
    Serial.print("19: ");
    Serial.println( pkgWrite[19] );

    gsmSerial.println("AT+CMGS=\"+5548998219908\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
    delay(300);
    gsmSerial.print("Trocando..."); //text content
    delay(300);
    gsmSerial.write(26); 
    delay(300);
    digitalWrite(4, 0);
/*
    // Change insensitivity
    pkgWrite[17] = 2;
    pkgWrite[18] = insensitivity.toInt();
    pkgWrite[19] = 0;
    Serial.write( pkgWrite, sizeof( pkgWrite ) );
    delay(3000);

    // Change bloqTapMax
    pkgWrite[17] = 13;
    pkgWrite[18] = bloqTapMax.toInt();
    pkgWrite[19] = 0;
    Serial.write( pkgWrite, sizeof( pkgWrite ) );
    delay(3000);

    // Change bloqTapMin
    pkgWrite[17] = 14;
    pkgWrite[18] = bloqTapMin.toInt();
    pkgWrite[19] = 0;
    Serial.write( pkgWrite, sizeof( pkgWrite ) );
    delay(3000);

    //  Timing change
    pkgWrite[17] = 3;
    pkgWrite[18] = timing.toInt();
    pkgWrite[19] = 0;
    Serial.write( pkgWrite, sizeof( pkgWrite ) );
    delay(3000);

    // Change RLV
    if (RLV.length() == 3 ){
        RLV += "0";
        msbRLV = RLV.substring(0, 2);
        lsbRLV = RLV.substring(2);
        lsbRLV = RLV[3];
        lsbRLV += RLV[2];
   
    } else {
        msbRLV = RLV.substring(0, 2);
        lsbRLV = RLV.substring(2);
    }

    pkgWrite[17] = 15;
    pkgWrite[18] = msbRLV.toInt();
    pkgWrite[19] = lsbRLV.toInt();
    Serial.write( pkgWrite, sizeof( pkgWrite ) );
    delay(3000);
    
    // Change RLI
    if (RLI.length() == 3 ){
        RLI += "0";
        msbRLI = RLI.substring(0, 2);
        lsbRLI = RLI.substring(2);
        lsbRLI = RLI[3];
        lsbRLI += RLI[2];    
    } else {
        msbRLI = RLI.substring(0, 2);       
        lsbRLI = RLI.substring(2);
    }

    pkgWrite[17] = 15;
    pkgWrite[18] = msbRLI.toInt();
    pkgWrite[19] = lsbRLI.toInt();
    Serial.write( pkgWrite, sizeof( pkgWrite ) );
    delay(3000);
*/
}

void loop() {
    if (gsmSerial.available()){
        receivedMSG();
    }
}

