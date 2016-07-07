/*
WebSocket for ESP-WROOM-02 ( ESP8266 ) Sample Sketch
for Beta version 1.39
---> https://www.mgo-tec.com
Please rewrite their own the ssid and password.
Please rewrite their own local IP address of "/data/spiffs_01.txt" in the sketch folder.
Use the SPIFFS file system uploader, please upload the spiffs_01.txt file to flash.
*/
#include <EasyWebSocket.h>
#include <ESP8266WiFi.h>
#include <Hash.h>

const char* ssid = "xxxx";
const char* password = "xxxx";

long CountTime;

byte cnt = 0;

EasyWebSocket ews;

String html_str1;
String html_str2;
String html_str3;
String html_str4;
String html_str5;
String html_str6;
String html_str7;

String ret_str;
String txt;

int PingSendTime = 30000;

#define ledPin1 12 //GPIO #12
#define ledPin2 13 //GPIO #13
#define ledPin3 14 //GPIO #14

void setup() 
{
  //The assignment in the Body element of the browser of HTML to a String variable
  html_str1 = "<body style='background:#000; color:#fff;'>\r\n";
  html_str1 += "<font size=3>\r\n";
  html_str1 += "ESP-WROOM-02(ESP8266)\r\n";
  html_str1 += "<br>\r\n";
  html_str1 += "EasyWebSocket Beta1.3 Sample\r\n";
  html_str1 += "</font><br>\r\n";
  html_str1 += ews.EWS_BrowserSendRate();
  html_str1 += "<br>\r\n";
  html_str1 += "from WROOM DATA =\r\n";
  html_str1 += ews.EWS_BrowserReceiveTextTag("wroomTXT",20,"RED");
  html_str1 += "<br>\r\n";
  html_str1 += "WS Status = \r\n";
  html_str1 += ews.EWS_Status_Text(20,"#FF00FF");
  html_str1 += "<br><br>\r\n";
  html_str1 += ews.EWS_TextBox_Send("txt1", "Hello Easy WebSocket Beta1.3","送信");
  html_str1 += "<br><br>\r\n";

  html_str2 = "LED \r\n";
  html_str2 += ews.EWS_On_Momentary_Button("ALL", "全点灯", 65,25,15,"#000000","#AAAAAA");
  html_str2 += ews.EWS_On_Momentary_Button("OUT", "全消灯", 65,25,15,"#FFFFFF","#555555");
  html_str2 += "<br>\r\n";
  
  html_str3 = "<br>LED BLUE... Dim\r\n";
  html_str3 += ews.EWS_Canvas_Slider_T("BLUE",200,40,"#777777","#0000ff");
  html_str3 += "<br>LED GREEN Dim\r\n";
  html_str3 += ews.EWS_Canvas_Slider_T("GREEN",200,40,"#777777","#00ff00");
  
  html_str4 = "<br>LED RED..... Dim\r\n";
  html_str4 += ews.EWS_Canvas_Slider_T("RED",200,40,"#777777","#ff0000");
  html_str4 += "<br>LED RGB..... Dim\r\n";
  html_str4 += ews.EWS_Canvas_Slider_T("_RGB",200,40,"#777777","#ffff00");
  
  html_str5 = "<br><br>\r\n";
  html_str5 += "BLUE... \r\n";
  html_str5 += ews.EWS_OnOff_Button("blue",60,25,15,"#FFFFFF","#0000FF");
  html_str5 += ews.EWS_Touch_Slider_T("blue", "Txt1");
  html_str5 += ews.EWS_Sl_Text("Txt1",15,"#5555FF");
  html_str5 += "<br><br>\r\n";
  html_str5 += "GREEN \r\n";
  html_str5 += ews.EWS_OnOff_Button("green",60,25,15,"#FFFFFF","#00FF00");
  html_str5 += ews.EWS_Touch_Slider_T("green", "Txt2");
  html_str5 += ews.EWS_Sl_Text("Txt2",15,"#00ff00");
  html_str5 += "<br><br\r\n>";
  
  html_str6 = "RED..... \r\n";
  html_str6 += ews.EWS_OnOff_Button("red",60,25,15,"#FFFFFF","#ff0000");
  html_str6 += ews.EWS_Touch_Slider_T("red", "Txt3");
  html_str6 += ews.EWS_Sl_Text("Txt3",15,"#ff0000");
  html_str6 += "<br><br>\r\n";
  html_str6 += "RGB..... \r\n";
  html_str6 += ews.EWS_Touch_Slider_BT("-RGB", "Txt4");
  html_str6 += ews.EWS_Sl_BoxText("Txt4",30,20,15,"#000000");
  html_str6 += "<br><br><br>\r\n";
  html_str6 += ews.EWS_WebSocket_Reconnection_Button("WS-Reconnect", 200, 40, 17);
  html_str6 += "<br><br>\r\n";  
  html_str6 += ews.EWS_Close_Button("WS CLOSE",150,40,17);
  html_str6 += ews.EWS_Window_ReLoad_Button("ReLoad",150,40,17);
  html_str6 += "</body></html>\r\n";

  html_str7 = ""; //The description here is if the string is too large. If you do not want to use it is empty.
  
  ews.AP_Connect(ssid, password);
  
  CountTime = millis();
}

