
//ALL LIBRARIES

#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//END LIBRARIES


//START PIN DECLARATIONS

//bool sendingSMS = false;
String msg = "";
#define RXD2 16
#define TXD2 17

////////////////////////////////////////////////////////////////
const unsigned long interval = 120000; //600000 milliseconds == 10min

unsigned long sms_sent_currentMillis = 0;
unsigned long sms_sent_previousMillis = 0;
bool SMSSentStatus = false ;

String phoneNumber = "+2347068021798" //subject to change phoneNumber

//int shiftCursor = 5;

#define NH3Pin 33
#define buzzerPin 25 //digital pin 25,26
#define DHTPIN 27 
    
#define DHTTYPE DHT11
;DHT dht(DHTPIN, DHTTYPE);

byte deg[]={
  B11100,
  B10100,
  B11100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
  };

//END PIN DECLARATIONS



//FUNTION TO GET AMOUNT OF AMONIA SMELL

int getNH3(){
int odor = analogRead(NH3Pin);
return odor;
  }
  
//END FUNTION TO GET AMOUNT OF AMONIA SMELL



//FUNCTION TO GET HUMIDITY AND TEMPERATURE

void dhtSetup() {
  Serial.println(F("DHTxx test!"));
  dht.begin();
}
int getTempt(){
   delay(1000);
    int t = dht.readTemperature();// Read temperature as Celsius (the default)
  if (isnan(t)){
    Serial.println(F("Failed to read tempt from DHT sensor!"));
     return 0;
  }
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  return t;
  }
  
int getHumidity(){
   delay(1000);
     int h = dht.readHumidity();  // Read humidity !
  if (isnan(h)){
    Serial.println(F("Failed to read humidity from DHT sensor!"));
    return 0.00;
  }
  
  Serial.print(F("Humidity: "));
  Serial.println(h);
  return h;
  }


//FUNCTION TO DISPLAY DATA
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars(coloumn) and 2 line display(rows)

void displaySetup(){
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.createChar(3, deg);
 
  lcd.setCursor(0,0);
  lcd.print("Loading.");
  lcd.setCursor(0,1);
  lcd.print("getting T & H ");
  delay(3000);
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Loading..");
  lcd.setCursor(0,1);
  lcd.print("getting NH3 lv");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Loading...");
  lcd.setCursor(0,1);
  lcd.print("getting day num ");
  delay(1000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("connecting...");
  lcd.setCursor(0,1);
  lcd.print("to network ");
  delay(1000);
  }

 void displayData(int t, int h, int o, int d){
  lcd.clear();

//calls temperature and humidity function
 //display tempture level
  lcd.setCursor(0,1);
  lcd.print("T=");
  lcd.setCursor(2,1);
  lcd.print(t);
  lcd.setCursor(4,1);
  lcd.write(3);
  lcd.setCursor(5,1);
  lcd.print("C");

//display humidity level
  lcd.setCursor(8,1);
  lcd.print("H=");
  lcd.setCursor(10,1);
  lcd.print(h);
  lcd.setCursor(12,1);
  lcd.print("%");


//display NH3 level
//calls NH3 leve function
  lcd.setCursor(0,0);
  lcd.print("NH3=");
  lcd.setCursor(4,0);
  lcd.print(o);
  lcd.setCursor(6,0);
  lcd.print("ppm");


//display day count
//calls timer function
  lcd.setCursor(10,0);
  lcd.print("Day=");
  lcd.setCursor(14,0);
  lcd.print(d);
  delay(5000);
  }

//END FUNCTION TO DISPLAY DATA


//DAY COUNT
unsigned long previousMillis = 0;
const unsigned long hour_interval =300000;  //86400000; //24*60*60*1000;
int dayCount=0;

int getDayCount(){
unsigned long currentMillis= millis();
   if (currentMillis - previousMillis >= hour_interval) {
     dayCount++;
       previousMillis = currentMillis;
      }
  return dayCount; 
  }

//END DAY COUNT



//ALARM
void send_alarm(){
  tone(buzzerPin, 70, 500);
  delay(2500);
  noTone(buzzerPin);
  }
//END ALARM


//BEGIN SIM MODULE FUNTIONS FOR SMS

void simModuleInitializiation() {
  Serial.println("Initializing....");
  initModule("AT", "OK", 1000);
  initModule("ATE1", "OK", 1000);
  initModule("AT+CPIN?", "READY", 1000);
  initModule("AT+CMGF=1", "OK", 1000);

  initModule("AT+CMGD=1,4", "OK", 1000);
  initModule("AT+CNMI=1,2,0,0,0", "OK", 1000);
  delay(200);
}

void initModule(String cmd, char *res, int t) { // function to initialize sim module for SMS setup
  while (1) {
    Serial.println(cmd);
    Serial2.println(cmd);
    delay(1000);
    while (Serial2.available() > 0) {
      if (Serial2.find(res)) {
        Serial.println(res);
        delay(t);
        return;
      }
      else {
        Serial.println("Error");
      }
    }
    delay(t);
  }
}

void SendSMS(String msg) { //Send SMS function
  delay(100);
  Serial2.println("AT");
  delay(50);
  Serial2.println("AT+CMGF=1");
  delay(50);
  //  phoneNumber = phoneNumber.substring(0, 14);
  Serial.println("---");
  Serial.println(phoneNumber);
  Serial2.print("AT+CMGS=");
  Serial2.print('"');
  Serial2.print(phoneNumber);    //mobile no. for SMS alert
  Serial2.println('"');
  delay(50);
  Serial2.println(msg);
  delay(50);
  Serial2.write(26);
  delay(50);
}//end sendSMS

//END SMS SIM MODULE FUNCTIONS


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dhtSetup();
  
  pinMode(NH3Pin, INPUT);
  pinMode(buzzerPin, OUTPUT);

   displaySetup();// call display setups
  
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  simModuleInitializiation();
}

