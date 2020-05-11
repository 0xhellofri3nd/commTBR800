#include <stdio.h>
#include <Crc16.h>
#include <MsTimer2.h>
#include <AltSoftSerial.h>
#include <SoftwareSerial.h>

#define ledstat 4

byte pkgWrite[13];


int index, tempopisca, tempoenvio,timeoutGSM, recGSM, hack, timeoutSF = 0;

String Sigfox, pkgRead, valuesToChange, msgGSM = "";

bool initpack = false;
bool lersigfox, enviaGSM , lendoSF= false;

Crc16 crc;
AltSoftSerial sigfoxSerial;
SoftwareSerial gsmSerial( 11, 10 );

void setup(){
    Serial.begin(9600);
    MsTimer2::set(1000, requestData); // interruption 
    MsTimer2::start();

    pinMode(ledstat, OUTPUT);

    sigfoxSerial.begin(9600);
    gsmSerial.begin(9600);

    // initialize Sigfox
    sigfoxSerial.println("AT");
    delay(100);
    sigfoxSerial.println("AT$RC");
    Sigfox = "AT$SF=255255";
    delay(100);
    sigfoxSerial.println(Sigfox);
    delay(100);

    // time to initialize GSM
    delay(5000);
    gsmSerial.println("AT+CMGF=1"); // Configuring TEXT mode
    delay(2000);
    gsmSerial.println("AT+CNMI=2,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
    delay(2000);

    initpack = true;
}

// Read the Voltage Regulator values
void requestData(){
    ++tempoenvio;
    ++tempopisca;
    ++timeoutGSM;
    ++timeoutSF;
    if ( (tempoenvio == 1200) || (initpack == true) ){
        lersigfox = true;
        initpack = false;
    }
}

// Show received packet parameters
void sendToSigfox(){
    String vCharge, iCharge, vSource, iSource, vLine, iLine, currentTap, maxTap, minTap, pkgToSend = "";
    
    
    //vCharge = hexToDec( String(pkgRead[64]) + String(pkgRead[65]) + String(pkgRead[62]) + String(pkgRead[63]) );
    vCharge = String(pkgRead[64]) + String(pkgRead[65]) + String(pkgRead[62]) + String(pkgRead[63]);
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
    //iCharge = hexToDec( String(pkgRead[70]) + String(pkgRead[71]) + String(pkgRead[68]) + String(pkgRead[69]) ); // Esta em mA (dividir por 1000 no supervisório)
    iCharge = String(pkgRead[70]) + String(pkgRead[71]) + String(pkgRead[68]) + String(pkgRead[69]); // Esta em mA (dividir por 1000 no supervisório)
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
    //vSource = hexToDec( String(pkgRead[76]) + String(pkgRead[77]) + String(pkgRead[74]) + String(pkgRead[75]) );
    vSource = String(pkgRead[76]) + String(pkgRead[77]) + String(pkgRead[74]) + String(pkgRead[75]);
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
    //iSource = hexToDec( String(pkgRead[82]) + String(pkgRead[83]) + String(pkgRead[80]) + String(pkgRead[81]) );
    iSource = String(pkgRead[82]) + String(pkgRead[83]) + String(pkgRead[80]) + String(pkgRead[81]);
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
    //vLine = hexToDec( String(pkgRead[92]) + String(pkgRead[93]) + String(pkgRead[86]) + String(pkgRead[87]) ); // Dado real:  67 42 eb 05, o que é 42 eb?
    vLine = String(pkgRead[92]) + String(pkgRead[93]) + String(pkgRead[86]) + String(pkgRead[87]); // Dado real:  67 42 eb 05, o que é 42 eb?
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
    //iLine = hexToDec( String(pkgRead[98]) + String(pkgRead[99]) + String(pkgRead[96]) + String(pkgRead[97]) ); 
    iLine = String(pkgRead[98]) + String(pkgRead[99]) + String(pkgRead[96]) + String(pkgRead[97]); 
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

    //currentTap = hexToDec( String(pkgRead[230]) + String(pkgRead[231]) + String(pkgRead[228]) + String(pkgRead[229]) ); 
    currentTap = String(pkgRead[230]) + String(pkgRead[231]) + String(pkgRead[228]) + String(pkgRead[229]);
        
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
    
    Serial.println(currentTap);
    
    //maxTap = hexToDec( String(pkgRead[240]) + String(pkgRead[241]) + String(pkgRead[238]) + String(pkgRead[239]) ); 
    maxTap = String(pkgRead[240]) + String(pkgRead[241]) + String(pkgRead[238]) + String(pkgRead[239]);

    // pkgToSend = vCharge + iCharge + vSource + iSource + vLine + iLine ;
    
    /*
    Serial.println(vCharge);
    Serial.println(iCharge);
    Serial.println(vSource);
    Serial.println(iSource);
    Serial.println(vLine);
    Serial.println(iLine);
    Serial.println(currentTap); 
    Serial.println(maxTap);
    Serial.println(minTap);
    */
    //Serial.println(pkgToSend);
    //Serial.println(sizeof(pkgToSend));

    delay(100);
    sigfoxSerial.println("AT");
    sigfoxSerial.println("AT$RC");
    Sigfox = "AT$SF=";
    delay(100);
    Serial.println("Enviando para nuvem : " + pkgToSend);
    Sigfox += pkgToSend;
    sigfoxSerial.println(Sigfox);
    pkgToSend, Sigfox = "";
}

void updateSerial(){
    while (Serial.available()){
        gsmSerial.write(Serial.read());
    }

    while (gsmSerial.available()){
        Serial.write(gsmSerial.read());
    }
}

 void receivedMSG(){
    msgGSM = gsmSerial.readString();
    recGSM = 1;
    timeoutGSM = 0;
 }

void changeParameters(String valuesToChange){
    
    int address, valueMSB, valueLSB, lsbCRC, msbCRC = 0;
    unsigned short crcValue;
    
    byte basePkgToWrite[23] = {5, 100, 16, 196, 1, 0, 1, 0, 143, 95, 192, 192, 5, 41, 2, 23, 1, address, valueMSB, valueLSB, 0};
   
    String lsbRLV, msbRLV, lsbRLI, msbRLI, tempRLV, tempRLI, forTest, strRLV, strRLI, tmpHexCRC = "";
    
    String vRef = getValue(valuesToChange, 'F', 0);
    String insensitivity = getValue(valuesToChange, 'F', 1);
    String bloqTapMax = getValue(valuesToChange, 'F', 2);
    String bloqTapMin = getValue(valuesToChange, 'F', 3);
    String timer = getValue(valuesToChange, 'F', 4);
    String RLV = getValue(valuesToChange, 'F', 5); //  relationship TP
    String RLI = getValue(valuesToChange, 'F', 6); //  relationship TC

    // Change reference voltage
    basePkgToWrite[17] = 1;
    basePkgToWrite[18] = vRef.toInt();
    basePkgToWrite[19] = 0;

    // separates only the data needed to calculate the CRC
    for (int i = 10; i <= sizeof(basePkgToWrite); i++){
        for (int x = hack; x <= 10; x++){
            pkgWrite[x] = basePkgToWrite[i];
            hack++;
            x = 11;
        }
    }

    // Calculate CRC
    crcValue = crc.DNP3(pkgWrite, 0, 11);

    tmpHexCRC = decToHex(crcValue, 4);
    lsbCRC         = hexToDec(tmpHexCRC.substring(2));
    msbCRC       = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC, msbCRC, crcValue, address, valueMSB, valueLSB, hack = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    // Change insensitivity
    basePkgToWrite[17] = 2;
    basePkgToWrite[18] = insensitivity.toInt();
    basePkgToWrite[19] = 0;

    // separates only the data needed to calculate the CRC
    for (int i = 10; i <= sizeof(basePkgToWrite); i++){
        for (int x = hack; x <= 10; x++){
            pkgWrite[x] = basePkgToWrite[i];
            hack++;
            x = 11;
        }
    }
    // Calculate CRC
    crcValue = crc.DNP3(pkgWrite, 0, 11);

    tmpHexCRC = decToHex(crcValue, 4);
    lsbCRC         = hexToDec(tmpHexCRC.substring(2));
    msbCRC       = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC, msbCRC, crcValue, address, valueMSB, valueLSB, hack = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    // Change bloqTapMax
    basePkgToWrite[17] = 13;
    basePkgToWrite[18] = bloqTapMax.toInt();
    basePkgToWrite[19] = 0;

    // separates only the data needed to calculate the CRC
    for (int i = 10; i <= sizeof(basePkgToWrite); i++){
        for (int x = hack; x <= 10; x++){
            pkgWrite[x] = basePkgToWrite[i];
            hack++;
            x = 11;
        }
    }

    // Calculate CRC
    crcValue = crc.DNP3(pkgWrite, 0, 11);

    tmpHexCRC = decToHex(crcValue, 4);
    lsbCRC         = hexToDec(tmpHexCRC.substring(2));
    msbCRC       = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC, msbCRC, crcValue, address, valueMSB, valueLSB, hack = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    // Change bloqTapMin
    basePkgToWrite[17] = 14;
    basePkgToWrite[18] = bloqTapMin.toInt();
    basePkgToWrite[19] = 0;

    // separates only the data needed to calculate the CRC
    for (int i = 10; i <= sizeof(basePkgToWrite); i++){
        for (int x = hack; x <= 10; x++){
            pkgWrite[x] = basePkgToWrite[i];
            hack++;
            x = 11;
        }
    }

    // Calculate CRC
    crcValue = crc.DNP3(pkgWrite, 0, 11);

    tmpHexCRC = decToHex(crcValue, 4);
    lsbCRC         = hexToDec(tmpHexCRC.substring(2));
    msbCRC       = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC, msbCRC, crcValue, address, valueMSB, valueLSB, hack = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    //  timer change
    basePkgToWrite[17] = 3;
    basePkgToWrite[18] = timer.toInt();
    basePkgToWrite[19] = 0;

    // separates only the data needed to calculate the CRC
    for (int i = 10; i <= sizeof(basePkgToWrite); i++){
        for (int x = hack; x <= 10; x++){
            pkgWrite[x] = basePkgToWrite[i];
            hack++;
            x = 11;
        }
    }

    // Calculate CRC
    crcValue = crc.DNP3(pkgWrite, 0, 11);

    tmpHexCRC = decToHex(crcValue, 4);
    lsbCRC         = hexToDec(tmpHexCRC.substring(2));
    msbCRC       = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC, msbCRC, crcValue, address, valueMSB, valueLSB, hack = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    // Change RLV
    int intRLV = RLV.toInt();
    int converted_msbRLV, converted_lsbRLV = 0;
    forTest = "";
    strRLV = "";

    strRLV = String(intRLV, HEX);

    if (strRLV.length() == 4){
        msbRLV = strRLV.substring(2);
        converted_msbRLV = int(hexToDec(msbRLV));

        lsbRLV = strRLV.substring(0, 2);
        converted_lsbRLV = int(hexToDec(lsbRLV));
    }

    if (strRLV.length() == 3){
        msbRLV = strRLV.substring(1);
        converted_msbRLV = int(hexToDec(msbRLV));

        lsbRLV = strRLV.substring(0, 1);
        converted_lsbRLV = int(hexToDec(lsbRLV));
    }

    if (strRLV.length() == 2){
        converted_msbRLV = int(hexToDec(strRLV));
        converted_lsbRLV = 0;
    }

    basePkgToWrite[17] = 15;
    basePkgToWrite[18] = converted_msbRLV;
    basePkgToWrite[19] = converted_lsbRLV;
   
    // separates only the data needed to calculate the CRC
    for (int i = 10; i <= sizeof(basePkgToWrite); i++){
        for (int x = hack; x <= 10; x++){
            pkgWrite[x] = basePkgToWrite[i];
            hack++;
            x = 11;
        }
    }

    // Calculate CRC
    crcValue = crc.DNP3(pkgWrite, 0, 11);

    tmpHexCRC = decToHex(crcValue, 4);
    lsbCRC         = hexToDec(tmpHexCRC.substring(2));
    msbCRC       = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC, msbCRC, crcValue, address, valueMSB, valueLSB, hack = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    // Change RLI
    int intRLI = RLI.toInt();
    int converted_msbRLI, converted_lsbRLI = 0;
    forTest = "";
    strRLV = "";

    strRLI = String(intRLI, HEX);

    if (strRLI.length() == 4){
        msbRLI = strRLI.substring(2);
        converted_msbRLI = int(hexToDec(msbRLI));

        lsbRLI = strRLI.substring(0, 2);
        converted_lsbRLI = int(hexToDec(lsbRLI));
    }

    if (strRLI.length() == 3){
        msbRLI = strRLI.substring(1);
        converted_msbRLI = int(hexToDec(msbRLI));

        lsbRLI = strRLI.substring(0, 1);
        converted_lsbRLI = int(hexToDec(lsbRLI));
    }

    if (strRLI.length() == 2){
        converted_msbRLI = int(hexToDec(strRLI));
        converted_lsbRLI = 0;
    }

    basePkgToWrite[17] = 16;
    basePkgToWrite[18] = converted_msbRLI;
    basePkgToWrite[19] = converted_lsbRLI;

     // separates only the data needed to calculate the CRC
    for (int i = 10; i <= sizeof(basePkgToWrite); i++){
        for (int x = hack; x <= 10; x++){
            pkgWrite[x] = basePkgToWrite[i];
            hack++;
            x = 11;
        }
    }

    // Calculate CRC
    crcValue = crc.DNP3(pkgWrite, 0, 11);

    tmpHexCRC   = decToHex(crcValue, 4);
    lsbCRC         = hexToDec(tmpHexCRC.substring(2));
    msbCRC       = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC, msbCRC, crcValue, address, valueMSB, valueLSB, hack = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    gsmSerial.println("AT+CMGS=\"+5548996137114\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
    delay(300);
    gsmSerial.print("Trocou..."); //text content
    delay(300);
    gsmSerial.write(26);

    enviaGSM = false;
}

String getValue(String data, char separator, int index){
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for ( int i = 0; i <= maxIndex && found <= index; i++ ){
        if ( data.charAt(i) == separator || i == maxIndex ){
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
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
/*
    if ( (lersigfox == true) && (enviaGSM == false)  ) {
        
        byte basePKG[] = { 0x05, 0x64, 0x0b,0xc4, 0x01, 0x00, 0x01,0x00, 0xc2, 0x2e, 0xc0, 0xc0,0x01, 0x3c, 0x01, 0x06, 0xff, 0x50 };
        Serial.write(basePKG, sizeof(basePKG));
        
        index = 0;
        tempoenvio = 0;
        lersigfox = false;
        lendoSF = true;
    }    
    if ( lendoSF == false ) {
        timeoutSF=0;
    }
*/
    while ( Serial.available() > 0 ) {
        //String pkgRead = Serial.readString();
        pkgRead = Serial.readString();
        //pkgRead[index] = sb;
        //index++;        
        Serial.println(pkgRead.length());      
    }


    //if ( timeoutSF == 2 ) {
        if ( pkgRead.length() >= 495 ){
            sendToSigfox();
            pkgRead = "";  
        }
        //lendoSF=false;
        //timeoutSF=0;  
    //}


    if ( gsmSerial.available() > 0 ){
        receivedMSG();
    }    


    if ( recGSM == 1 ) {
        if ( timeoutGSM > 1 ) {
            if ( msgGSM[50] == 'R' ){
                // Remove o numero de quem esta eviando
                valuesToChange = msgGSM.substring(51);
                enviaGSM = true;
            }
            recGSM=0;
        }
    } else {
        timeoutGSM=0;
    }    

    if ( ( enviaGSM == true ) && ( lersigfox == false ) ){
        changeParameters(valuesToChange);
        valuesToChange = "";
    }    


    if ( tempopisca > 1 ) {
        digitalWrite( ledstat, true );
    }
    if ( tempopisca > 2 ) {
        digitalWrite(ledstat, false);
        tempopisca = 0;
    }

}
