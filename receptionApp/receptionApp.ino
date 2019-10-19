const int btn = 4;  
byte byteRead;
int buttonState = 0; 

boolean lastButton = LOW;
boolean currentButton = LOW;  
                                                             
byte message[] = { 0x05,0x64,0x0b,0xc4,0x03,0x00,0x01,0x00,0x6b,0xe6,0xc0,0xc0,0x01,0x3c,0x01,0x06,0xff,0x50 };
char pkg[500];

void setup() {                 
    Serial.begin(9600);
    pinMode(btn, INPUT);
}

boolean debounce( boolean last ){
    boolean current = digitalRead(btn);
    if( last != current ){
        delay(3);                      
        current = digitalRead(btn);  
    }
    return current;
}
 
void loop() {
    currentButton = debounce(lastButton);
    if( lastButton == LOW && currentButton == HIGH ) { 
        Serial.println("");
        Serial.println("Enviando...");
        Serial.write(message, sizeof(message));     
    }
    lastButton = currentButton;

    if ( Serial.available() > 0 ){        
        byteRead = Serial.read();
        sprintf(pkg, "%02x", byteRead);
        Serial.print(pkg);
    }
    
}