void loop() {
  // put your main code here, to run repeatedly:

//getTemptHumidity();
int tempt = getTempt();
int hum = getHumidity();
int NH3 = getNH3();
int day_count = getDayCount();

displayData(tempt, hum, NH3, day_count);


if (SMSSentStatus) {
      sms_sent_currentMillis = millis();
      if (sms_sent_currentMillis - sms_sent_previousMillis >= interval) {
        sms_sent_previousMillis = sms_sent_currentMillis;
        SMSSentStatus = false;
      }
    }else{

if(tempt >= 38 ){
  send_alarm();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tempt too high "); //print to lcd
  Serial.println("Tempt too high"); //print to serial monitor
  lcd.setCursor(0,1);   
  lcd.print("Sending SMS..."); //print to lcd
  Serial.println("Sending SMS..."); //print to serial monitor
  delay(2000);
  msg="Tempture too high, Birds may be in danger";
  Serial.println(msg); //print to serial monitor
  SendSMS(msg); //call send msg
  SMSSentStatus = true;
  }

if(hum >=75 ){
 send_alarm();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity too low "); //print to lcd
  Serial.println("Humidity too low"); //print to serial monitor
  lcd.setCursor(0,1);   
  lcd.print("Sending SMS..."); //print to lcd
  Serial.println("Sending SMS..."); //print to serial monitor
  delay(2000);
  msg="Humidity too High, Birds may be in danger";
  Serial.println(msg); //print to serial monitor
  SendSMS(msg); //call send msg
  SMSSentStatus = true;
  }
  
//  if(NH3 >= 25 ){
//   send_alarm();
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("Amonia too high "); //print to lcd
//  Serial.println("Amonia level too high "); //print to serial monitor
//  lcd.setCursor(0,1);   
//  lcd.print("Sending SMS..."); //print to lcd 
//  Serial.println("Sending SMS..."); //print to s erial monitor
//  delay(2000); 
//  msg="Amonia level too high, Birds may be in danger";
//  Serial.println(msg); //print to serial monitor
//  SendSMS(msg); //call send msg
//    SMSSentStatus = true;
//  }
      }


//print all to serial monitor
Serial.println();
Serial.print("DAY Count = ");
 Serial.print(getDayCount()); 

Serial.println();
Serial.print("NH3 = ");
 Serial.println(getNH3()); 

  getTempt();
  getHumidity();
  delay(1000);
//end print all to serial monitor
}
