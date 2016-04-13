/*
  EasyWebsocket.cpp - WebSocket for ESP-WROOM-02 ( esp8266 )
  Beta version 1.35

Dual licensed under the MIT or GPL Version 2 (2.1) licenses.

Copyright (c) 2016 Mgo-tec
https://www.mgo-tec.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Includes ESP8266WiFi.h, Hash.h
https://github.com/esp8266/Arduino
Copyright (C) 1991, 1999 Free Software Foundation, Inc.
Released under the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
https://opensource.org/licenses/GPL-2.0

Includes FS.h
https://github.com/esp8266/arduino-esp8266fs-plugin
Copyright (C) 1989, 1991 Free Software Foundation, Inc.
Released under the GNU GENERAL PUBLIC LICENSE Version 2
https://opensource.org/licenses/GPL-2.0

*/

#include "Arduino.h"
#include "EasyWebsocket.h"

const char* GUID_str = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

WiFiClient client;
WiFiServer server(80);

EasyWebSocket::EasyWebSocket(){}

//********AP(Router) Connection****
void EasyWebSocket::AP_Connect(const char* ssid, const char* password)
{
  Serial.begin(115200);
  // Connect to WiFi network
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println(F("WiFi connected"));
  
  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
  delay(10);
}

//********WebSocket Hand Shake ****************
void EasyWebSocket::EWS_HandShake(String _res_html1, String _res_html2, String _res_html3, String _res_html4, String _res_html5, String _res_html6, String _res_html7)
{
  String req;
  String hash_req_key;
  long LoopTime = millis();
  
  if(!_WS_on){
    client = server.available();
  }
  
  if(client){
    LoopTime = millis();
    while(!_WS_on){

      if(millis()-LoopTime > 5000L){
        _WS_on = false;
        _Ini_html_on = false;
        _Upgrade_first_on = false;
        Serial.println(F("-----------------------Received TimeOut 1"));
        if(client){
          delay(10);
          client.stop();
          Serial.println(F("---------------------TimeOut Client Stop 1"));
        }
        Serial.println(F("-----------------------The Handshake returns to the beginning 1"));
        break;
      }

      delay(1);
      switch(_Ini_html_on){
        case false:
          LoopTime = millis();
          while(client){
            if(millis()-LoopTime > 5000L){
              _WS_on = false;
              _Ini_html_on = false;
              _Upgrade_first_on = false;
              Serial.println(F("-----------------------Received TimeOut 2"));
              if(client){
                delay(10);
                client.stop();
                Serial.println(F("---------------------TimeOut Client Stop 2"));
              }
              Serial.println(F("-----------------------The Handshake returns to the beginning 2"));

              break;
            }
            if(client.available()){
              req = client.readStringUntil('\n');

              if(req.indexOf("GET / HTTP") != -1){
                Serial.println(F("-------------------HTTP Request from Browser"));
                Serial.println(req);

                while(req.indexOf("\r") != 0){
                  req = client.readStringUntil('\n');
                  if(req.indexOf("Connection: Upgrade") != -1){
                    Serial.println(F("-------------------Connection: Upgrade HTTP Request"));
                    Serial.println(req);
                    _Ini_html_on = true;
                    _Upgrade_first_on = true;
                    EasyWebSocket::EWS_HTTP_Responce();
                    _PingLastTime = millis();
                    _PongLastTime = millis();
                    break;
                  }

                  if(req.indexOf("Android") != -1){
                    _Android_or_iPad = 'A';
                  }else if(req.indexOf("iPad") != -1){
                    _Android_or_iPad = 'i';
                  }else if(req.indexOf("iPhone") != -1){
                    _Android_or_iPad = 'P';
                  }
                  Serial.println(req);
                }
                req = "";
                
                if(_Upgrade_first_on == true)break;
                
                Serial.println(F("-------------------HTTP Response Send to Browser"));
                delay(10);

                client.print(F("HTTP/1.1 200 OK\r\n"));
                client.print(F("Content-Type:text/html\r\n"));
                client.print(F("Connection:close\r\n\r\n"));

                SPIFFS.begin();

                File f1 = SPIFFS.open("/spiffs_01.txt", "r");
                if(f1){
                  char c = f1.read();
                  client.print(c);
                  byte spi_count = 0;
                  while(c!='\0'){
                    c= f1.read();
                    if(c>0xDD) break;
                    if(c=='\n')spi_count++;
                    if(spi_count==10){
                      Serial.print('.');
                      spi_count = 0;
                    }
                    client.print(c);
                  }
                  Serial.println();

                  f1.close();
                  
                  client.print(_res_html1);
                  client.print(_res_html2);
                  client.print(_res_html3);
                  client.print(_res_html4);
                  client.print(_res_html5);
                  client.print(_res_html6);
                  client.print(_res_html7);
                  
                }else{
                  f1.close();
                  Serial.println(F("ERROR.\r\n spiffs_01.txt file has not been uploaded to the flash in SPIFFS file system"));
                  client.print(F("ERROR!!<br>spiffs_01.txt file has not been uploaded to the flash in SPIFFS file system"));
                }
                Serial.println(F("---------------------HTTP response complete"));

                _res_html1 = "";
                _res_html2 = "";
                _res_html3 = "";
                _res_html4 = "";
                _res_html5 = "";
                _res_html6 = "";
                _res_html7 = "";

                client.flush();
                delay(10);

                client.stop();

                delay(10);

                Serial.println(F("\n--------------------GET HTTP client stop"));
                req = "";
                _Ini_html_on = true;
                LoopTime = millis();

                if(_Android_or_iPad == 'i'){
                  break;
                }
              }else if(req.indexOf("GET /favicon") != -1){
                Serial.println(F("-----------------------Favicon GET Request Received 1"));
                _Ini_html_on = true;
                Serial.println(req);
                while(client.available()){
                  Serial.write(client.read());
                }
                
                client.print(F("HTTP/1.1 404 Not Found\r\n"));
                client.print(F("Connection:close\r\n\r\n"));
                
                delay(10);                
                client.stop();
                delay(10);
                client.flush();
                Serial.println(F("-----------------Favicon client.stop"));
                break;
              }
            }
          }
          break;
        case true:
          switch(_WS_on){
            case false:
              EasyWebSocket::EWS_HTTP_Responce();
              _PingLastTime = millis();
              _PongLastTime = millis();
              break;
            case true:
              Serial.println(F("-----------------WebSocket HandShake Complete!"));
              LoopTime = millis();
              break;
          }
          break;
      }
    }
  }
}