void loop() {
  ews.EWS_HandShake(html_str1, html_str2, html_str3, html_str4, html_str5, html_str6, html_str7);

  String str;

  if(ret_str != "_close"){
    if(millis()-CountTime > 10){//Data transmission from WROOM (ESP8266) every 500ms
      if(cnt > 3){
        cnt = 0;
      }
      switch(cnt){
        case 0:
          str = txt;
          break;
        case 1:
          str = "WebSockets";
          break;
        case 2:
          str = "Hello!!";
          break;
        case 3:
          str = "World!!";
          break;
      }
      ews.EWS_ESP8266_Str_SEND(str, "wroomTXT");
      CountTime = millis();
      cnt++;
    }

    ret_str = ews.EWS_ESP8266CharReceive(PingSendTime);
    if(ret_str != "\0"){
      Serial.println(ret_str);
      if(ret_str != "Ping"){
        if(ret_str[0] != 't'){
          int ws_data = (ret_str[0]-0x30)*100 + (ret_str[1]-0x30)*10 + (ret_str[2]-0x30);
          switch(ret_str[4]){
            case 'B':
              LED_PWM(1, ledPin1, floor(ws_data/2));
              break;
            case 'G':
              LED_PWM(1, ledPin2, floor(ws_data/2));
              break;
            case 'R':
              LED_PWM(1, ledPin3, floor(ws_data/2));
              break;
            case '_':
              LED_PWM(2, 0, floor(ws_data/2));
              break;
            case 'b':
              LED_PWM(1, ledPin1, ws_data);
              break;
            case 'g':
              LED_PWM(1, ledPin2, ws_data);
              break;
            case 'r':
              LED_PWM(1, ledPin3, ws_data);
              break;
            case '-':
              LED_PWM(2, 0, ws_data);
              break;
            case 'A':
              LED_PWM(3, 0, ws_data);
              break;
            case 'O':
              LED_PWM(4, 0, ws_data);
              break;
          }
        }else if(ret_str[0] == 't'){
          txt = ret_str.substring(ret_str.indexOf('|')+1, ret_str.length()-1);
          Serial.println(txt);
        }
      }
    }
  }else if(ret_str == "_close"){
    CountTime = millis();
    ret_str = "";
  }
}

//**************************************************************
void LED_PWM(byte Led_gr, byte Led_Pin, int data_i)
{
Serial.println(data_i);
  switch(Led_gr){
    case 1:
      analogWrite(Led_Pin, data_i);
      break;
    case 2:
      if(data_i < 34){
        analogWrite(ledPin1, data_i*8);
        analogWrite(ledPin2, 0);
        analogWrite(ledPin3, 0);
      }else if(data_i > 33 && data_i < 67){
        analogWrite(ledPin2, (data_i-33)*8);
        analogWrite(ledPin1, 0);
        analogWrite(ledPin3, 0);
      }else if(data_i > 66){
        analogWrite(ledPin3, (data_i-66)*7.78);
        analogWrite(ledPin1, 0);
        analogWrite(ledPin2, 0);
      }
      break;
    case 3:
      analogWrite(ledPin1, 100);
      analogWrite(ledPin2, 100);
      analogWrite(ledPin3, 100);
      break;
    case 4:
      analogWrite(ledPin1, 0);
      analogWrite(ledPin2, 0);
      analogWrite(ledPin3, 0);
      break;
  }
}
