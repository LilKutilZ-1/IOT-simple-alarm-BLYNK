#define BLYNK_TEMPLATE_ID "TMPL6ja0rU775"
#define BLYNK_TEMPLATE_NAME "AlarmEndraV2"
#define BLYNK_AUTH_TOKEN "8GMK4qrOKqRtHG6Z2VhgVv8x7PAcxHm2"
#define BLYNK_PRINT Serial 

#include <SoftwareSerial.h> //komunikasi pin digital 
#include <DFPlayer_Mini_Mp3.h> 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
// cccccc

SoftwareSerial serial(  2, 4);            //Deklarasi pin TX dan RX 
  
char auth[] = "8GMK4qrOKqRtHG6Z2VhgVv8x7PAcxHm2";
//nama dan password wifi
char ssid[] = "STAFF CROG";
char pass[] = "staffcrog";

#define JAM           V3

unsigned char start_time_hour_1;
unsigned char start_time_min_1;
unsigned char stop_time_hour_1;
unsigned char stop_time_min_1;
unsigned char day_timer_1;

long rtc_sec_server;
unsigned char weekday_server;
unsigned char status;
unsigned char Switch_Alarm;


BLYNK_CONNECTED()
{
  Serial.print("BLYNK SERVER CONNECTED !!!");
 Blynk.syncAll();

  Blynk.sendInternal("rtc", "sync");
}

BLYNK_WRITE(V1){ //swtich lagu
  int value = param.asInt();
  if(value == HIGH){
    mp3_play();
    Serial.println("Lagu Nyala");
  }else{
    mp3_stop();
    Serial.println("Lagu Mati");
  }
}
BLYNK_WRITE(V2){ // range volume 
int value = param.asInt();
mp3_set_volume(value);
    Serial.print("Range Volume : ");
    Serial.println(value);

}


BLYNK_WRITE(JAM)
{
  unsigned char week_day;
  
  TimeInputParam  t(param);
  
  if (t.hasStartTime() && t.hasStopTime())
  {
     start_time_hour_1 = t.getStartHour();
     start_time_min_1 = t.getStartMinute();
     Serial.println(String("Time1 Start: ") +
                     start_time_hour_1 + ":" +
                     start_time_min_1);
    
     stop_time_hour_1 = t.getStopHour();
     stop_time_min_1 = t.getStopMinute();
     Serial.println(String("Time1 Stop: ") +
                     stop_time_hour_1 + ":" +
                     stop_time_min_1);
    
     for (int i = 1; i <= 7; i++)
     {
       if (t.isWeekdaySelected(i))  // will be "TRUE" if nothing selected as well
       {
         day_timer_1 |= (0x01 << (i-1));
       }
       else
         day_timer_1 &= (~(0x01 << (i-1)));
     }
    
     Serial.print("Time1 Selected Days: ");
     Serial.println(day_timer_1, HEX);
    // flag_timer1_en = 1;
  }4
  elserr5f
  {
   // flag_timer1_en = 0;
    Serial.println("Disabled Timer 1");
  }
}
BLYNK_WRITE(InternalPinRTC) 
{
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
  unsigned long blynkTime = param.asLong(); 
  
  if (blynkTime >= DEFAULT_TIME) 
  {
    setTime(blynkTime);

    weekday_server = weekday();
  
    if ( weekday_server == 1 )
      weekday_server = 7;
    else
      weekday_server -= 1; 
    
    rtc_sec_server = (hour()*60*60) + (minute()*60);
   
    Serial.println(blynkTime);
    Serial.println(String("RTC Server: ") + hour() + ":" + minute());
    Serial.println(String("Day of Week: ") + weekday()); 
  }
}



BLYNK_WRITE(V0){ // switch alarm
  int value = param.asInt();
  if (value == HIGH){
    Serial.println("ALARM ON");
    Switch_Alarm = 1;
    status = 0;
  }else{
    mp3_stop();
    Switch_Alarm = 0;
     
    status = 1;
    Serial.println("ALARM OFF");
  }
}
unsigned char time_10_sec;
void checkTime() 
{
  time_10_sec++;
  if(time_10_sec >= 10)
  {
    time_10_sec = 0;
    Blynk.sendInternal("rtc", "sync"); 
    long jam = start_time_hour_1*3600 + start_time_min_1*60;
    long stop = stop_time_hour_1*3600 + stop_time_min_1*60;
    
    Serial.println("Jam yang di pilih");
    Serial.println(jam);
    Serial.print("Status Alarm : ");
    Serial.println(status);
    if(Switch_Alarm == 1){
    if(day_timer_1 == 0x00 || (day_timer_1 & (0x01 << (weekday_server - 1) )) && jam == rtc_sec_server){
      if(status == 0){
        mp3_play();
        status = 1;
      }else {
        Serial.println("Alarm Lagi Berbunyi");
      }
      
    }else{
      Serial.println("Bukan Saat Alarm");
    }
    if (stop == rtc_sec_server){
        mp3_stop();
        status = 0;
        Serial.println("Alarm Di matikan");
        delay(60000);
      }

    }else{
      Serial.println("alarm Mati");
    }

  }
}
void setup() {
  serial.begin(9600);
  Serial.begin(9600);
 mp3_set_serial(serial);
Blynk.begin(auth, ssid, pass);

}
unsigned long ms_buf;
void loop() {
   unsigned long ms_dif = millis() - ms_buf;
  if ( ms_dif >= 1000 ) // 1 second
  {
     ms_buf = millis();
     checkTime();
     
     
  }
  Blynk.run();
  delay(50);
  // put your main code here, to run repeatedly:
}
