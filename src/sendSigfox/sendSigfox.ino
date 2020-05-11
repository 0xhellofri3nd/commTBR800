#include <stdio.h>
#include <MsTimer2.h>
#include <AltSoftSerial.h>
#include <SoftwareSerial.h> 

#define ledstat 4

int tempopisca, tempoenvio, timeoutSF = 0;

//String pkgRead = "";
byte pkgRead[300];
bool initpack = false;
bool lersigfox, lendoSF = false;
int index =0;
String dado="";

AltSoftSerial sigfoxSerial;
SoftwareSerial gsmSerial( 11, 10 );

void setup(){
    Serial.begin(9600);
    MsTimer2::set(1000, requestData); // interruption 
    MsTimer2::start();
    pinMode(ledstat, OUTPUT);
    gsmSerial.begin(9600);
    sigfoxSerial.begin(9600);

    // initialize Sigfox
    sigfoxSerial.println("AT");
    delay(100);
    sigfoxSerial.println("AT$RC");
    delay(100);
    sigfoxSerial.println("AT$SF=255255");
    delay(100);
    initpack = true;
}

// Read the Voltage Regulator values
void requestData(){
    ++tempoenvio;
    ++tempopisca;
    ++timeoutSF;
    if ( (tempoenvio == 30) || (initpack == true) ){
        lersigfox = true;
        initpack = false;
    }
}

