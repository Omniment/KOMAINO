//
//  komaino.cpp
//
//
//  Created by 田村 塁 on 2017/01/25.
//
//

//#include "komaino.h"
#include "ESP8266WebServer/src/ESP8266WebServer.h"
#include "komaino_font.h"
#include "Wire/Wire.h"
#include "EEPROM/EEPROM.h"
#include "FS.h"

byte addr_ioex = 0x20;//IOEXアドレス

unsigned int dsp_temp[5]; //ディスプレイ描画データ
int dsp_dt = 3; //ディスプレイの走査間隔


//unsigned int dsp_temp[5]; //ディスプレイ描画データ
//int dsp_dt = 3; //ディスプレイの走査間隔

//void lightProc(byte x, byte y); //ディスプレイ描画関数
//void dspWrite();
//void slide(String slide_string);

ESP8266WebServer server(80);



void handleRoot() {
    //【サーバー】ルートアクセス時の動作
    server.send(200, "text/html", "hello KOMAINO!!");
}

void severHandle(){
    server.handleClient();
}

void komaino_init(){
    SPIFFS.begin(); //SPIFFSスタート
    Wire.begin();   //i2cスタート
    Serial.begin(115200);   //serialスタート
    
    ioex_init();
    
    //APモード
    //char* ssid_ap;
    char ssid_ap[15];
    char* password_ap;
    
    //esp chip idからWiFi SSID作成
    password_ap = "12345678";   //apモードパスワード設定
    //ssid_ap = ESP.getChipId();
    //ssid_ap = "KOMAINO-" + ssid_ap;
    //Serial.println(ssid_ap);    //デバッグ用SSID出力
    
    sprintf(ssid_ap, "KOMAINO-%06x", ESP.getChipId());
    
    //slide("APMODE", 80);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid_ap, password_ap);
    IPAddress myIP = WiFi.softAPIP();
    
    Serial.print("AP IP address : ");   //デバッグ用出力
    Serial.println(myIP);
    
    server.on("/", handleRoot);
    server.begin();
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

void KomainoControl::print(String slide_string , unsigned int slide_speed){
    int millis_old = millis();//文字送りディレイ用
    byte slide_add_cnt = 6;//1画面カウント
    unsigned int dsp_chash;//ディスプレイキャッシュ
    unsigned int slide_cnt = 0;//行送りカウント
    unsigned int char_cnt = 0;//文字カウント
    int slide_chach_char = int(slide_string.charAt(char_cnt) - 32);
    
    for (slide_cnt = 0; slide_cnt <= ((slide_string.length() + 1) * 6);) {
        if (millis() > millis_old + slide_speed) {
            
            if (slide_add_cnt > 5 & ((slide_string.length()) > char_cnt)) {
                slide_chach_char = int(slide_string.charAt(char_cnt) - 32);
                for (byte slide_add_cnt = 0; slide_add_cnt <= 4; slide_add_cnt++) {
                    dsp_chash = komaino_font[slide_chach_char][slide_add_cnt];
                    dsp_chash = dsp_chash << 5;
                    dsp_temp[slide_add_cnt] += dsp_chash;
                }
                slide_add_cnt = 0;
                char_cnt++;
                //Serial.println(char_cnt);
            }
            
            dsp_temp[0] = dsp_temp[0] >> 1;
            dsp_temp[1] = dsp_temp[1] >> 1;
            dsp_temp[2] = dsp_temp[2] >> 1;
            dsp_temp[3] = dsp_temp[3] >> 1;
            dsp_temp[4] = dsp_temp[4] >> 1;
            millis_old = millis();
            slide_cnt++;
            slide_add_cnt++;
            
        }
        dspWrite();
    }
    
    dsp_temp[0] = 0x0;
    dsp_temp[1] = 0x0;
    dsp_temp[2] = 0x0;
    dsp_temp[3] = 0x0;
    dsp_temp[4] = 0x0;
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
    delay(dsp_dt);
    lightProc(dsp_temp[1], 0x1D);
    delay(dsp_dt);
    lightProc(dsp_temp[2], 0x1B);
    delay(dsp_dt);
    lightProc(dsp_temp[3], 0x17);
    delay(dsp_dt);
    lightProc(dsp_temp[4], 0x0F);
    delay(dsp_dt);
    lightProc(0x00, 0x1F);
}

KomainoControl komaino;