//************HTTP Response**************************
void EasyWebSocket::EWS_HTTP_Responce()
{  
  String req;
  String hash_req_key;
  long LoopTime = millis();
  
  if(_Upgrade_first_on != true){
    client = server.available();
  }

  while(client){
    if(millis()-LoopTime > 5000L){
      _WS_on = false;
      _Ini_html_on = false;
      _Upgrade_first_on = false;
      Serial.println(F("-----------------------Received TimeOut 3"));
      if(client){
        delay(10);
        client.stop();
        Serial.println(F("---------------------TimeOut Client Stop 3"));
      }
      Serial.println(F("-----------------------The Handshake returns to the beginning 3"));
      break;
    }

    if(client.available()){
      req = client.readStringUntil('\n');
      Serial.println(req);
      if (req.indexOf("websocket") != -1){
        Serial.println(F("---------------------Websocket Requests received"));
        Serial.println(req);
 
        while(req.indexOf("\r") != 0){
          req = client.readStringUntil('\n');
          Serial.println(req);
          if(req.indexOf("Sec-WebSocket-Key")>=0){
            hash_req_key = req.substring(req.indexOf(':')+2,req.indexOf('\r'));
            Serial.println();
            Serial.print(F("hash_req_key ="));
            Serial.println(hash_req_key);
          }        
        }
  
        delay(10);
        req ="";
  
        char h_resp_key[29];

        EasyWebSocket::Hash_Key(hash_req_key, h_resp_key);
        
        Serial.print(F("h_resp_key = "));
        Serial.println(h_resp_key);
        String str;

        str = "HTTP/1.1 101 Switching Protocols\r\n";
        str += "Upgrade: websocket\r\n";
        str += "Connection: Upgrade\r\n";
        str += "Sec-WebSocket-Accept: ";
        for(byte i=0; i<28; i++){
          str += h_resp_key[i];
        }

        str += "\r\n\r\n";
        
        Serial.println(F("\n--------------------WebSocket HTTP Response Send"));
        Serial.println(str);
        client.print(str);
        str = "";
  
        _WS_on = true;
        Serial.println(F("-------------------WebSocket Response Complete!"));
        break;
  
      }else if(req.indexOf("favicon") != -1){

        Serial.println(F("---------------------Favicon GET Request Received 2"));
        Serial.println(req);
        while(client.available()){
          Serial.write(client.read());
        }
        
        
        client.print(F("HTTP/1.1 404 Not Found\r\n"));
        client.print(F("Connection:close\r\n\r\n"));
                
        
        delay(10);
        client.flush();
        delay(10);
        client.stop();

        Serial.println();
        Serial.println(F("---------------------Favicon_Client Stop"));
        LoopTime = millis();
      }else if(req.indexOf("apple-touch-icon") != -1){

        Serial.println();
        Serial.println(F("---------------------GET apple-touch-icon Request"));
        Serial.print(req);
        while(client.available()){
          Serial.write(client.read());
        }
        delay(10);
        client.stop();
        delay(10);
        client.flush();
        Serial.println();
        Serial.println(F("---------------------apple-touch-icon_Client Stop"));
        LoopTime = millis();
      }
    }
  }
}

