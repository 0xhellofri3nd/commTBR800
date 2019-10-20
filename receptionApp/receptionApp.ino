/*
* Realiza a separação em STRING (valores decimais) dos bytes:
* Entradas e Saídas analogicas
* Entradas binárias
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

boolean debounce(boolean last){
    boolean current = digitalRead(btn);
    if(last != current){
        delay(3);                      
        current = digitalRead(btn);  
    }
    return current;
}

// Lê o dado em forma de String
void serialEvent(){
    String conteudo = "";
    byte caractere;
    byte ar[100];
    String letra;
    while( Serial.available() > 0 ) {
        caractere = Serial.read();
        delay(1);
        conteudo.concat(caractere); 
        
        aux = 1;
    }
  
    if ( aux == 1 ) {
        /*
        * Na separação do pacote em STRINGs, 
        * O valor final é aberto, isso é, o ultimo caracter é n+1 (" [ [ ")
        */

        Serial.println("");
        // Entrada Binária (LSB)
        String binario = conteudo.substring(37, 43);// Verificar se é LSB ou MSB
        Serial.print("Entrada Binaria: "); 
        Serial.println(binario);

        // Testar...
        //Serial.print(binario.toInt(), HEX);
        //Serial.print(bin2.toInt(), HEX); 

        String analogInputs = conteudo.substring(56, 194);
        Serial.print("Entrada Analógica: "); 
        Serial.println(analogInputs);

        String analogOutputs = conteudo.substring(229, 353);
        Serial.print("Saídas Analógica: "); 
        Serial.println(analogOutputs);

        aux=0;
    }  
    return conteudo;
}

void loop() {
  currentButton = debounce(lastButton);
  if(lastButton == LOW && currentButton == HIGH) { 
    Serial.println("Enviando...");
    Serial.write(message, sizeof(message)); 
   // leStringSerial();
      
  }
  lastButton = currentButton;
  //readSerialString();

}
