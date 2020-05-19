#include <stdio.h>
#include <Crc16.h>
#include <MsTimer2.h>
#include <AltSoftSerial.h>
#include <SoftwareSerial.h>

#define ledstat 4

byte pkgRead[300];

bool lersigfox = false;
char sb;

int hack = 0, index = 0,
      readingTime = 0, timeMessage = 0, 
      posCut = 0, idtData = 0, timePriorityMessage = 0;

byte pkgWrite[13];

String valuesToChange = "", valueVerficator = "", msgGSM = "";

SoftwareSerial gsmSerial(11, 10);
AltSoftSerial sigfoxSerial;
Crc16 crc;

void setup(){
    pinMode(ledstat, OUTPUT);

    Serial.begin(9600);
    gsmSerial.begin(9600);
    sigfoxSerial.begin(9600);

    MsTimer2::set(1000, requestData); // interruption
    MsTimer2::start();

    // initialize Sigfox
    sigfoxSerial.println("AT");
    delay(100);
    sigfoxSerial.println("AT$RC");
    delay(100);
    sigfoxSerial.println("AT$SF=255255");
    delay(100);

    // time to initialize GSM
    delay(5000);
    gsmSerial.println("AT+CMGF=1"); // Configuring TEXT mode
    delay(2000);
    gsmSerial.println("AT+CNMI=2,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
    delay(2000);
}

void requestData(){
    timePriorityMessage++;
    timeMessage++;
    readingTime++;

    // A cada 3 min (180 s) realiza uma leitura
    if (readingTime == 180){
        lersigfox = true;
        readingTime = 0;
    }

    // A cada 15 min (~900 s) envia uma mensagem do tipo A
    if (timePriorityMessage == 900){
        idtData = 1;
        lersigfox = true;
        timePriorityMessage = 0;
    }

    // A cada 51 min (~3085 s) envia uma mensagem do tipo B, C e D
    if (timeMessage == 3085){
        idtData = 2;
        lersigfox = true;
        timeMessage = 0;
    }
}
void sendToSigfox(String pkgToSend){
    String Sigfox = "";
    sigfoxSerial.println("AT");
    delay(100);
    sigfoxSerial.println("AT$RC");
    delay(100);
    Sigfox = "AT$SF=";
    Sigfox += pkgToSend;
    sigfoxSerial.println(Sigfox);
    delay(100);
    Sigfox = "", pkgToSend = "";
}

// Show received packet parameters
void dataAnalysis(int idtData){
    int alarm = 0;
    String pkgToSendA = "", pkgToSendB = "", pkgToSendC = "", pkgToSendD = "", pkgToSendE = "",
               vCharge = "", vChargeLSB = "", vChargeMSB = "", iCharge = "", iChargeLSB = "", iChargeMSB = "",
               vSource = "", vSourceLSB = "", vSourceMSB = "", iSource = "", iSourceLSB = "", iSourceMSB = "",
               maxTap = "", maxTapLSB = "", maxTapMSB = "", minTap = "", minTapLSB = "", minTapMSB = "",
               vLine = "", vLineLSB = "", vLineMSB = "", iLine = "", iLineLSB = "", iLineMSB = "",
               opCounterMult = "", opCounterMultLSB = "", opCounterMultMSB = "",
               operationsCounter = "", opCounterMSB = "", opCounterLSB = "",
               currentTap = "", currentTapLSB = "", currentTapMSB = "";

    vChargeLSB = decToHex(pkgRead[31], 1);
    vChargeMSB = decToHex(pkgRead[32], 1);
    vCharge = vChargeMSB + vChargeLSB;

    vCharge = hexToDec(vCharge);

    switch (vCharge.length()){
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

    switch (iCharge.length()){
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

    switch (vSource.length()){
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

    switch (iSource.length()){
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

    vLine = hexToDec(vLine); // Dado real:  67 42 eb 05, o que é 42 eb?

    switch (vLine.length()){
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

    switch (iLine.length()){
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

    /*
    Os bytes 60, 61, 62 e 63 também são destinados ao
    contador de operações (Verificar o que acontecerá aqui!)
  */

    opCounterMultLSB = decToHex(pkgRead[60], 1);
    opCounterMultMSB = decToHex(pkgRead[61], 1);
    opCounterMult = opCounterMultMSB + opCounterMultLSB;
    opCounterMult = hexToDec(opCounterMult);

    opCounterLSB = decToHex(pkgRead[65], 1);
    opCounterMSB = decToHex(pkgRead[66], 1);
    operationsCounter = opCounterMSB + opCounterLSB;
    operationsCounter = hexToDec(operationsCounter);

    operationsCounter = opCounterMult + operationsCounter;

    operationsCounter = opCounterMultMSB + opCounterMultLSB + opCounterMSB + opCounterLSB;

    switch (operationsCounter.length()){
    case 1:
        operationsCounter = "00000" + operationsCounter;
        break;
    case 2:
        operationsCounter = "0000" + operationsCounter;
        break;
    case 3:
        operationsCounter = "000" + operationsCounter;
        break;
    case 4:
        operationsCounter = "00" + operationsCounter;
        break;
    case 5:
        operationsCounter = "0" + operationsCounter;
        break;
    }

    currentTapLSB = decToHex(pkgRead[114], 1);
    currentTapMSB = decToHex(pkgRead[115], 1);
    currentTap = currentTapMSB + currentTapLSB;

    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "ff"))
        currentTap = "A01";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fe"))
        currentTap = "A02";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fd"))
        currentTap = "A03";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fc"))
        currentTap = "A04";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fb"))
        currentTap = "A05";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "fa"))
        currentTap = "A06";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f9"))
        currentTap = "A07";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f8"))
        currentTap = "A08";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f7"))
        currentTap = "A09";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f6"))
        currentTap = "A10";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f5"))
        currentTap = "A11";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f4"))
        currentTap = "A12";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f3"))
        currentTap = "A13";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f2"))
        currentTap = "A14";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f1"))
        currentTap = "A15";
    if ((currentTap.substring(0, 2) == "ff") && (currentTap.substring(2) == "f0"))
        currentTap = "A16";

    if (currentTap.substring(0, 1) == "0"){
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

    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "ff"))
        minTap = "A01";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fe"))
        minTap = "A02";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fd"))
        minTap = "A03";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fc"))
        minTap = "A04";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fb"))
        minTap = "A05";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "fa"))
        minTap = "A06";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f9"))
        minTap = "A07";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f8"))
        minTap = "A08";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f7"))
        minTap = "A09";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f6"))
        minTap = "A10";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f5"))
        minTap = "A11";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f4"))
        minTap = "A12";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f3"))
        minTap = "A13";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f2"))
        minTap = "A14";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f1"))
        minTap = "A15";
    if ((minTap.substring(0, 2) == "ff") && (minTap.substring(2) == "f0"))
        minTap = "A16";

    if (minTap.substring(0, 1) == "0"){
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

    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "ff"))
        maxTap = "A01";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fe"))
        maxTap = "A02";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fd"))
        maxTap = "A03";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fc"))
        maxTap = "A04";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fb"))
        maxTap = "A05";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "fa"))
        maxTap = "A06";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f9"))
        maxTap = "A07";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f8"))
        maxTap = "A08";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f7"))
        maxTap = "A09";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f6"))
        maxTap = "A10";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f5"))
        maxTap = "A11";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f4"))
        maxTap = "A12";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f3"))
        maxTap = "A13";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f2"))
        maxTap = "A14";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f1"))
        maxTap = "A15";
    if ((maxTap.substring(0, 2) == "ff") && (maxTap.substring(2) == "f0"))
        maxTap = "A16";

    if (maxTap.substring(0, 1) == "0"){
        maxTap = hexToDec(maxTap.substring(1));
        if (maxTap.length() == 1){
            maxTap = "B0" + maxTap;
        } else {
            maxTap = "B" + maxTap;
        }
    }

    pkgToSendA = "A" + vCharge + iCharge + currentTap;
    pkgToSendB = "B" + iCharge + vSource + maxTap;
    pkgToSendC = "C" + vLine + iLine + minTap;
    pkgToSendD = "D0" + operationsCounter + iSource;

    /*
      if ( (vCharge.toInt() <= 114) || (vCharge.toInt() >= 126) ) {
          alarm++;
          if ( alarm == 2){
              pkgToSendE = "E0" + vCharge;
              sendToSigfox(pkgToSendE);
              delay(21000);
              alarm = 0;
          }
          readingTime = 0;
      }
  */

    if (idtData == 1){
        sendToSigfox(pkgToSendA);
        delay(21000);
    }

    if (idtData == 2){
        sendToSigfox(pkgToSendB);
        delay(21000); // Minimo intervalo entre menssagens é de 20 segundos
        sendToSigfox(pkgToSendC);
        delay(21000); // Minimo intervalo entre menssagens é de 20 segundos
        sendToSigfox(pkgToSendD);
        delay(21000);
    }
}

