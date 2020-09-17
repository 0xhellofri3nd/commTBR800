#include <stdio.h>
#include <Crc16.h>
#include <avr/wdt.h>
#include <MsTimer2.h>
#include <AltSoftSerial.h>
#include <NeoSWSerial.h>

NeoSWSerial gsmSerial(11, 10);
AltSoftSerial sigfoxSerial;
Crc16 crc;

#define outStatus 4

char sb;

int timeInitialize = 0, x = 0, pkgGSMlength = 0, hack = 0, index = 0, readingTime = 0, timeMessage = 0, 
      posCut = 0, idtData = 0, timePriorityMessage = 0, timestatus = 0, alarm = 0, timeSigFox = 0, timeChange = 0;

String valuesToChange = "", valueVerficator = "", msgGSM = "", dataTapMax = "", horaTapMax = "", dataTapMin = "", 
           horaTapMin = "", pkgToSendA = "", pkgToSendB = "", pkgToSendC = "", pkgToSendD = "", pkgToSendE = "", pkgToSendF = "", pkgToSendG = "";

bool initializing = false, SFStart = false, pkgASending = false, pkgBSending = false, blockpkgA = false, 
         blockpkgB = false, blockpkgC = false, blockpkgD = false, blockpkgE = false, vChargepkg = false, tapMaxpkg = false, tapMinpkg = false, 
         sendCycle = false, iChargepkg = false, blockpkgF = false, changeGSM = false, partonePackOk = false, parttwoPackOk = false, lersigfox = false, lendoSF = false;

byte gsmStart = 0, pkgNumSending = 0, pkgCycle = 0,  parameterChange = 0, pkgRead[300], pkgWrite[13];


