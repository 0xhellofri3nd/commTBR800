#include <SoftwareSerial.h>

// Alternative serial port
SoftwareSerial mySerial(2, 3); 

int nParameter;
int qResp;

void setup() {
    Serial.begin(9600);  
    mySerial.begin(9600);
}

void loop(){
    getParameter();
    if(nParameter > 0){
        parameterSetValue(nParameter);
    }
}

void getParameter() {
    Serial.println( "-------------------------------------------" );
    Serial.println( "Qual parametros deseja modificar?" );
    Serial.println( "* [1] Tensao REF" );
    Serial.println( "* [2] Insensibilidade" );
    Serial.println( "* [3] Teporizacao" );
    Serial.println( "* [4] Limitador Max" );
    Serial.println( "* [5] Limitador Min" );
    Serial.println( "* [6] Bloq. Tap Max" );
    Serial.println( "* [7] Bloq. Tap Min" );
    Serial.println( "* [8] RLV" );
    Serial.println( "* [9] RLI" );
    Serial.println( "-------------------------------------------" );
   
    while( ( nParameter != '1' ) && ( nParameter != '2' ) && ( nParameter != '3' ) && ( nParameter != '4' ) && ( nParameter != '5' ) && ( nParameter != '6' ) && ( nParameter != '7' ) && ( nParameter != '8' ) && ( nParameter != '9' ) ){
        nParameter = Serial.read();
    }
    return nParameter;
}

void parameterSetValue( byte selectedParameter ) {
    switch( selectedParameter ) {
        case '1':
            changeREFVoltage();
            break;
        case '2':
            Insensitivity();
            break;
        case '3':
            timing();
            break;
        case '4':
            maxLimit();
            break;
        case '5':
            minLimit();
            break;
        case '6':
            maxBTap();
            break;
        case '7':
            minBTap();
            break;            
        case '8':
            changeRlv();
            break;                
        case '9':
            changeRli();
            break;                  
    }
}

void changeREFVoltage() {
    int vRef = 0;
    Serial.println( "-------------------------------------------" );
    Serial.println( "Parametro selecionado: Tensão REF" );
    Serial.println( "São aceitos valores inteiros de 100 à 130 " );
    Serial.println( "-------------------------------------------" );
    Serial.println( "-> Valor desejado: " );
    
    while( ( vRef < 100 ) || ( vRef > 130 ) ){
        vRef = Serial.parseInt();
    }
    sendPkg(vRef, 0, 1); 
}

void Insensitivity(){
    float insens = 0;
    Serial.println( "-------------------------------------------" );
    Serial.println( "Parametro selecionado: Insensibilidade" );
    Serial.println( "São aceitos valores de 1.0 à 6.0 " );
    Serial.println( "-------------------------------------------" );
    Serial.println( "-> Valor desejado: " );
    
    while( ( insens < 1 ) || ( insens > 6 ) ){
        insens = Serial.parseFloat();
    }
    if ( insens == 1 || insens == 2 || insens == 3 || insens == 4 || insens == 5 || insens == 6 ) {
      sendPkg( (int)insens, 0,  2 );   
    } else {
      insens *= 10;
      sendPkg( (int)insens, 0, 2 );   
    }
}

void timing(){
    int vTemp = 0;
    Serial.println( "-------------------------------------------" );
    Serial.println( "Parametro selecionado: Temporização" );
    Serial.println( "São aceitos valores inteiros de 10 à 120 " );
    Serial.println( "-------------------------------------------" );
    Serial.println( "-> Valor desejado: " );

    while( ( vTemp < 10 ) || ( vTemp > 120 ) ){
        vTemp = Serial.parseInt();
    }
    sendPkg(vTemp, 0, 3); 
}

void maxLimit(){
    int vMaxLimit = 0;
    int aux = 0;
    Serial.println( "--------------------------------------------------------------------------" );
    Serial.println( "Parametro selecionado: Limitador de tensão MAX (VMA)" );
    Serial.println( "São aceitos valores inteiros de 120 à 135 (Para desligar, utilize o valor 10)" );
    Serial.println( "--------------------------------------------------------------------------" );
    Serial.println( "-> Valor desejado: " );

    while( (( vMaxLimit < 120 ) || ( vMaxLimit > 135 )) && vMaxLimit != 10 ){
        vMaxLimit = Serial.parseInt();
    }
    // turn off the limiter
    if (vMaxLimit == 10){
        vMaxLimit = 0;
        sendPkg(vMaxLimit, 0, 11); 
    }
    sendPkg(vMaxLimit, 0, 11); 
}

void minLimit(){
    int vMinLimit = 0;
    Serial.println( "--------------------------------------------------------------------------" );
    Serial.println( "Parametro selecionado: Limitador de tensão MIN (VMI)" );
    Serial.println( "São aceitos valores inteiros de 105 à 118 (Para desligar, utilize o valor 10)" );
    Serial.println( "--------------------------------------------------------------------------" );
    Serial.println( "-> Valor desejado: " );

    while( (( vMinLimit < 105 ) || ( vMinLimit > 118 ))  && (vMinLimit != 10) ){
        vMinLimit = Serial.parseInt();
    }
    // turn off the limiter
    if (vMinLimit == 10){
        vMinLimit = 0;
        sendPkg(vMinLimit, 0, 12); 
    }    
    sendPkg(vMinLimit, 0, 12);    
}

