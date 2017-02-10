//
//  komaino.cpp
//
//  Copyright (c) 2017 Omniment INC. All rights reserved.
//  This library is free software
//  License as published by the Free Software Foundation
//  either
//  version 2.1 of the License, or (at your option) any later version.
//
//  Created by R.tamura on 2017/01/25.
//
//

//#include "komaino.h"
#include "ESP8266WebServer/src/ESP8266WebServer.h"
#include "komaino_font.h"
#include "Wire/Wire.h"
#include "EEPROM/EEPROM.h"
#include "FS.h"
#include "ArduinoOTA/ArduinoOTA.h"

byte addr_ioex = 0x20;//IOEXアドレス

unsigned int dsp_temp[5]; //ディスプレイ描画データ
int dsp_dt = 300; //ディスプレイの走査間隔

boolean slide_handle = 0;   //slide実行フラグ
int slide_millis_old;//文字送りディレイ用
byte slide_add_cnt = 6;//1画面カウント
unsigned int slide_dsp_chash;//ディスプレイキャッシュ
unsigned int slide_cnt;//行送りカウント
unsigned int slide_char_cnt;//文字カウント

int slide_char;
String slide_string;
unsigned int slide_speed;

String debug;

ESP8266WebServer komainoServer(80);

void handleRoot() {
    //【サーバー】ルートアクセス時の動作
    komainoServer.send(200, "text/html", "HELLO KOMAINO!!");
    //komainoServer.send(200, "text/html", debug);
}

void severHandle(){
    komainoServer.handleClient();
}

void komaino_init(){
    SPIFFS.begin(); //SPIFFSスタート
    Wire.begin();   //i2cスタート
    Serial.begin(115200);   //serialスタート
    
    delay(100);
    
    ioex_init();
    
    //APモード
    char ssid_ap[15];
    char* password_ap;
    
    //esp chip idからWiFi SSID作成
    password_ap = "12345678";   //apモードパスワード設定
    
    sprintf(ssid_ap, "KOMAINO-%06x", ESP.getChipId());
    
    //WiFi AP 立ち上げ
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid_ap, password_ap);
    IPAddress myIP = WiFi.softAPIP();
    
    Serial.println(ssid_ap);
    Serial.print("AP IP address : ");   //デバッグ用出力
    Serial.println(myIP);
    
    komainoServer.on("/", handleRoot);
    komainoServer.begin();
    Serial.println("HTTP server started");
    
    pinMode(IO1, OUTPUT);
    pinMode(IO2, OUTPUT);
    pinMode(butA, INPUT_PULLUP);
    pinMode(butB, INPUT_PULLUP);
}

void ioex_init() {
    //アウトプット設定
    Wire.beginTransmission(addr_ioex);
    Wire.write(0x06);
    Wire.write(0xE0);
    Wire.write(0xE0);
    Wire.endTransmission();
    delay(1);
}

boolean KomainoControl::wifiSta(char* ssid_sta,char* password_sta){
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_sta, password_sta);
    
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        //APモードに変更
        char ssid_ap[15];
        char* password_ap;
        
        //esp chip idからWiFi SSID作成
        password_ap = "12345678";   //apモードパスワード設定
        sprintf(ssid_ap, "KOMAINO-%06x", ESP.getChipId());
        
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ssid_ap, password_ap);
        return false;
    }else{
        return true;
    }
}

String KomainoControl::wifiIP(){
    String staIP = "IP:" + String(WiFi.localIP()[0]) + "." +
    String(WiFi.localIP()[1]) + "." +
    String(WiFi.localIP()[2]) + "." +
    String(WiFi.localIP()[3]);
    return staIP;
}

void KomainoControl::print(String slide_string_chach , unsigned int slide_speed_chach){
    dsp_temp[0] = 0x0;
    dsp_temp[1] = 0x0;
    dsp_temp[2] = 0x0;
    dsp_temp[3] = 0x0;
    dsp_temp[4] = 0x0;
    
    dspWrite();
    
    slide_millis_old = millis();//文字送りディレイ用
    slide_add_cnt = 6;//1画面カウント
    slide_cnt = 0;//行送りカウント
    slide_char_cnt = 0;//文字カウント
    
    slide_string = slide_string_chach;
    slide_speed = slide_speed_chach;
    
    slide_handle = 1;
    
    while (slide_handle == 1){
        slide();
        dspWrite();
        //delay(1);
        yield();
        ArduinoOTA.handle();
        severHandle();
    }
}

void KomainoControl::printOnLoop(String slide_string_chach , unsigned int slide_speed_chach){
    dsp_temp[0] = 0x0;
    dsp_temp[1] = 0x0;
    dsp_temp[2] = 0x0;
    dsp_temp[3] = 0x0;
    dsp_temp[4] = 0x0;
    
    dspWrite();
    
    slide_millis_old = millis();//文字送りディレイ用
    slide_add_cnt = 6;//1画面カウント
    slide_cnt = 0;//行送りカウント
    slide_char_cnt = 0;//文字カウント
    
    slide_string = slide_string_chach;
    slide_speed = slide_speed_chach;
    
    slide_handle = 1;
}

