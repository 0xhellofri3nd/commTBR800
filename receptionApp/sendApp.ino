
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
    Serial.println( "* [5] Limitador Max" );
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
            callousness();
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
    char correctAnswer;
    Serial.println( "-------------------------------------------" );
    Serial.println( "Parametro selecionado: Tensão REF" );
    Serial.println( "São aceitos valores inteiros de 100 à 130 " );
    Serial.println( "-------------------------------------------" );
    Serial.println( "-> Valor desejado: " );
    while ( ( correctAnswer < 100 ) || ( correctAnswer > 130 ) ){
        correctAnswer = Serial.parseInt();
        Serial.println(correctAnswer);
    }
    getResponse(correctAnswer);
}

void callousness(){
    // Insensibilidade
}
void timing(){
    // Temporização
}
void maxLimit(){

}
void minLimit(){
    
}


void getResponse( char okAns ){
    if ( okAns == 'a' ){
     Serial.println(okAns);
     Serial.println("Right answer!");// hooray
    } else {
       Serial.println(okAns);
     Serial.println("Wrong answer!");// boo
    }
    nParameter = 0;
}