void setup() {
    wdt_disable();

    Serial.begin(9600);
    gsmSerial.begin(38400); // Changed to 38400 to avoid interference between serials
    sigfoxSerial.begin(9600);

    pinMode(outStatus, OUTPUT);

    MsTimer2::set(1000, timersSettings); // Define time base
    MsTimer2::start();
    initializing = true;

    wdt_enable(WDTO_8S);

    pkgToSendA.reserve(30);
    pkgToSendB.reserve(30);
    pkgToSendC.reserve(30);
    pkgToSendD.reserve(30);
}
/*
* Function for implementing multiple timers from the same time base
*/
void timersSettings() {
    ++timestatus;
    if ( initializing ) {
        timeInitialize++;
    } else {
        timeInitialize = 0;
        if ((pkgASending) || (pkgBSending) || (vChargepkg) || (tapMaxpkg) || (tapMinpkg) || (iChargepkg)) {
            timeSigFox++;
        } else {
            if (changeGSM == false) {
                timePriorityMessage++;
                timeMessage++;
                readingTime++;
                timeSigFox = 0;
            } else {
                timeChange++;
            }
        }
        // Every 3 min (180 s) take a reading
        if (readingTime == 180) {
            lersigfox = true;
            readingTime = 0;
        }
        // Every 15 min (~ 900 s) sends a type A message
        if (timePriorityMessage == 900) {
            idtData = 1;
            lersigfox = true;
            timePriorityMessage = 0;
            alarm = 0;
        };
        // Every 3h min (~ 10800 s) send a message of type B, C and D
        if (timeMessage == 10800) {
            idtData = 2;
            lersigfox = true;
            timeMessage = 0;
        }
    }
}
/*
* Function for sending data over the Sigfox network
*       pkgToSend: It must be a String (with a maximum of 12 characters), where the characters A, B, C and D indicate a data update package. 
*                            The "E" package is intended for alarms
*       Note: The string must always contain an even number of characters
*/
void sendToSigfox( String pkgToSend ) {
    String Sigfox = "";
    Sigfox.reserve(30);
    Sigfox = "AT$SF=" + pkgToSend;
    delay(100);
    sigfoxSerial.println("AT");
    delay(100);
    sigfoxSerial.println("AT$RC");
    delay(100);
    sigfoxSerial.println(Sigfox);
    delay(100);
    Sigfox = "", pkgToSend = "";
}
/*
*  Function that separates the data read from the TB-R800 control and prepares the packets for sending over the Sigfox network
*       idtData: It will indicate which packet will be sent, with equal value 1 for packet A, and 2 for packets B, C, D
*/
void dataAnalysis( int idtData ) {
    String  vCharge = "", iCharge = "", vSource = "",  iSource = "", maxTap = "" ,
                minTap = "", vLine = "", iLine = "", opCounterMult = "",  operationsCounter = "", currentTap = "", 
                hora = "", minuto = "", dia = "", mes = "", valueLSB = "", valueMSB = "", valueLSB1 = "", valueMSB1 = "";

    valueLSB = decToHex(pkgRead[31], 1);
    valueMSB = decToHex(pkgRead[32], 1);
    vCharge = valueMSB + valueLSB;

    vCharge = hexToDec(vCharge);

    switch ( vCharge.length() ) {
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
    // Charge voltage alarm
    if ((vCharge.toInt() < 1140) || (vCharge.toInt() >= 1260)) {
        alarm++;
        if ( alarm == 2) {
            pkgToSendE = "E0" + vCharge;
            vChargepkg = true;
            blockpkgC = true;
        }
    }

    valueLSB = decToHex(pkgRead[34], 1);
    valueMSB = decToHex(pkgRead[35], 1);
    iCharge = valueMSB + valueLSB;

    iCharge = hexToDec(iCharge);

    switch ( iCharge.length() ) {
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
    // Current charge alarm
   if (iCharge.toInt() > 270 ) {
        pkgToSendE = "E1" + iCharge;
        iChargepkg = true;
        blockpkgF = true;
    }

    valueLSB = decToHex(pkgRead[37], 1);
    valueMSB = decToHex(pkgRead[38], 1);
    vSource = valueMSB + valueLSB;

    vSource = hexToDec(vSource);

    switch (vSource.length()) {
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

    valueLSB = decToHex(pkgRead[40], 1);
    valueMSB = decToHex(pkgRead[41], 1);
    iSource = valueMSB + valueLSB;

    iSource = hexToDec(iSource);

    switch ( iSource.length() ) {
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

    // It skips 2 bytes anyway!
    valueLSB = decToHex(pkgRead[43], 1);
    valueMSB = decToHex(pkgRead[46], 1);
    vLine = valueMSB + valueLSB;

    vLine = hexToDec(vLine);  // Rea data:  67 42 eb 05, what's 42 eb?

    switch ( vLine.length() ) {
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

    valueLSB = decToHex(pkgRead[48], 1);
    valueMSB = decToHex(pkgRead[49], 1);
    iLine = valueMSB + valueLSB;

    iLine = hexToDec(iLine);

    switch ( iLine.length() ) {
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
    * Bytes 60, 61, 62 and 63 are also intended for
    * operations counter (Check what will happen here!)
    */
    valueLSB = decToHex(pkgRead[60], 1);
    valueMSB = decToHex(pkgRead[61], 1);
    opCounterMult = valueMSB + valueLSB;
    opCounterMult = hexToDec(opCounterMult);

    valueLSB1 = decToHex(pkgRead[65], 1);
    valueMSB1 = decToHex(pkgRead[64], 1);
    operationsCounter = valueMSB1 + valueLSB1;
    operationsCounter = hexToDec(operationsCounter);

    operationsCounter = opCounterMult + operationsCounter;

    switch ( operationsCounter.length() ) {
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

    valueLSB = decToHex(pkgRead[114], 1);
    valueMSB = decToHex(pkgRead[115], 1);
    currentTap = valueMSB + valueLSB;

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

    if (currentTap.substring(0, 1) == "0") {
        currentTap = hexToDec(currentTap.substring(1));
        if (currentTap.length() == 1) {
            currentTap = "B0" + currentTap;
        } else {
            currentTap = "B" + currentTap;
        }
    }

    valueLSB = decToHex(pkgRead[119], 1);
    valueMSB = decToHex(pkgRead[120], 1);
    minTap = valueMSB + valueLSB;

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

    if (minTap.substring(0, 1) == "0") {
        minTap = hexToDec(minTap.substring(1));
        if (minTap.length() == 1) {
            minTap = "B0" + minTap;
        } else {
            minTap = "B" + minTap;
        }
    }

    /*   
    * TAP alarm
    */
    dia    = pkgRead[128];
    mes    = pkgRead[131];
    hora   = pkgRead[125];
    minuto = pkgRead[122];

    if ((dataTapMin == 0) && (horaTapMin == 0)) {
        dataTapMin = dia + mes;
        horaTapMin = hora + minuto;
    }

    if ((dataTapMin != (dia + mes)) || (horaTapMin != (hora + minuto))) {
        dataTapMin = dia + mes;
        horaTapMin = hora + minuto;
        pkgToSendG = "FBB" + minTap ;
        tapMinpkg = true;
        blockpkgE = true;
    }

    valueLSB = decToHex(pkgRead[139], 1);
    valueMSB = decToHex(pkgRead[140], 1);
    maxTap = valueMSB + valueLSB;

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

    if (maxTap.substring(0, 1) == "0") {
        maxTap = hexToDec(maxTap.substring(1));
        if (maxTap.length() == 1) {
            maxTap = "B0" + maxTap;
        } else {
            maxTap = "B" + maxTap;
        }
    }

    hora   = pkgRead[145];
    minuto = pkgRead[142];
    dia    = pkgRead[148];
    mes    = pkgRead[151];

    if ((dataTapMax == 0) && (horaTapMax == 0)) {
        dataTapMax = dia + mes;
        horaTapMax = hora + minuto;
    }

    if ((dataTapMax != (dia + mes)) || (horaTapMax != (hora + minuto))) {
        dataTapMax = dia + mes;
        horaTapMax = hora + minuto;
        pkgToSendF = "FAA" + maxTap;
        tapMaxpkg = true;
        blockpkgD = true;
    }

    pkgToSendA = "A" + vCharge + iCharge + currentTap;
    pkgToSendB = "B" + iCharge + vSource + maxTap;
    pkgToSendC = "C" + vLine + iLine + minTap;
    pkgToSendD = "D0" + operationsCounter + iSource;
  
    if (idtData == 1) {
        pkgASending = true;
        blockpkgA = true;
        timeSigFox = 0;
    }

    if (idtData == 2) {
        pkgBSending = true;
        timeSigFox = 0;
        pkgNumSending = 0;
    }
    
    lendoSF = false;
}

/*
* Function for transforming hexadecimal numbers into decimals
*/
unsigned int hexToDec( String hexString ) {
    unsigned int decValue = 0;
    int nextInt;
    for (int i = 0; i < hexString.length(); i++) {
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

/*
* Function for transforming decimals numbers into hexadecimal
*/
String decToHex( long decValue, byte desiredStringLength ) {
    String hexString = String(decValue, HEX);
    while (hexString.length() < desiredStringLength)
        hexString = "0" + hexString;
    return hexString;
}

/* 
* Function to separate a string using a predetermined separator
*/
String getValue( String data, char separator, int index ) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

/*
*  Function for changing the voltage regulator parameters from the values received through the GSM network         
*       valuesToChange: It must contain valid values for each parameter, separated by the letter "F"
*/
void changeParameters(String valuesToChange) {

    unsigned short crcValue;
    int address = 0, valueMSB = 0, valueLSB = 0, lsbCRC = 0, msbCRC = 0;

    byte basePkgToWrite[23] = {5, 100, 16, 196, 1, 0, 1, 0, 143, 95, 192, 192, 5, 41, 2, 23, 1, address, valueMSB, valueLSB, 0};

    String lsbRLV = "", msbRLV = "", lsbRLI = "", msbRLI = "", 
                tempRLV = "", tempRLI = "", strRLV = "", strRLI = "", tmpHexCRC = "";

    String vRef = getValue(valuesToChange, 'F', 0);
    String insensitivity = getValue(valuesToChange, 'F', 1);
    String bloqTapMax = getValue(valuesToChange, 'F', 2);
    String bloqTapMin = getValue(valuesToChange, 'F', 3);
    String timer = getValue(valuesToChange, 'F', 4);
    String RLV = getValue(valuesToChange, 'F', 5); //  relationship TP
    String RLI = getValue(valuesToChange, 'F', 6); //  relationship TC

    switch ( parameterChange ) {
        case 0:
            if (timeChange == 1) {
                //sigfoxSerial.println(vRef);
                basePkgToWrite[17] = 1;
                basePkgToWrite[18] = vRef.toInt();
                basePkgToWrite[19] = 0;

                // Separates only the data needed to calculate the CRC
                for (int i = 10; i <= sizeof(basePkgToWrite); i++) {
                    for (int x = hack; x <= 10; x++) {
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
                parameterChange = 1;
            }
            break;

        case 1:
            if (timeChange == 2) {
                // Change insensitivity
                basePkgToWrite[17] = 2;
                basePkgToWrite[18] = insensitivity.toInt();
                basePkgToWrite[19] = 0;

                // Separates only the data needed to calculate the CRC
                for (int i = 10; i <= sizeof(basePkgToWrite); i++) {
                    for (int x = hack; x <= 10; x++) {
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

                // Add CRC to basePkgToWrite
                basePkgToWrite[21] = lsbCRC;
                basePkgToWrite[22] = msbCRC;

                Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

                lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
                tmpHexCRC = "";
                parameterChange = 2;
            }
            break;

        case 2:
            if (timeChange == 3) {
                // Change bloqTapMax
                basePkgToWrite[17] = 13;
                basePkgToWrite[18] = bloqTapMax.toInt();
                basePkgToWrite[19] = 0;

                // Separates only the data needed to calculate the CRC
                for (int i = 10; i <= sizeof(basePkgToWrite); i++) {
                    for (int x = hack; x <= 10; x++) {
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

                // Add CRC to basePkgToWrite
                basePkgToWrite[21] = lsbCRC;
                basePkgToWrite[22] = msbCRC;

                Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

                lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
                tmpHexCRC = "";
                parameterChange = 3;
            }
            break;

        case 3:
            if (timeChange == 4) {
                // Change bloqTapMin
                basePkgToWrite[17] = 14;
                basePkgToWrite[18] = bloqTapMin.toInt();
                basePkgToWrite[19] = 0;

                // Separates only the data needed to calculate the CRC
                for (int i = 10; i <= sizeof(basePkgToWrite); i++) {
                    for (int x = hack; x <= 10; x++) {
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

                // Add CRC to basePkgToWrite
                basePkgToWrite[21] = lsbCRC;
                basePkgToWrite[22] = msbCRC;

                Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

                lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
                tmpHexCRC = "";
                parameterChange = 4;
            }
            break;

        case 4:
            if (timeChange == 5) {
                //  Timer change
                basePkgToWrite[17] = 3;
                basePkgToWrite[18] = timer.toInt();
                basePkgToWrite[19] = 0;

                // Separates only the data needed to calculate the CRC
                for (int i = 10; i <= sizeof(basePkgToWrite); i++) {
                    for (int x = hack; x <= 10; x++) {
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

                // Add CRC to basePkgToWrite
                basePkgToWrite[21] = lsbCRC;
                basePkgToWrite[22] = msbCRC;

                Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

                lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
                tmpHexCRC = "";
                parameterChange = 5;
            }
            break;

        case 5:
            if (timeChange == 6) {
                // Change RLV
                int intRLV = RLV.toInt();
                int converted_msbRLV = 0;
                int converted_lsbRLV = 0;
                strRLV = "";

                strRLV = String(intRLV, HEX);

                if (strRLV.length() == 4) {
                    msbRLV = strRLV.substring(2);
                    converted_msbRLV = int(hexToDec(msbRLV));

                    lsbRLV = strRLV.substring(0, 2);
                    converted_lsbRLV = int(hexToDec(lsbRLV));
                }

                if (strRLV.length() == 3) {
                    msbRLV = strRLV.substring(1);
                    converted_msbRLV = int(hexToDec(msbRLV));

                    lsbRLV = strRLV.substring(0, 1);
                    converted_lsbRLV = int(hexToDec(lsbRLV));
                }

                if (strRLV.length() == 2) {
                    converted_msbRLV = int(hexToDec(strRLV));
                    converted_lsbRLV = 0;
                }

                basePkgToWrite[17] = 15;
                basePkgToWrite[18] = converted_msbRLV;
                basePkgToWrite[19] = converted_lsbRLV;

                // Separates only the data needed to calculate the CRC
                for (int i = 10; i <= sizeof(basePkgToWrite); i++) {
                    for (int x = hack; x <= 10; x++) {
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

                // Add CRC to basePkgToWrite
                basePkgToWrite[21] = lsbCRC;
                basePkgToWrite[22] = msbCRC;

                Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

                lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; valueMSB = 0; valueLSB = 0; hack = 0;
                tmpHexCRC = "";
                parameterChange = 6;
            }
            break;

        case 6:
            if (timeChange == 7) {
                // Change RLI
                int intRLI = RLI.toInt();
                int converted_msbRLI = 0;
                int converted_lsbRLI = 0;

                strRLV = "";
                strRLI = String(intRLI, HEX);

                if (strRLI.length() == 4) {
                    msbRLI = strRLI.substring(2);
                    converted_msbRLI = int(hexToDec(msbRLI));

                    lsbRLI = strRLI.substring(0, 2);
                    converted_lsbRLI = int(hexToDec(lsbRLI));
                }

                if (strRLI.length() == 3) {
                    msbRLI = strRLI.substring(1);
                    converted_msbRLI = int(hexToDec(msbRLI));

                    lsbRLI = strRLI.substring(0, 1);
                    converted_lsbRLI = int(hexToDec(lsbRLI));
                }

                if (strRLI.length() == 2) {
                    converted_msbRLI = int(hexToDec(strRLI));
                    converted_lsbRLI = 0;
                }

                basePkgToWrite[17] = 16;
                basePkgToWrite[18] = converted_msbRLI;
                basePkgToWrite[19] = converted_lsbRLI;

                // Separates only the data needed to calculate the CRC
                for (int i = 10; i <= sizeof(basePkgToWrite); i++) {
                    for (int x = hack; x <= 10; x++) {
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

                // Add CRC to basePkgToWrite
                basePkgToWrite[21] = lsbCRC;
                basePkgToWrite[22] = msbCRC;

                Serial.write(basePkgToWrite, sizeof(basePkgToWrite));

                lsbCRC = 0; msbCRC = 0; crcValue = 0; address = 0; hack = 0; valueMSB = 0; valueLSB = 0;
                tmpHexCRC = "";
                parameterChange = 7;
            }
            break;

        case 7:
            if (timeChange == 8) {
                Serial.end();
                gsmSerial.println("AT+CMGS=\"+5548996137114\"");//envia para o COD 996319628
                delay(300);
                gsmSerial.print("TR"); //text content
                delay(300);
                gsmSerial.write(26);
                parameterChange = 8;
            }
            break;

        case 8:
            if (timeChange == 9) {
                Serial.begin(9600);
                timeChange = 0;
                changeGSM = false;
                parameterChange = 0;
                while (true);
            }
            break;
    }
}

/*
* Function for receiving data over the GSM network and checking whether or not it is a package designed to change parameters
*/
void receivedMSG() {
    msgGSM = gsmSerial.readString();
    for (int i = 0; i < msgGSM.length(); i++) {
        if (i > 50) {
            if (msgGSM[i] == 'R') {
                valueVerficator = msgGSM.substring(i);
                posCut = i;
                partonePackOk = true;
            }

            if (msgGSM[i] == 'E') {
                parttwoPackOk = true;
                valuesToChange = msgGSM.substring(posCut + 1, i);
                // sigfoxSerial.println(valuesToChange); // Tem que remover
            }
        }
    }
    if ((partonePackOk == true) && (parttwoPackOk == true)) {
        changeGSM = true;
        parameterChange = 0;
        timeChange = 0;
        msgGSM = "";
        pkgASending = false;
        pkgBSending = false;
        vChargepkg = false;
        tapMaxpkg = false;
        tapMinpkg = false;
        iChargepkg = false;
        pkgCycle = 0;
        sendCycle = false;
        timeSigFox = 0;
    } else {
        changeGSM=false;
        partonePackOk=false;
        parttwoPackOk=false;
    }
}

/*
* Function for initialization and configuration of GSM and Sigfox networks
*/
void initSystem() {
    if ( (timeInitialize == 1) && (SFStart == false) ) {
        SFStart = true;
    }
    if ( (timeInitialize == 5) && (gsmStart == 0) ) {
        gsmSerial.println("AT+CMGF=1"); // Configuring TEXT mode
        gsmStart = 1;
    }
    if ( (timeInitialize == 7) && (gsmStart == 1) ) {
        gsmStart = 2;
    }
    if ( (timeInitialize == 15) && (gsmStart == 2) ) {
        gsmSerial.println("AT+CNMI=2,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
        initializing = false;
        gsmStart = 0;
        lersigfox = true;
        idtData = 1;
    }
}

void loop() {
    if (initializing == true) {
        initSystem();
    } else {
        if (changeGSM == false) {
            if ( (lersigfox == true) && (idtData == 0) ) {
                delay(100);
                byte basePKG[] = {0x05, 0x64, 0x0b, 0xc4, 0x01, 0x00, 0x01, 0x00, 0xc2, 0x2e, 0xc0, 0xc0, 0x01, 0x3c, 0x01, 0x06, 0xff, 0x50};
                Serial.write(basePKG, sizeof(basePKG));
                lersigfox = false;
                index = 0;
                pkgCycle = 0;
                lendoSF = true;
            }

            if ( (lersigfox == true) && (idtData != 0) ) {
                delay(100);
                byte basePKG[] = {0x05, 0x64, 0x0b, 0xc4, 0x01, 0x00, 0x01, 0x00, 0xc2, 0x2e, 0xc0, 0xc0, 0x01, 0x3c, 0x01, 0x06, 0xff, 0x50};
                Serial.write(basePKG, sizeof(basePKG));
                lersigfox = false;
                index = 0;
                pkgCycle = 0;
                lendoSF = true;
            }
      
            while ( Serial.available() > 0 ) {
                    sb = Serial.read();
                    pkgRead[index] = sb;
                    index++;
            }

            if (index >= 247) {
                    dataAnalysis(idtData);
                    sigfoxSerial.println(index);
                    index = 0;
                    idtData = 0;
            }

            switch ( pkgCycle ) {
                case 0:
                    if (pkgASending == true) {
                        if ((blockpkgA == true) && (sendCycle == false)) {
                            sendToSigfox(pkgToSendA);
                            blockpkgA = false;
                            timeSigFox = 0;
                            sendCycle = true;
                        }
                        if (timeSigFox == 30) {
                            pkgASending = false;
                            pkgCycle = 1;
                            sendCycle = false;
                        }
                    } else {
                        pkgCycle = 1;
                    }
                    break;

                case 1:
                    if ( (pkgBSending == true) && (sendCycle == false) ) {
                        blockpkgB = true;
                        sendCycle = false;
                    } else {
                        pkgCycle = 2;
                    }
                    if (blockpkgB == true) {
                        if (pkgNumSending == 0) {
                            pkgNumSending = 1;
                            sendToSigfox(pkgToSendB);
                            timeSigFox = 0;
                        }
                        if ( (timeSigFox == 30) && (pkgNumSending == 1) ) {
                            sendToSigfox(pkgToSendC);
                            pkgNumSending = 2;
                            timeSigFox = 0;
                        }
                        if ( (timeSigFox == 30) && (pkgNumSending == 2) ) {
                            sendToSigfox(pkgToSendD);
                            pkgNumSending = 3;
                            timeSigFox = 0;
                        }
                        if ( (timeSigFox == 30) && (pkgNumSending == 3) ) {
                            pkgBSending = false;
                            sendCycle = false;
                            blockpkgB = false;
                            pkgCycle = 2;
                            while (true);
                        }
                    }
                    break;

                case 2:
                    if (vChargepkg == true)  {
                        if ((blockpkgC == true) && (sendCycle == false)) {
                            sendToSigfox(pkgToSendE);
                            blockpkgC = false;
                            timeSigFox = 0;
                            sendCycle = true;
                        }
                        if (timeSigFox == 30) {
                            vChargepkg = false;
                            pkgCycle = 3;
                            timeSigFox = 0;
                            sendCycle = false;
                        }
                    } else {
                        pkgCycle = 3;
                    }
                    break;

                case 3:
                    if (iChargepkg == true) {
                        if ((blockpkgF == true)  && (sendCycle == false)) {
                            sendToSigfox(pkgToSendE);
                            blockpkgF = false;
                            timeSigFox = 0;
                            sendCycle = true;
                        }
                        if (timeSigFox == 30) {
                            iChargepkg = false;
                            pkgCycle = 4;
                            sendCycle = false;
                        }
                    } else {
                        pkgCycle = 4;
                    }
                    break;    

                case 4:
                    if (tapMinpkg == true)  {
                        if ((blockpkgE == true) && (sendCycle == false)) {
                            sendToSigfox(pkgToSendG);
                            blockpkgE = false;
                            timeSigFox = 0;
                            sendCycle = true;
                        }
                        if (timeSigFox == 30) {
                            tapMinpkg = false;
                            pkgCycle = 5;
                            sendCycle = false;
                        }
                    } else {
                        pkgCycle = 5;
                    }
                    break;

                case 5:
                    if (tapMaxpkg == true)    {
                        if ((blockpkgD == true) && (sendCycle == false)) {
                            sendToSigfox(pkgToSendF);
                            blockpkgD = false;
                            timeSigFox = 0;
                            sendCycle = true;
                        }
                        if (timeSigFox == 30) {
                            tapMaxpkg = false;
                            pkgCycle = 0;
                            sendCycle = false;
                        }
                    } else {
                        pkgCycle = 0;
                    }
            }
        }
        //    ******************* Recpção GSM ************************
        if (lendoSF == false) {
            if (gsmSerial.available() > 0) {
                receivedMSG();
            }
            if (changeGSM == true) {
                changeParameters(valuesToChange);
            }

            if (msgGSM[52] == 'T') {
                gsmSerial.println("AT+CMGS=\"+5548996137114\"");
                delay(300);
                gsmSerial.print("Rodando"); //text content
                delay(300);
                gsmSerial.write(26);
                changeGSM = false;
            }

            if (msgGSM[52] == 'K') {
                gsmSerial.println("AT+CMGS=\"+5548996137114\"");
                delay(300);
                gsmSerial.print("Resetting..."); //text content
                delay(300);
                gsmSerial.write(26);
                changeGSM = false;
                while (true);
            }
        }
    }

    if (timestatus > 0) {
        digitalWrite(outStatus, HIGH);
    }
    if (timestatus > 1) {
        digitalWrite(outStatus, LOW);
        timestatus = 0;
    }

    wdt_reset();
}