void KomainoControl::printStop(){
    slide_handle = 0;
}

void KomainoControl::drawDisplay(byte l1,byte l2,byte l3,byte l4,byte l5){
    byte cache_draw_dot;
    byte cnt = 0;
    
    //データ逆転
    while(cnt < 5){
        //cache_draw_dot;
        cache_draw_dot = cache_draw_dot << 1;
        cache_draw_dot += l1 & 1;
        cnt++;
        l1 = l1 >> 1;
    }
    dsp_temp[0] = cache_draw_dot;
    cache_draw_dot = 0;
    cnt = 0;
    
    while(cnt < 5){
        //cache_draw_dot;
        cache_draw_dot = cache_draw_dot << 1;
        cache_draw_dot += l2 & 1;
        cnt++;
        l2 = l2 >> 1;
    }
    dsp_temp[1] = cache_draw_dot;
    cache_draw_dot = 0;
    cnt = 0;
    
    while(cnt < 5){
        //cache_draw_dot;
        cache_draw_dot = cache_draw_dot << 1;
        cache_draw_dot += l3 & 1;
        cnt++;
        l3 = l3 >> 1;
    }
    dsp_temp[2] = cache_draw_dot;
    cache_draw_dot = 0;
    cnt = 0;
    
    while(cnt < 5){
        //cache_draw_dot;
        cache_draw_dot = cache_draw_dot << 1;
        cache_draw_dot += l4 & 1;
        cnt++;
        l4 = l4 >> 1;
    }
    dsp_temp[3] = cache_draw_dot;
    cache_draw_dot = 0;
    cnt = 0;
    
    while(cnt < 5){
        //cache_draw_dot;
        cache_draw_dot = cache_draw_dot << 1;
        cache_draw_dot += l5 & 1;
        cnt++;
        l5 = l5 >> 1;
    }
    dsp_temp[4] = cache_draw_dot;
    cache_draw_dot = 0;
    
    /*
    dsp_temp[0] = l1;
    dsp_temp[1] = l2;
    dsp_temp[2] = l3;
    dsp_temp[3] = l4;
    dsp_temp[4] = l5;
    */
}

void slide(){
    if(slide_handle == 1){
        
        slide_char = int(slide_string.charAt(slide_char_cnt) - 32);
        
        if(slide_cnt <= ((slide_string.length() + 1) * 6)){
            if (millis() > slide_millis_old + slide_speed) {
                
                if (slide_add_cnt > 5 & ((slide_string.length()) > slide_char_cnt)) {
                    slide_char = int(slide_string.charAt(slide_char_cnt) - 32);
                    for (byte slide_add_cnt = 0; slide_add_cnt <= 4; slide_add_cnt++) {
                        slide_dsp_chash = komaino_font[slide_char][slide_add_cnt];
                        slide_dsp_chash = slide_dsp_chash << 5;
                        dsp_temp[slide_add_cnt] += slide_dsp_chash;
                    }
                    slide_add_cnt = 0;
                    slide_char_cnt++;
                }
                dsp_temp[0] = dsp_temp[0] >> 1;
                dsp_temp[1] = dsp_temp[1] >> 1;
                dsp_temp[2] = dsp_temp[2] >> 1;
                dsp_temp[3] = dsp_temp[3] >> 1;
                dsp_temp[4] = dsp_temp[4] >> 1;
                slide_millis_old = millis();
                slide_cnt++;
                slide_add_cnt++;
            }
        }else{
            slide_handle = 0;
            dsp_temp[0] = 0x0;
            dsp_temp[1] = 0x0;
            dsp_temp[2] = 0x0;
            dsp_temp[3] = 0x0;
            dsp_temp[4] = 0x0;
        }
    }
}


//ディスプレイ1列を描画する関数
void lightProc(byte x, byte y) {
    //一度画面を消去
    Wire.beginTransmission(addr_ioex);
    Wire.write(0x03);
    Wire.write(0x1F);
    Wire.endTransmission();
    delayMicroseconds(10);
    
    //画面描画
    //0x02から0x03まで2バイト連続で送信
    Wire.beginTransmission(addr_ioex);
    Wire.write(0x02);
    Wire.write(x);
    Wire.write(y);
    Wire.endTransmission();
    delayMicroseconds(10);
}

void dspWrite() {
    lightProc(dsp_temp[0], 0x1E);
    delayMicroseconds(dsp_dt);
    lightProc(dsp_temp[1], 0x1D);
    delayMicroseconds(dsp_dt);
    lightProc(dsp_temp[2], 0x1B);
    delayMicroseconds(dsp_dt);
    lightProc(dsp_temp[3], 0x17);
    delayMicroseconds(dsp_dt);
    lightProc(dsp_temp[4], 0x0F);
    delayMicroseconds(dsp_dt);
    lightProc(0x00, 0x1F);
}

void loopManager(){
    slide();
    dspWrite();
    ArduinoOTA.handle();
}

void arduino_ota_init(){
    //OTA
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    
    ArduinoOTA.handle();
}

KomainoControl komaino;
