int nParameter;
int qResp;

void setup() {
    Serial.begin(9600);  
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
    Serial.println( "-------------------------------------------" );
   
    while( ( nParameter != '1' ) && ( nParameter != '2' ) && ( nParameter != '3' ) && ( nParameter != '4' ) && ( nParameter != '5' ) ){
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
    sendPkg(vRef, 1); 
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
      sendPkg( (int)insens, 2 );   
    } else {
      insens *= 10;
      sendPkg( (int)insens, 2 );   
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
    sendPkg(vTemp, 3); 
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
        sendPkg(vMaxLimit, 11); 
    }
    sendPkg(vMaxLimit, 11); 
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
        sendPkg(vMinLimit, 12); 
    }    
    sendPkg(vMinLimit, 12);    
}

void sendPkg(int value, int address){
    byte pkgWrite[ ] = {5, 100, 16, 196, 3, 0, 1, 0, 38, 151, 192 ,192, 5, 41, 2, 23,  1, address, value, 0, 0 ,47, 104 };
    switch (address){
    case 1:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Modificando a Tensão de referencia para: ");
        Serial.println(value);
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = value;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;
    case 2:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Modificando a Insensibilidade para: ");
        Serial.println((float)value/10); // show in decimal 
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = value;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;
    case 3:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Ajustando a Temporização para: ");
        Serial.println(value); // show in decimal 
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = value;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;
    case 11:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Ajustando VMA para: ");
        Serial.println(value); // show in decimal 
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = value;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;      
    case 12:
        Serial.println("");
        Serial.println( "-------------------------------------------" );
        Serial.print("Ajustando VMI para: ");
        Serial.println(value); // show in decimal 
        Serial.println("");
        Serial.println("");
        pkgWrite[18] = value;
        Serial.write( pkgWrite, sizeof( pkgWrite ) );
        break;          
    }
}