#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(11, 10); //SIM800L Tx & Rx is connected to Arduino #3 & #2

char dado;
void setup()
{
  pinMode(13,OUTPUT);
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
}

void loop()
{
   String comp = "";
     if (mySerial.available() > 0) {

    String msggsm = "";
    while (mySerial.available() > 0)
    {
      char letra = mySerial.read();
      if (isAlphaNumeric(letra)) msggsm += letra;
    }
    int tam = msggsm.length();


    comp += msggsm[tam - 2];
    comp += msggsm[tam - 1];

    Serial.print("MSG1: ");
    Serial.println(msggsm);

    Serial.print("comp: ");
    Serial.println(comp);    

    if (comp=="AA")
    {
      digitalWrite(13,1);
      mySerial.println("AT+CMGS=\"+5548998219908\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
      updateSerial();
      mySerial.print("Ligou"); //text content
      updateSerial();
      mySerial.write(26);  

    }
    if (comp=="BB")
    {
      digitalWrite(13,1);
      mySerial.println("AT+CMGS=\"+5548998219908\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
      updateSerial();
      mySerial.print("Desligou"); //text content
      updateSerial();
      mySerial.write(26);  
      digitalWrite(13,0);
    }
    delay(300);
  }

}

void updateSerial() 
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
