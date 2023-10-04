#include <SoftwareSerial.h>

long Previous = 0;
long Previous1 = 0; 
int val = 0;

byte cmd1[] = {0xaF,0x3F,0x21,0x8D,0x0A};// открываем сессию
byte cmd2[] = {0x06,0x30,0x35,0xb1,0x8d,0x0a};// читаем тип счетчика
byte cmd3[] = {0x81,0xd2,0xb1,0x82,0xc5,0xd4,0x30,0x50,0xc5,0x28,0xa9,0x03,0xb7};// снимаем показания
byte cmd4[] = {0x81,0xd2,0xb1,0x82,0x56,0xcf,0xcc,0xd4,0x41,0x28,0xa9,0x03,0x5f};// напряжение
byte cmd5[] = {0x81,0xd2,0xb1,0x82,0xc3,0x55,0xd2,0xd2,0xc5,0x28,0xa9,0x03,0x5a};// ток 
byte cmd6[] = {0x81,0xd2,0xb1,0x82,0x50,0xcf,0xd7,0xc5,0x50,0x28,0xa9,0x03,0xe4}; //мощность

float U220, I220, P220, T, T1, T2, T3;
float values[3];
char strBuffer[20];

uint8_t valueIdx = 0, strBufferPos = 0, packetProcessing = false, catchedCR = false;
boolean packetProcessing_Volta = false, packetProcessing_Curre = false, packetProcessing_Power = false;

SoftwareSerial RS485 (14, 12); // RX, TX

void setup()
{
  RS485.begin(9600);
  Serial.begin(9600);
}
void loop()
{
   if (millis() - Previous > 1000)
  {
    Previous = millis();        
    val ++;
    switch (val) {   
      case 1:
         RS485.write (cmd1,5);
         break;
      case 2:
         RS485.write (cmd2,6);
         break;
      case 3:
         RS485.write (cmd3,13);
         break;
      case 4:
         RS485.write (cmd4,13);
         break;
      case 5:
         RS485.write (cmd5,13);
         break;
      case 6: 
         RS485.write (cmd6,13);
         break;         
      case 8:
        val = 0;
        break;
       }
   Serial.print(".");  
    }   
   if (RS485.available()>0)
    {
    char response = RS485.read();
    response &= 0x7F;// convert 8N1 to 7E1
      char inChar = response;
      response++;
    switch (inChar) {
      case 0x02:
        valueIdx = 0;
        packetProcessing = true;
        break;
      case 0x03:
        // packet processing is finished
        packetProcessing = false;
        break;
      case 0x0D:
        catchedCR = true;
        break;
      case 0x0A:
        if (packetProcessing && catchedCR) {
          catchedCR = false;
          valueIdx++;
        }
        break;
      case 'V':
        packetProcessing_Volta = true;
        break;
      case 'U':
        packetProcessing_Curre = true;
        break;
      case 'W':
        packetProcessing_Power = true;
        break;        
      case '(':
        if (packetProcessing) {
          strBufferPos = 0;
        }
        break;
      case ')':
        if (packetProcessing) {
          strBuffer[strBufferPos] = 0x00;
             if (packetProcessing_Volta) {U220 = atof(strBuffer);}
             if (packetProcessing_Curre) {I220 = atof(strBuffer);}
             if (packetProcessing_Power) {P220 = atof(strBuffer);}     
          values[valueIdx] = atof(strBuffer);
		  T1=values[1];
      T2=values[2];
		  T3=values[3];
      T=T1+T2+T3;
        packetProcessing_Volta = false; 
        packetProcessing_Curre = false; 
        packetProcessing_Power = false;
        }
        break;
      default:
        if (!packetProcessing) {
          break;
        }
        strBuffer[strBufferPos] = inChar;
        if (strBufferPos < sizeof(strBuffer)) {
          strBufferPos++;
        }
    }
  }

	
   if (millis() - Previous1 > 10000)
  {
    Serial.print(".");      Serial.print(".");        Serial.println(".");
    Serial.print("Volta= "); Serial.print(U220);      Serial.println(" V");
    Serial.print("Power= "); Serial.print(P220*1000); Serial.println(" W");
    Serial.print("Curre= "); Serial.print(I220);      Serial.println(" A");
    Serial.print("T= ");     Serial.print(T);         Serial.println(" kWt*h");
    Serial.print("T1= ");    Serial.print(T1);        Serial.println(" kWt*h");
    Serial.print("T2= ");    Serial.print(T2);        Serial.println(" kWt*h");
    Serial.print("T3= ");    Serial.print(T3);        Serial.println(" kWt*h");
    Previous1 = millis();        
    }	
}