//************Hash sha1 base64 encord**************************
void EasyWebSocket::Hash_Key(String h_req_key, char* h_resp_key)
{
  char Base64[65] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
                      'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
                      'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
                      'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/',
                      '='
                    };
  byte hash_six[27];
  byte dummy_h1, dummy_h2;
  byte bb;
  byte i, j;
  i=0;
  j=0;
  
  String merge_str;

  merge_str = h_req_key + String(GUID_str);
  Serial.println(F("--------------------Hash key Generation"));
  Serial.print(F("merge_str ="));
  Serial.println(merge_str);
  Serial.print(F("SHA1:"));
  Serial.println(sha1(merge_str));

  byte hash[20];
  sha1(merge_str, &hash[0]);

  for( i = 0; i < 20; i++) {
    hash_six[j] = hash[i]>>2;
    
    hash_six[j+1] = hash[i+1] >> 4;
    bitWrite(hash_six[j+1], 4, bitRead(hash[i],0));
    bitWrite(hash_six[j+1], 5, bitRead(hash[i],1));
    
    if(j+2 < 26){
      hash_six[j+2] = hash[i+2] >> 6;
      bitWrite(hash_six[j+2], 2, bitRead(hash[i+1],0));
      bitWrite(hash_six[j+2], 3, bitRead(hash[i+1],1));
      bitWrite(hash_six[j+2], 4, bitRead(hash[i+1],2));
      bitWrite(hash_six[j+2], 5, bitRead(hash[i+1],3));
    }else if(j+2 == 26){
      dummy_h1 = 0;
      dummy_h2 = 0;
      dummy_h2 = hash[i+1] << 4;
      dummy_h2 = dummy_h2 >>2;
      hash_six[j+2] = dummy_h1 | dummy_h2;
    }
    
    if( j+3 < 27 ){
      hash_six[j+3] = hash[i+2];
      bitWrite(hash_six[j+3], 6, 0);
      bitWrite(hash_six[j+3], 7, 0);
    }else if(j+3 == 27){
      hash_six[j+3] = '=';
    }
    
    h_resp_key[j] = Base64[hash_six[j]];
    h_resp_key[j+1] = Base64[hash_six[j+1]];
    h_resp_key[j+2] = Base64[hash_six[j+2]];
    
    if(j+3==27){
      h_resp_key[j+3] = Base64[64];
      break;
    }else{
      h_resp_key[j+3] = Base64[hash_six[j+3]];
    }
    
    i = i + 2;
    j = j + 4;
  }
  h_resp_key[28] = '\0';

  Serial.print(F("hash_six = "));
  for(i=0; i<28; i++){
    Serial.print(hash_six[i],BIN);
    Serial.print('_');
  }
  Serial.println();
}
  