unsigned int hexToDec(String hexString){
    unsigned int decValue = 0;
    int nextInt;
    for (int i = 0; i < hexString.length(); i++){
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

String decToHex(long decValue, byte desiredStringLength){
    String hexString = String(decValue, HEX);
    while (hexString.length() < desiredStringLength)
        hexString = "0" + hexString;
    return hexString;
}

String getValue(String data, char separator, int index){
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++){
        if (data.charAt(i) == separator || i == maxIndex){
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void changeParameters(String valuesToChange){

    unsigned short crcValue;
    int address = 0, valueMSB = 0, valueLSB = 0, lsbCRC = 0, msbCRC = 0;

    byte basePkgToWrite[23] = {5, 100, 16, 196, 1, 0, 1, 0, 143, 95, 192, 192, 5, 41, 2, 23, 1, address, valueMSB, valueLSB, 0};

    String lsbRLV = "", msbRLV = "", lsbRLI = "", msbRLI = "", tempRLV = "", tempRLI = "", strRLV = "", strRLI = "", tmpHexCRC = "";

    String vRef = getValue(valuesToChange, 'F', 0);
    String insensitivity = getValue(valuesToChange, 'F', 1);
    String bloqTapMax = getValue(valuesToChange, 'F', 2);
    String bloqTapMin = getValue(valuesToChange, 'F', 3);
    String timer = getValue(valuesToChange, 'F', 4);
    String RLV = getValue(valuesToChange, 'F', 5); //  relationship TP
    String RLI = getValue(valuesToChange, 'F', 6); //  relationship TC

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
    lsbCRC = hexToDec(tmpHexCRC.substring(2));
    msbCRC = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC = 0; msbCRC = 0;  crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
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
    lsbCRC = hexToDec(tmpHexCRC.substring(2));
    msbCRC = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC =0; msbCRC =0; crcValue =0; address =0; valueMSB =0; valueLSB =0; hack = 0;
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
    lsbCRC = hexToDec(tmpHexCRC.substring(2));
    msbCRC = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
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
    lsbCRC = hexToDec(tmpHexCRC.substring(2));
    msbCRC = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
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
    lsbCRC = hexToDec(tmpHexCRC.substring(2));
    msbCRC = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    // Change RLV
    int intRLV = RLV.toInt();
    int converted_msbRLV = 0;
    int converted_lsbRLV = 0;
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
    lsbCRC = hexToDec(tmpHexCRC.substring(2));
    msbCRC = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    // Change RLI
    int intRLI = RLI.toInt();
    int converted_msbRLI = 0;
    int converted_lsbRLI = 0;
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

    tmpHexCRC = decToHex(crcValue, 4);
    lsbCRC = hexToDec(tmpHexCRC.substring(2));
    msbCRC = hexToDec(tmpHexCRC.substring(0, 2));

    // add CRC to basePkgToWrite
    basePkgToWrite[21] = lsbCRC;
    basePkgToWrite[22] = msbCRC;

    Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

    lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; hack = 0; valueMSB = 0; valueLSB = 0;
    tmpHexCRC = "";

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);

    byte clearPkg[25] = {5, 100, 18, 68, 1, 0, 1, 0, 76, 24, 194, 192, 129, 128, 0, 41, 2, 23, 1, 16, converted_msbRLI, converted_lsbRLI, 0};
    byte clearPkgSend[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // separates only the data needed to calculate the CRC
    for (int i = 10; i <= sizeof(clearPkg); i++){
        for (int x = hack; x <= 12; x++){
            clearPkgSend[x] = clearPkg[i];
            hack++;
            x = 13;
        }
    }

    // Calculate CRC
    crcValue = crc.DNP3(clearPkgSend, 0, 13);
    tmpHexCRC = decToHex(crcValue, 4);
    lsbCRC = hexToDec(tmpHexCRC.substring(2));
    msbCRC = hexToDec(tmpHexCRC.substring(0, 2));

    clearPkg[23] = lsbCRC;
    clearPkg[24] = msbCRC;

    Serial.write(clearPkg, sizeof(clearPkg));

    Serial.end();

    digitalWrite(ledstat, true);
    delay(500);
    digitalWrite(ledstat, false);

    delay(800);
    Serial.begin(9600);

    /*gsmSerial.println("AT+CMGS=\"+5548996137114\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  delay(300);
  gsmSerial.print("Trocou..."); //text content
  delay(300);
  gsmSerial.write(26);*/
}

void receivedMSG(){
    msgGSM = gsmSerial.readString();
    valueVerficator = msgGSM.substring(51);
    for (int i = 0; i < valueVerficator.length(); i++){
        if (valueVerficator[i] == 'E'){
            valuesToChange = valueVerficator.substring(0, i);
            posCut = i;
        }
    }
}

void loop(){
    if ( (lersigfox == true) && (idtData == 0) ){
        delay(100);
        byte basePKG[] = {0x05, 0x64, 0x0b, 0xc4, 0x01, 0x00, 0x01, 0x00, 0xc2, 0x2e, 0xc0, 0xc0, 0x01, 0x3c, 0x01, 0x06, 0xff, 0x50};
        Serial.write(basePKG, sizeof(basePKG));
        lersigfox = false;
    }

    if ( (lersigfox == true) && (idtData != 0) ){
        delay(100);
        byte basePKG[] = {0x05, 0x64, 0x0b, 0xc4, 0x01, 0x00, 0x01, 0x00, 0xc2, 0x2e, 0xc0, 0xc0, 0x01, 0x3c, 0x01, 0x06, 0xff, 0x50};
        Serial.write(basePKG, sizeof(basePKG));
        lersigfox = false;
    }

    while ( Serial.available() > 0 ){
        sb = Serial.read();
        pkgRead[index] = sb;
        index++;
    }

    if (index >= 247){
        dataAnalysis(idtData);
        index = 0;
        idtData = 0;
    }

    if (gsmSerial.available() > 0){
        receivedMSG();
    }

    if ( (msgGSM[50] == 'R') && (msgGSM[posCut + 51] == 'E') ){
        changeParameters(valuesToChange);
        msgGSM = "";
    }
}