void maxBTap(){
    int value;
    Serial.println( "-------------------------------------------" );
    Serial.println( "Parametro selecionado:  Bloqueio de Tap Max." );
    Serial.println( "Escolha o valor desejado: " );
    Serial.println( "* [1] 5%" );           // 08
    Serial.println( "* [2] 6 1/4%" );    // 0a
    Serial.println( "* [3]  7 1/2%" );   // 0c
    Serial.println( "* [4] 8 3/4%" );   // 0e
    Serial.println( "* [5] 10%" );        // 10
    Serial.println( "-------------------------------------------" );
   
    while( ( value != '1' ) && ( value != '2' ) && ( value != '3' ) && ( value != '4' ) && ( value != '5' ) ){
        value = Serial.read();
    }

    switch (value) {
    case '1':
        sendPkg(8, 0, 13);    
        break;
    case '2':
        sendPkg(10, 0, 13);
        break;
    case '3':
        sendPkg(12, 0, 13);    
        break; 
    case '4':
        sendPkg(14, 0, 13);    
        break;       
    case '5':
        sendPkg(16, 0, 13);    
        break;     
    }
}

void minBTap(){
    int value;
    Serial.println( "-------------------------------------------" );
    Serial.println( "Parametro selecionado:  Bloqueio de Tap Min." );
    Serial.println( "Escolha o valor desejado: " );
    Serial.println( "* [1] 5%" );           // 08
    Serial.println( "* [2] 6 1/4%" );    // 0a
    Serial.println( "* [3]  7 1/2%" );   // 0c
    Serial.println( "* [4] 8 3/4%" );   // 0e
    Serial.println( "* [5] 10%" );        // 10
    Serial.println( "-------------------------------------------" );
   
    while( ( value != '1' ) && ( value != '2' ) && ( value != '3' ) && ( value != '4' ) && ( value != '5' ) ){
        value = Serial.read();
    }

    switch (value) {
    case '1':
        sendPkg(8, 0, 14);    
        break;
    case '2':
        sendPkg(10, 0, 14);
        break;
    case '3':
        sendPkg(12, 0, 14);    
        break; 
    case '4':
        sendPkg(14, 0, 14);    
        break;       
    case '5':
        sendPkg(16, 0, 14);    
        break;
    }
}

void changeRlv(){
    float rlvValue = 0;
    Serial.println( "-------------------------------------------" );
    Serial.println( "Parametro selecionado: RLV" );
    Serial.println( "São aceitos valores de 25.0 à 209.0 " );
    Serial.println( "-------------------------------------------" );
    Serial.println( "-> Valor desejado: " );
    
    while( ( rlvValue < 25 ) || ( rlvValue > 209 ) ){
        rlvValue = Serial.parseFloat();
    }
    /*
    if ( ( (int)(rlvValue * 10) % 10 ) == 0) {
      sendPkg( (int)rlvValue, 15 );   
    } else {
      rlvValue *= 10;
      sendPkg( (int)rlvValue, 15 );   
    }
    */
    rlvValue *= 10;
    byte tempBuff[2];
    tempBuff[0] = (int)rlvValue & 255;
    tempBuff[1] = ((int)rlvValue >> 8)  & 255;
    sendPkg( tempBuff[0], tempBuff[1], 15 );   
}

void changeRli(){
    int rliValue = 0;
    Serial.println( "-------------------------------------------" );
    Serial.println( "Parametro selecionado: RLI" );
    Serial.println( "São aceitos valores de 250 à 6000 " );
    Serial.println( "-------------------------------------------" );
    Serial.println( "-> Valor desejado: " );
    
    while( ( rliValue < 250 ) || ( rliValue > 6000 ) ){
        rliValue = Serial.parseInt();
    }
    byte tempBuff[2];
    tempBuff[0] = rliValue & 255;
    tempBuff[1] = ( rliValue >> 8 ) & 255;
    sendPkg( tempBuff[0], tempBuff[1], 16 );   
}

void sendPkg(int valueMSB, int valueLSB, int address){
    byte pkgWrite[ ] = {5, 100, 16, 196, 3, 0, 1, 0, 38, 151, 192 ,192, 5, 41, 2, 23,  1, address, valueMSB, valueLSB, 0 ,47, 104 };
    switch (address){
    case 1:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Modificando a Tensão de referencia para: ");
        Serial.println(valueMSB);
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = valueMSB;
        pkgWrite[19] = valueLSB;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        //  Change parameter by alternative serial port
        // Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;
    case 2:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Modificando a Insensibilidade para: ");
        Serial.println((float)valueMSB/10); // show in decimal 
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = valueMSB;
        pkgWrite[19] = valueLSB;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;
    case 3:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Ajustando a Temporização para: ");
        Serial.println(valueMSB); // show in decimal 
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = valueMSB;
        pkgWrite[19] = valueLSB;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;
    case 11:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Ajustando VMA para: ");
        Serial.println(valueMSB); // show in decimal 
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = valueMSB;
        pkgWrite[19] = valueLSB;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;      
    case 12:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Ajustando VMI para: ");
        Serial.println(valueMSB); // show in decimal 
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = valueMSB;
        pkgWrite[19] = valueLSB;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;          
    case 13:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.println("Ajustando Bloq. Tap Max: ");
        pkgWrite[18] = valueMSB;
        pkgWrite[19] = valueLSB;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;   
    case 14:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.println("Ajustando Bloq. Tap Min: ");
        pkgWrite[18] = valueMSB;
        pkgWrite[19] = valueLSB;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;   
    case 15:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.println("Ajustando RLV: ");
        pkgWrite[18] = valueMSB;
        pkgWrite[19] = valueLSB;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;       
    case 16:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.println("Ajustando RLI: ");
        pkgWrite[18] = valueMSB;
        pkgWrite[19] = valueLSB;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;                    
    }
}