void EasyWebSocket::EWS_ESP8266_Str_SEND(String str, String id)
{
  str += '|' + id + '|';
  client.write(B10000001);
  client.write(str.length());
  client.print(str); 
}

String EasyWebSocket::EWS_ESP8266CharReceive(int pTime)
{
  byte b=0;
  byte data_len;
  byte mask[4];
  byte i;
  String str_close = "_close";

  if(_WS_on){
    if(pTime > 0){
      if(millis()-_PingLastTime > pTime){
        EasyWebSocket::EWS_PING_SEND();
        _PingLastTime = millis();
      }

      if((millis() - _PongLastTime) > (pTime + 500)){
        delay(10);
        client.stop();
        delay(10);
        client.flush();
        Serial.println();
        Serial.println(F("-----------------Ping Non-Response Client.STOP"));
        _WS_on = false;
        _Ini_html_on = false;
        _Upgrade_first_on = false;
        return str_close;
      }
    }
  }
  
  if(client.available()){
    b = client.read();
    if(b == B10000001 || b == B10001010){
      switch (b){
        case B10000001:
          _PingLastTime = millis();
          _PongLastTime = millis();
          break;
        case B10001010:
          _PongLastTime = millis();
          Serial.println(F("Pong Receive**********"));
          break;
      }

      b = client.read();
      data_len = b - B10000000;

      for(i=0; i<4; i++){
        mask[i] = client.read();
      }
      
      byte m_data[data_len];
      char data_c[data_len + 1];
          
      for(i = 0; i<data_len; i++){
        m_data[i] = client.read();
        data_c[i] = mask[i%4]^m_data[i];
      }
      data_c[data_len] = '\0';

      return String(data_c);

    }else if(b == B10001000){
      Serial.println(F("------------------Close Command Received"));
      b = client.read();
      Serial.println(b,BIN);
      data_len = b - B10000000;
      if(data_len == 0){
        while(client.available()){
          b = client.read();
        }
        Serial.println("Closing HandShake OK!");
      }else{
        for(i=0; i<4; i++){
          mask[i] = client.read();
        }

        byte m_data2[data_len];
        char data_c2[data_len + 1];

        for(i = 0; i<data_len; i++){
          m_data2[i] = client.read();
          data_c2[i] = mask[i%4]^m_data2[i];
//          Serial.println(data_c2[i],BIN);
        }
        data_c2[data_len] = '\0';
//        Serial.println(data_c2);
        Serial.println("Closing Message ??");
      }    
      
      delay(1);
      client.write(B10001000);
      delay(1);
      Serial.println(F("------------------Close Command Send"));
      
      delay(10);
      client.stop();
      delay(10);
      client.flush();
      Serial.println();
      Serial.println(F("------------------Client.STOP"));
      _WS_on = false;
      _Ini_html_on = false;
      _Upgrade_first_on = false;
      
      while(client){
        if(client.available()){
          String req = client.readStringUntil('\n');
          Serial.println(req);
          if(req.indexOf("GET /favicon") != -1){
            Serial.println(F("------------------------Favicon GET Request Received 3"));

            while(client.available()){
              Serial.write(client.read());
            }
            
            client.print(F("HTTP/1.1 404 Not Found\r\n"));
            client.print(F("Connection:close\r\n\r\n"));
            
            delay(10);
            client.stop();
            delay(10);
            client.flush();
            Serial.println(F("-----------------------Favicon client.stop"));
            Serial.println(client);
            break;
          }
        }
      }
      
      return str_close;
    }
  }else{
    return String('\0');
  }
}

