#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "./ttgo.h"

int moistPin = 4;
int moistVal = 0;

int maxRange = 3000;


void initLora()
{

  //initDisplay();

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init())
  {
    Serial.println("LoRa radio init failed");
    while (1)
      ;
  }
  Serial.println("LoRa radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ))
  {
    Serial.println("setFrequency failed");
    while (1)
      ;
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void sendLora(char* msg){

  int size = strlen(msg);

  if(size> 251){
    char msgtrunc[251];
    memcpy ( msgtrunc, msg, 251 );
    rf95.send((uint8_t *)msgtrunc, 251);
    rf95.waitPacketSent();
  }else{
    rf95.send((uint8_t *)msg, size);
    rf95.waitPacketSent();
  }


}


void initDisplay(){
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW); // low to reset OLED
    delay(50);
    digitalWrite(OLED_RST, HIGH); // must be high to turn on OLED

    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup");

  initLora();

  Serial.println("Setup finished");
}
void loop()
{

  moistVal = analogRead(moistPin);
  //Serial.println(moistVal);

  //moistPin >= 3000 bem molhado
  //moistPin = 0 totalmente seco

  float range_humidade = maxRange / 5;

  //seco
  //um pouco seco
  //normal
  //humido
  //muito humido

  if (moistVal < range_humidade)
  {
    Serial.println("Solo seco");
    sendLora("seco");
    
  }
  else if (moistVal >= range_humidade * 2 && moistVal < range_humidade * 3)
  {
    Serial.println("Solo um pouco seco");
    sendLora("pouco seco");
  }
  else if (moistVal >= range_humidade * 3 && moistVal < range_humidade * 4)
  {
    Serial.println("Solo normal");
    sendLora("normal");
  }
  else if (moistVal >= range_humidade * 4 && moistVal < range_humidade * 5)
  {
    Serial.println("Solo umido");
    sendLora("umido");
  }
  else if (moistVal >= range_humidade * 5)
  {
    Serial.println("Solo muito umido");
    sendLora("muito umido");
  }

  delay(1000); 
  Serial.println("loop");


}

