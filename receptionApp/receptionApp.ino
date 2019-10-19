/* 
 * Reading Serial data with array
 */

const int btn = 8;  
int  serIn;      
int  serInIndx  = 0;  
int  serOutIndx = 0;
int buttonState = 0; 
boolean lastButton = LOW;
boolean currentButton = LOW;  

byte serInString[400];
byte byteRead;
byte message[] = { 0x05,0x64,0x0b,0xc4,0x03,0x00,0x01,0x00,0x6b,0xe6,0xc0,0xc0,0x01,0x3c,0x01,0x06,0xff,0x50 };

void setup() {
  pinMode(btn, INPUT);
  Serial.begin(9600); 
}

boolean debounce( boolean last ){
    boolean current = digitalRead(btn);
    if( last != current ){
        delay(3);                      
        current = digitalRead(btn);  
    }
    return current;
}

void readSerialString () {
    byte sb;   
    if( Serial.available() ) { 
       while ( Serial.available() ){ 
          sb = Serial.read();             
          serInString[serInIndx] = sb;
          serInIndx++;  
       }
    }
    if( serInIndx > 0 ) {      
      for( serOutIndx=0; serOutIndx < serInIndx; serOutIndx++ ) {
          Serial.print( serInString[serOutIndx] );      
      }        
      serOutIndx = 0;
      serInIndx  = 0;  
    }
}

void loop () {
    currentButton = debounce(lastButton);
    if(lastButton == LOW && currentButton == HIGH) { 
        Serial.println("Enviando...");
        Serial.write(message, sizeof(message)); 
        //readPkg(); 
    }
    lastButton = currentButton;  
    readSerialString(); 
}
