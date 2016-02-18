/*
  EasyWebsocket.h - WebSocket for ESP-WROOM-02 ( esp8266 )
  Beta version 1.3

The MIT License (MIT)

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
*/

#ifndef EasyWebSocket_h_
#define EasyWebSocket_h_
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "Hash.h"
#include "FS.h"

class EasyWebSocket
{
public:
  EasyWebSocket();

  void AP_Connect(const char* ssid, const char* password);
  void EWS_HandShake(String _res_html1, String _res_html2, String _res_html3, String _res_html4, String _res_html5, String _res_html6, String _res_html7);
  void EWS_HTTP_Responce();
  void Hash_Key(String h_req_key, char* h_resp_key);
  void EWS_ESP8266_Str_SEND(String str, String id);
  void EWS_PING_SEND();
  String EWS_ESP8266CharReceive(int pTime);
  String EWS_OnOff_Button(String button_id, int width, int height, byte font_size, String f_color, String b_color);
  String EWS_On_Momentary_Button(String button_id, String text, int width, int height, byte font_size, String f_color, String b_color);
  String EWS_Touch_Slider_BT(String slider_id, String vbox_id);
  String EWS_Touch_Slider_T(String slider_id, String txt_id);
  String EWS_Mouse_Slider_BT(String slider_id, String vbox_id);
  String EWS_Mouse_Slider_T(String slider_id, String txt_id);
  String EWS_Sl_BoxText(String vbox_id, int width, int height, byte font_size, String color);
  String EWS_Sl_Text(String text_id, byte font_size, String color);
  String EWS_Body_style(String text_color, String bg_color);
  String EWS_BrowserReceiveTextTag(String id, byte font_size, String fnt_col);
  String EWS_Close_Button(String name, int width, int height, byte font_size);
  String EWS_Window_ReLoad_Button(String name, int width, int height, byte font_size);
  String EWS_BrowserSendRate();
  String EWS_Status_Text(byte font_size, String color);
  String EWS_Canvas_Slider_T(String slider_id, int width, int height, String frame_col, String fill_col);
  String EWS_TextBox_Send(String id, String txt, String BT_txt);
  
private:
  boolean _Ini_html_on = false;
  boolean _WS_on = false;
  boolean _Upgrade_first_on = false;
  char _Android_or_iPad;
  long _PingLastTime;
  long _PongLastTime;
};

#endif