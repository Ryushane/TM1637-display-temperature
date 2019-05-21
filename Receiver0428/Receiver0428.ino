/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/
#include <TM1637Display.h>
#include <Arduino.h>
#include "CyclicQueue.h"
#include "esp_task_wdt.h"

TaskHandle_t Task1;

uint32_t size0  = 10000;
#define RXD2 16
#define TXD2 17
#define PERIOD 125

#define CLK 18
#define DIO 23
#define TEST_DELAY 2000

unsigned char buffer0[10000];
bool ok;
bool isfull = 0;
unsigned char popdata = 12;
unsigned char p;
long lastmicros;

unsigned int sps = 0;
unsigned int sps1 = 0,sps2=0;
unsigned long lasttime;

unsigned char rxdata = 0;
unsigned int temp = 0;

CyclicQueue<unsigned char>cq0(buffer0, size0);
volatile int interruptCounter;
int totalInterruptCounter;

TM1637Display display(CLK, DIO);
//uint8_t data[] = {0xff. 0xff, 0xff, 0xff};
//uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};
unsigned int displayNum = 0;

void setup() {
  Serial.begin(230400);
  Serial2.begin(115200, SERIAL_8N1,RXD2,TXD2);
  lastmicros=micros();
  display.setBrightness(4);
  
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 
}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
//  Serial.print("Task1 running on core ");
//  Serial.println(xPortGetCoreID());

  for(;;){
    if(temp <= 85){
      digFlash(temp);
    }
    delay(1000);
  }
  esp_task_wdt_feed();
}


void loop() {
    while (Serial2.available()) {
    rxdata = Serial2.read();
    if(rxdata == 255)
    {
      rxdata = Serial2.read();
      if(rxdata == 255)
      temp = Serial2.read();
//      displayNum0 = temp[0] - '0';
//      displayNum1 = temp[1] - '0';
//      displayNum = displayNum0*10 + displayNum1;
//      displayNum = temp - '0';
//      Serial.println(temp);
//      Serial.println(displayNum);
//      if(temp <= 85){
//        digFlash(temp);
//      }
    }
    else
    {
    cq0.push(rxdata);
    sps++;
    } 
 }
  long nowmicros=micros();
  if(nowmicros>=1048576&&lastmicros<=65536){
    lastmicros=nowmicros;
  }
  while(nowmicros-lastmicros>=PERIOD){

    ok = cq0.pop(&p);
    //Serial.println(p,HEX);
    if(!ok)
    dacWrite(25,p);
    sps1++;
    lastmicros+=PERIOD;
  }
  sps2++;
  if (millis() - lasttime >1000){
      lasttime = millis();
//      Serial.print(sps);
//      Serial.print("  ");
//      Serial.println(sps1);
     //Serial.print("  ");
      //Serial.println(nowmicros);
//      Serial.println(cq0.len);
      sps = 0; 
      sps1 = 0;
      sps2 = 0;
//      digFlash(temp);
    }
}

void digFlash(unsigned int displayNum){
  display.showNumberDec(displayNum, false); // Expect: _301

}