void EasyWebSocket::EWS_PING_SEND()
{
  client.write(B10001001);
  client.write(4);
  client.print(F("Ping"));
  Serial.println();
  Serial.println(F("Ping Send-----------"));
}

String EasyWebSocket::EWS_Body_style(String text_color, String bg_color)
{
  String str;
  str = "<body style='color:";
  str += text_color;
  str += "; background:";
  str += bg_color;
  str += ";'>\r\n";
  return str;
}

String EasyWebSocket::EWS_OnOff_Button(String button_id, int width, int height, byte font_size, String f_color, String b_color)
{
  String str;
  str = "<input type='button' value='OFF' onClick=\"OnOffBt(this,'";
  str += button_id;
  str += "');\"";
  str += " style='width:";
  str += String(width);
  str += "px; ";
  str += "height:";
  str += String(height);
  str += "px; font-size:";
  str += String(font_size);
  str += "px; color:";
  str += f_color;
  str += "; background-color:";
  str += b_color;
  str += ";' >\r\n";
  
  return str;
}

String EasyWebSocket::EWS_On_Momentary_Button(String button_id, String text, int width, int height, byte font_size, String f_color, String b_color)
{
  String str;
  str = "<input type='button' value='";
  str += text;
  str += "' onClick=\"doSend(100,'";
  str += button_id;
  str += "'); data_tmp = 0;\"";
  str += " style='width:";
  str += String(width);
  str += "px; ";
  str += "height:";
  str += String(height);
  str += "px; font-size:";
  str += String(font_size);
  str += "px; color:";
  str += f_color;
  str += "; background-color:";
  str += b_color;
  str += ";' >\r\n";
  
  return str;
}


String EasyWebSocket::EWS_Touch_Slider_BT(String slider_id, String vbox_id)
{
  String str;
  str += "<input type='range' ontouchmove=\"doSend(this.value,'";
  str += slider_id;
  str += "'); document.getElementById('";
  str += vbox_id;
  str += "').value=this.value;\">\r\n";
  return str;
}

String EasyWebSocket::EWS_Touch_Slider_T(String slider_id, String txt_id)
{
  String str;
  str += "<input type='range' ontouchmove=\"doSend(this.value,'";
  str += slider_id;
  str += "'); document.getElementById('";
  str += txt_id;
  str += "').innerHTML=this.value;\">\r\n";
  return str;
}


String EasyWebSocket::EWS_Mouse_Slider_BT(String slider_id, String vbox_id)
{
  String str;
  str += "<input type='range' onMousemove=\"doSend(this.value,'";
  str += slider_id;
  str += "'); document.getElementById('";
  str += vbox_id;
  str += "').value=this.value;\">\r\n";
  return str;
}

String EasyWebSocket::EWS_Mouse_Slider_T(String slider_id, String txt_id)
{
  String str;
  str += "<input type='range' onMousemove=\"doSend(this.value,'";
  str += slider_id;
  str += "'); document.getElementById('";
  str += txt_id;
  str += "').innerHTML=this.value;\">\r\n";
  return str;
}

String EasyWebSocket::EWS_Sl_BoxText(String vbox_id, int width, int height, byte font_size, String color)
{
  String str;
  str = "<input type='number' id='";
  str += vbox_id;
  str += "' style='width:";
  str += String(width);
  str += "px; ";
  str += "height:";
  str += String(height);
  str += "px; font-size:";
  str += String(font_size);
  str += "px; color:";
  str += String(color);
  str += ";' >\r\n";
  return str;
}

String EasyWebSocket::EWS_Sl_Text(String text_id, byte font_size, String color)
{
  String str;
  str = "<span id='";
  str += text_id;
  str += "' style='font-size:";
  str += String(font_size);
  str += "px; color:";
  str += String(color);
  str += ";' ></span>\r\n";
  return str;
}

String EasyWebSocket::EWS_BrowserReceiveTextTag(String id, byte font_size, String fnt_col)
{
  String str;
  str = "<span id='" + id;
  str += "' style='font-size:";
  str += String(font_size);
  str += "px; color:" + fnt_col + ";'></span>\r\n";
  return str;
}

