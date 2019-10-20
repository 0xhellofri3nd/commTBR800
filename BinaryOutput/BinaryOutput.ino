/*
* Data coming from the control is read correctly.
* Here you can access each part of the string separately.
*/

const int btn = 8;  
int aux = 0;
int  index  = 0;
int buttonState = 0; 
int  serOutIndx = 0;
boolean lastButton = LOW;
boolean currentButton = LOW;  
byte message[] = {0x05,0x64,0x0b,0xc4,0x03,0x00,0x01,0x00,0x6b,0xe6,0xc0,0xc0,0x01,0x3c,0x01,0x06,0xff,0x50};
byte pkg[400];      
String dado = "";

void setup() {
    pinMode(btn, INPUT);
    Serial.begin(9600);
    // attachInterrupt(digitalPinToInterrupt(btn), leStringSerial, RISING);

}

boolean debounce( boolean last ){
    boolean current = digitalRead(btn);
    if( last != current ){
        delay(3);                      
        current = digitalRead(btn);  
    }
    return current;
}

// Read the string data
String leStringSerial(){
    Serial.write(message, sizeof(message));  
    String conteudo = "";
    byte caractere;
    byte ar[100];
    String letra;
    while ( Serial.available() > 0 ) {
        caractere = Serial.read();
        delay(1);
        conteudo.concat(caractere); 
        aux = 1;
  }
  
    if ( aux == 1 ) {
        Serial.print(conteudo);
        Serial.println("");
        //Serial.print(conteudo[41]);  
        //Serial.print("");
    
        // Binary Output
        String bin1 = conteudo.substring( 37, 40 );
        String bin2 = conteudo.substring( 40, 43 );
        Serial.print( bin1.toInt(), HEX );
        Serial.print( bin2.toInt(), HEX ); 

        aux = 0;
    }  
  
  return conteudo;
}

// Read the data and put it in a byte array
void readSerialString () {
    byte sb;   
    String dado = "";
    if( Serial.available() ) { 
        while ( Serial.available() ){ 
            sb = Serial.read();             
            pkg[index] = sb;
            index++;  
        }
    }
    
    if( index > 0) {      
        for( int i = 0; i < index; i++ ){
            // dado.concat(pkg[i]);
            Serial.print(pkg[i], HEX);
        }       
        // Serial.print(dado.substring(0, 4));    
        index  = 0;  
    }
}

void loop() {
    currentButton = debounce(lastButton);
    if( lastButton == LOW && currentButton == HIGH ) { 
        Serial.println("Enviando...");
        Serial.write(message, sizeof(message)); 
        leStringSerial();
      
    }
    lastButton = currentButton;
    //readSerialString();
}