// Show received packet parameters
void sendToSigfox(){
    String Sigfox, 
               vCharge, vChargeLSB, vChargeMSB, 
               iCharge, iChargeLSB, iChargeMSB,
               vSource, vSourceLSB, vSourceMSB,
               iSource, iSourceLSB, iSourceMSB,
               vLine,   vLineLSB, vLineMSB,
               iLine,   iLineLSB, iLineMSB,
               currentTap, currentTapLSB, currentTapMSB,
               maxTap,  maxTapLSB, maxTapMSB, 
               minTap,  minTapLSB, minTapMSB, 
               pkgToSendA, pkgToSendB, pkgToSendC = "";

    vChargeLSB = decToHex(pkgRead[31], 1);
    vChargeMSB = decToHex(pkgRead[32], 1);
    vCharge = vChargeMSB + vChargeLSB;
    
    vCharge = hexToDec( vCharge );

    switch ( vCharge.length() ){
        case 1:
            vCharge = "000" + vCharge;
            break;
        case 2:
            vCharge = "00" + vCharge;
            break;
        case 3:
            vCharge = "0" + vCharge;
            break;
    }

    iChargeLSB = decToHex(pkgRead[34], 1);
    iChargeMSB = decToHex(pkgRead[35], 1);
    iCharge = iChargeMSB + iChargeLSB;

    iCharge = hexToDec(iCharge);

    switch ( iCharge.length() ){
        case 1:
            iCharge = "000" + iCharge;
            break;
        case 2:
            iCharge = "00" + iCharge;
            break;
        case 3:
            iCharge = "0" + iCharge;
            break;
    }

    vSourceLSB = decToHex(pkgRead[37], 1);
    vSourceMSB = decToHex(pkgRead[38], 1);
    vSource = vSourceMSB + vSourceLSB;
    
    vSource = hexToDec(vSource);
    
    switch ( vSource.length() ){
        case 1:
            vSource = "000" + vSource;
            break;
        case 2:
            vSource = "00" + vSource;
            break;
        case 3:
            vSource = "0" + vSource;
            break;
    }
    
    iSourceLSB = decToHex(pkgRead[40], 1);
    iSourceMSB = decToHex(pkgRead[41], 1);
    iSource = iSourceMSB + iSourceLSB;
    
    iSource = hexToDec(iSource);

    switch ( iSource.length() ){
        case 1:
            iSource = "000" + iSource;
            break;
        case 2:
            iSource = "00" + iSource;
            break;
        case 3:
            iSource = "0" + iSource;
            break;
    }
    
    // Pula 2 bytes mesmo!
    vLineLSB = decToHex(pkgRead[43], 1);
    vLineMSB = decToHex(pkgRead[46], 1);
    vLine = vLineMSB + vLineLSB;
    
    vLine = hexToDec( vLine ); // Dado real:  67 42 eb 05, o que é 42 eb?

    switch ( vLine.length() ){
        case 1:
            vLine = "000" + vLine;
            break;
        case 2:
            vLine = "00" + vLine;
            break;
        case 3:
            vLine = "0" + vLine;
            break;
    }

    iLineLSB = decToHex(pkgRead[48], 1);
    iLineMSB = decToHex(pkgRead[49], 1);
    iLine = iLineMSB + iLineLSB;    

    iLine = hexToDec(iLine); 

    switch ( iLine.length() ){
        case 1:
            iLine = "000" + iLine;
            break;
        case 2:
            iLine = "00" + iLine;
            break;
        case 3:
            iLine = "0" + iLine;
            break;
    }
    
    currentTapLSB = decToHex(pkgRead[114], 1);
    currentTapMSB = decToHex(pkgRead[115], 1);
    currentTap = currentTapMSB + currentTapLSB;
        
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "ff") )
        currentTap = "A01";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fe") )
        currentTap = "A02";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fd") )
        currentTap = "A03";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fc") )
        currentTap = "A04";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fb") )
        currentTap = "A05";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fa") )
        currentTap = "A06";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f9") )
        currentTap = "A07";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f8") )
        currentTap = "A08";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f7") )
        currentTap = "A09";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f6") )
        currentTap = "A10";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f5") )
        currentTap = "A11";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f4") )
        currentTap = "A12";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f3") )
        currentTap = "A13";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f2") )
        currentTap = "A14";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f1") )
        currentTap = "A15";
    if ( (currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f0") )
        currentTap = "A16";

    if ( currentTap.substring(0, 1) == "0" ){
        currentTap = hexToDec(currentTap.substring(1));
        if (currentTap.length() == 1){
            currentTap = "B0" + currentTap;
        } else {
            currentTap = "B" + currentTap;
        }
    }

    minTapLSB = decToHex(pkgRead[119], 1);
    minTapMSB = decToHex(pkgRead[120], 1);
    minTap = minTapMSB + minTapLSB; 

    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "ff") )
        minTap = "A01";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fe") )
        minTap = "A02";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fd") )
        minTap = "A03";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fc") )
        minTap = "A04";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fb") )
        minTap = "A05";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fa") )
        minTap = "A06";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f9") )
        minTap = "A07";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f8") )
        minTap = "A08";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f7") )
        minTap = "A09";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f6") )
        minTap = "A10";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f5") )
        minTap = "A11";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f4") )
        minTap = "A12";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f3") )
        minTap = "A13";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f2") )
        minTap = "A14";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f1") )
        minTap = "A15";
    if ( (minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f0") )
        minTap = "A16";

    if ( minTap.substring(0, 1) == "0" ){
        minTap = hexToDec(minTap.substring(1));
        if (minTap.length() == 1){
            minTap = "B0" + minTap;
        } else {
            minTap = "B" + minTap;
        }
    }

    maxTapLSB = decToHex(pkgRead[139], 1);
    maxTapMSB = decToHex(pkgRead[140], 1);
    maxTap = maxTapMSB + maxTapLSB; 

    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "ff") )
        maxTap = "A01";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fe") )
        maxTap = "A02";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fd") )
        maxTap = "A03";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fc") )
        maxTap = "A04";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fb") )
        maxTap = "A05";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fa") )
        maxTap = "A06";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f9") )
        maxTap = "A07";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f8") )
        maxTap = "A08";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f7") )
        maxTap = "A09";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f6") )
        maxTap = "A10";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f5") )
        maxTap = "A11";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f4") )
        maxTap = "A12";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f3") )
        maxTap = "A13";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f2") )
        maxTap = "A14";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f1") )
        maxTap = "A15";
    if ( (maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f0") )
        maxTap = "A16";

    if ( maxTap.substring(0, 1) == "0" ){
        maxTap = hexToDec(maxTap.substring(1));
        if (maxTap.length() == 1){
            maxTap = "B0" + maxTap;
        } else {
            maxTap = "B" + maxTap;
        }
    }

    pkgToSendA = "A" + vCharge + iCharge + currentTap;
    pkgToSendB = "B" + vSource + iSource + maxTap;
    pkgToSendC = "C" + vLine + iLine + minTap;

    delay(100);
    sigfoxSerial.println("AT");
    sigfoxSerial.println("AT$RC");
    Sigfox = "AT$SF=";
    delay(100);
    //Serial.println("Enviando para nuvem : " + pkgToSendA);
    Sigfox += pkgToSendA;
    sigfoxSerial.println(Sigfox);    
    pkgToSendA, Sigfox = "";

    delay(21000); // Minimo intervalo entre menssagens é de 20 segundos

    sigfoxSerial.println("AT");
    sigfoxSerial.println("AT$RC");
    Sigfox = "AT$SF=";
    delay(100);
    Serial.println("Enviando para nuvem : " + pkgToSendB);
    Sigfox += pkgToSendB;
    sigfoxSerial.println(Sigfox);    
    pkgToSendB, Sigfox = "";

    delay(21000); // Minimo intervalo entre menssagens é de 20 segundos

    sigfoxSerial.println("AT");
    sigfoxSerial.println("AT$RC");
    Sigfox = "AT$SF=";
    delay(100);
    Serial.println("Enviando para nuvem : " + pkgToSendC);
    Sigfox += pkgToSendC;
    sigfoxSerial.println(Sigfox);    
    pkgToSendC, Sigfox = "";
}

unsigned int hexToDec(String hexString){
    unsigned int decValue = 0;
    int nextInt;
    for ( int i = 0; i < hexString.length(); i++ ){
        nextInt = int(hexString.charAt(i));
        if (nextInt >= 48 && nextInt <= 57)
            nextInt = map(nextInt, 48, 57, 0, 9);
        if (nextInt >= 65 && nextInt <= 70)
            nextInt = map(nextInt, 65, 70, 10, 15);
        if (nextInt >= 97 && nextInt <= 102)
            nextInt = map(nextInt, 97, 102, 10, 15);
        nextInt = constrain(nextInt, 0, 15);
        decValue = (decValue * 16) + nextInt;
    }
    return decValue;
}

String decToHex(long decValue, byte desiredStringLength) {
  
  String hexString = String(decValue, HEX);
  while (hexString.length() < desiredStringLength) hexString = "0" + hexString;

  return hexString;
}
void loop(){

    if (lersigfox == true) {
        byte basePKG[] = { 0x05, 0x64, 0x0b,0xc4, 0x01, 0x00, 0x01,0x00, 0xc2, 0x2e, 0xc0, 0xc0,0x01, 0x3c, 0x01, 0x06, 0xff, 0x50 };
        Serial.write(basePKG, sizeof(basePKG));
        tempoenvio = 0;
        lersigfox = false;
        lendoSF = true;
        gsmSerial.println("*******");
        gsmSerial.print("index: ");
        gsmSerial.println(index);
        
        index=0;
    }    
    if ( lendoSF == false ) {
        timeoutSF=0;
    }

    while ( Serial.available() > 0 ) {
        char sb = Serial.read();
        pkgRead[index] = sb;
        index++;        
    }
    
    if ( index >= 247 ){
        sendToSigfox();
          
    }


    if ( tempopisca > 1 ) {
        digitalWrite( ledstat, true );
    }
    if ( tempopisca > 2 ) {
        digitalWrite(ledstat, false);
        tempopisca = 0;
    }
}