String EasyWebSocket::EWS_Close_Button(String name, int width, int height, byte font_size)
{
  String str;
  str = "<input type='button' value='";
  str += name;
  str += "' style='width:";
  str += String(width);
  str += "px; ";
  str += "height:";
  str += String(height);
  str += "px; font-size:";
  str += String(font_size);
  str += "px;' onclick='WS_close()'>\r\n";
  return str;
}

String EasyWebSocket::EWS_Window_ReLoad_Button(String name, int width, int height, byte font_size)
{
  String str;
  str = "<input type='button' value='";
  str += name;
  str += "' style='width:";
  str += String(width);
  str += "px; ";
  str += "height:";
  str += String(height);
  str += "px; font-size:";
  str += String(font_size);
  str += "px;' onclick='window.location.reload()'>\r\n";
  return str;
}

String EasyWebSocket::EWS_WebSocket_Reconnection_Button(String name, int width, int height, byte font_size)
{
  String str;
  str = "<input type='button' value='";
  str += name;
  str += "' style='width:";
  str += String(width);
  str += "px; ";
  str += "height:";
  str += String(height);
  str += "px; font-size:";
  str += String(font_size);
  str += "px;' onclick='init();'>\r\n";
  return str;
}

String EasyWebSocket::EWS_BrowserSendRate()
{
  String str;
  str += "<form name='fRate'>\r\n";
  str += "  <select id='selRate'>\r\n";
  str += "    <option value=0>0ms</option>\r\n";
  str += "    <option value=5>5ms</option>\r\n";
  str += "    <option value=10>10ms</option>\r\n";
  str += "    <option value=15>15ms</option>\r\n";
  str += "    <option value=20>20ms</option>\r\n";
  str += "    <option value=25>25ms</option>\r\n";
  str += "    <option value=30>30ms</option>\r\n";
  str += "    <option value=35>35ms</option>\r\n";
  str += "    <option value=40>40ms</option>\r\n";
  str += "    <option value=45>45ms</option>\r\n";
  str += "    <option value=50>50ms</option>\r\n";
  str += "  </select>\r\n";
  str += "  <input type='button' value='Rate Exec' onclick='onButtonRate();' />\r\n";
  str += "  Transfer Rate= <span id='RateTxt'>0</span>ms\r\n";
  str += "</form>\r\n";
  return str;
}

String EasyWebSocket::EWS_Status_Text(byte font_size, String color)
{
  String str;
  str = "<span id='__wsStatus__' style='font-size:";
  str += String(font_size);
  str += "px; color:";
  str += String(color);
  str += ";' ></span>\r\n";
  return str;
}

String EasyWebSocket::EWS_Canvas_Slider_T(String slider_id, int width, int height, String frame_col, String fill_col)
{
  String str;
  str = "<canvas id='" + slider_id + "' width='" + String(width) + "' height='" + String(height) + "'></canvas>\r\n";
  
  str += "<script type='text/javascript'>\r\n";
  str += "  fnc_" + slider_id + "();\r\n";
  str += "  function fnc_" + slider_id + "(){\r\n";
  str += "    var c_w = " + String(width) + ";\r\n";
  str += "    var c_h = " + String(height) + ";\r\n";
  str += "    var line_width = 5;\r\n";
  str += "    var canvas2 = document.getElementById('" + slider_id + "');\r\n";
  str += "    var ctx2 = canvas2.getContext('2d');\r\n";
  str += "    ctx2.clearRect(0, 0, c_w, c_h);\r\n";
  str += "    ctx2.beginPath();\r\n";
  str += "    ctx2.lineWidth = line_width;\r\n";
  str += "    ctx2.strokeStyle = '" + frame_col + "';\r\n";
  str += "    ctx2.strokeRect(0,0,c_w,c_h);\r\n";

  str += "    canvas2.addEventListener('touchmove', slider_" + slider_id + ", false);\r\n";
  str += "    canvas2.addEventListener('touchstart', slider_" + slider_id + ", false);\r\n";

  str += "    function slider_" + slider_id + "(event3) {\r\n";
  str += "      event3.preventDefault();\r\n";
  str += "      event3.stopPropagation();\r\n";
  str += "      var evt555=event3.touches[0];\r\n";
  str += "      var OffSet2 = evt555.target.getBoundingClientRect();\r\n";
  str += "      var ex = evt555.clientX - OffSet2.left;\r\n";
  str += "      if( ex < 0 ){ex = 0;}\r\n";
  str += "      else if(ex>c_w){ex = c_w;}\r\n";
  str += "      var e_cl_X = Math.floor(ex);\r\n";
  str += "      ctx2.clearRect(0, 0, c_w, c_h);\r\n";
  str += "      ctx2.beginPath();\r\n";
  str += "      ctx2.fillStyle = '" + fill_col + "';\r\n";
  str += "      ctx2.rect(0,0, e_cl_X, c_h);\r\n";
  str += "      ctx2.fill();\r\n";
  str += "      ctx2.beginPath();\r\n";
  str += "      ctx2.lineWidth = line_width;\r\n";
  str += "      ctx2.strokeStyle = '" + frame_col + "';\r\n";
  str += "      ctx2.strokeRect(0,0,c_w,c_h);\r\n";
  str += "      doSend(String(e_cl_X), '" + slider_id + "');\r\n";
  str += "    };\r\n";
  str += "  };\r\n";
  str += "</script>\r\n";
  return str;
}

String EasyWebSocket::EWS_TextBox_Send(String id, String txt, String BT_txt)
{
  String str;
  str = "<form>\r\n";
  str += "<input type='text' id='" + id + "' value='" + txt + "'>\r\n";
  str += "<input type='button' value='" + BT_txt + "' onclick='doSend_TextBox(\"" + id + "\");'>\r\n";
  str += "</form>\r\n";
  return str;
}

String EasyWebSocket::EWS_Web_Get(char* host, String target_ip, uint8_t char_tag, String Final_tag, String Begin_tag, String End_tag, String Paragraph)
{
  String str1;
  String str2;
  String ret_str = "";

  delay(10);
  client.stop();
  delay(10);
  client.flush();
  Serial.println(F("--------------------WebSocket Client Stop"));
 
  if (client.connect(host, 80)) {
    Serial.print(host); Serial.print(F("-------------"));
    Serial.println(F("connected"));
    Serial.println(F("--------------------WEB HTTP GET Request"));
    str1 = "GET " + target_ip + " HTTP/1.1\r\n";
    str2 = "Host: " + String(host)+"\r\n";
    client.print(str1);
    client.print(str2);
    client.print(F("Content-Type: text/html; charset=UTF-8\r\n"));
    client.print(F("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"));
    client.print(F("Content-Language: ja\r\n"));
    client.print(F("Accept-Language: ja\r\n"));
    client.print(F("Accept-Charset: UTF-8\r\n"));
    client.print(F("Connection: close\r\n\r\n"));
    Serial.print(str1);
    Serial.println(str2);
  }else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  String dummy_str;
  uint16_t from, to;
  if(client){
    Serial.println(F("--------------------WEB HTTP Response"));
    while(client.connected()){
      while (client.available()) {
        if(dummy_str.indexOf(Final_tag) < 0){
          dummy_str = client.readStringUntil(char_tag);
//Serial.println(dummy_str);
          if(dummy_str.indexOf(Begin_tag) != -1){
            from = dummy_str.indexOf(Begin_tag) + Begin_tag.length();
            to = dummy_str.indexOf(End_tag);
            ret_str += Paragraph;
            ret_str += dummy_str.substring(from,to);
            ret_str += "  ";
          }
          dummy_str = "";
        }else{
          break;
        }
      }
    }
  }
  ret_str += "\0";
 
  delay(10);
  client.stop();
  delay(10);
  client.flush();
  Serial.println(F("--------------------Client Stop"));
 
  _WS_on = false;
  _Ini_html_on = false;
  _Upgrade_first_on = false;
  
  return ret_str;
}