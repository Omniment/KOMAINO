//
//  komaino.cpp
//
//
//  Created by 田村 塁 on 2017/01/25.
//
//

#include "komaino.h"
#include "ESP8266WiFi/src/ESP8266WiFi.h"

byte addr_ioex = 0x20;//IOEXアドレス

//unsigned int dsp_temp[5]; //ディスプレイ描画データ
//int dsp_dt = 3; //ディスプレイの走査間隔

//void lightProc(byte x, byte y); //ディスプレイ描画関数
//void dspWrite();
//void slide(String slide_string);

void komaino_init(){
    SPIFFS.begin(); //SPIFFSスタート
    Wire.begin();   //i2cスタート
    Serial.begin(115200);   //serialスタート
    
    //ESP8266WebServer server(80);
    
    //APモード
    char* ssid_ap;
    char* password_ap;
    
    //esp chip idからWiFi SSID作成
    password_ap = "12345678";   //apモードパスワード設定
    sprintf(ssid_ap, "KOMAINO-%06x", ESP.getChipId());  //SSID作成
    Serial.println(ssid_ap);    //デバッグ用SSID出力
    
    //slide("APMODE", 80);
    
    //ssid = char(mac_addr_u3);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid_ap, password_ap);
    IPAddress myIP = WiFi.softAPIP();
    
    Serial.print("AP IP address : ");   //デバッグ用出力
    Serial.println(myIP);
    
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

/*
 byte komaino_font[95][5] {
 {0B0, 0B0, 0B0, 0B0, 0B0},//スペース
 {0B00100, 0B00100, 0B00100, 0B0, 0B00100},//!
 {0B01010, 0B01010, 0B0, 0B0, 0B0},//"
 {0B01010, 0B11111, 0B01010, 0B11111, 0B01010},//#
 {0B11110, 0B01011, 0B01110, 0B11010, 0B01111},//$
 {0B10011, 0B01011, 0B00100, 0B11010, 0B11001},//%
 {0B00100, 0B01010, 0B10110, 0B01001, 0B11110},//&
 {0B00100, 0B00100, 0B0, 0B0, 0B0},//'
 {0B10000, 0B01000, 0B01000, 0B01000, 0B10000},//(
 {0B00001, 0B00010, 0B00010, 0B00010, 0B00001},//)
 {0B10100, 0B01110, 0B00100, 0B01110, 0B10101},//*
 {0B00100, 0B00100, 0B11111, 0B00100, 0B00100},//+
 {0B0, 0B0, 0B00100, 0B00100, 0B00010},//,
 {0B0, 0B0, 0B11111, 0B0, 0B0},//-
 {0B0, 0B0, 0B0, 0B0, 0B00010},//.
 {0B00001, 0B00010, 0B00100, 0B01000, 0B10000},///
 {0B00100, 0B01010, 0B01010, 0B01010, 0B00100},//0
 {0B00100, 0B00110, 0B00100, 0B00100, 0B01110},//1
 {0B01110, 0B10001, 0B01000, 0B00100, 0B11111},//2
 {0B01110, 0B10000, 0B01110, 0B10000, 0B01110},//3
 {0B01000, 0B01100, 0B01010, 0B11111, 0B01000},//4
 {0B11111, 0B00001, 0B01111, 0B10000, 0B01111},//5
 {0B01110, 0B00001, 0B01111, 0B10001, 0B01110},//6
 {0B11111, 0B10000, 0B01000, 0B00100, 0B00100},//7
 {0B01110, 0B10001, 0B01110, 0B10001, 0B01110},//8
 {0B01110, 0B10001, 0B11110, 0B01000, 0B00100},//9
 {0B0, 0B00100, 0B0, 0B00100, 0B0},//:
 {0B0, 0B00100, 0B0, 0B00100, 0B00100},//;
 {0B11000, 0B00110, 0B00001, 0B00110, 0B11000},//<
 {0B0, 0B11111, 0B0, 0B11111, 0B0},//=
 {0B00011, 0B01100, 0B10000, 0B01100, 0B00011},//>
 {0B01110, 0B10001, 0B01000, 0B00100, 0B00100},//?
 {0B01110, 0B10001, 0B11101, 0B11101, 0B01110},//@
 {0B00100, 0B01010, 0B10001, 0B11111, 0B10001},//A
 {0B01111, 0B10001, 0B01111, 0B10001, 0B01111},//B
 {0B01110, 0B10001, 0B00001, 0B10001, 0B01110},//C
 {0B01111, 0B10001, 0B10001, 0B10001, 0B01111},//D
 {0B11111, 0B00001, 0B11111, 0B00001, 0B11111},//E
 {0B11111, 0B00001, 0B01111, 0B00001, 0B00001},//F
 {0B01110, 0B00001, 0B11101, 0B10001, 0B01110},//G
 {0B10001, 0B10001, 0B11111, 0B10001, 0B10001},//H
 {0B01110, 0B00100, 0B00100, 0B00100, 0B01110},//I
 {0B01110, 0B00100, 0B00100, 0B00101, 0B00010},//J
 {0B10001, 0B01001, 0B00111, 0B01001, 0B10001},//K
 {0B00001, 0B00001, 0B00001, 0B00001, 0B11111},//L
 {0B10001, 0B11011, 0B10101, 0B10001, 0B10001},//M
 {0B10001, 0B10011, 0B10101, 0B11001, 0B10001},//N
 {0B01110, 0B10001, 0B10001, 0B10001, 0B01110},//O
 {0B01111, 0B10001, 0B01111, 0B00001, 0B00001},//P
 {0B01110, 0B10001, 0B10001, 0B11000, 0B11110},//Q
 {0B01111, 0B10001, 0B01111, 0B01001, 0B10001},//R
 {0B01110, 0B00001, 0B01110, 0B10000, 0B01110},//S
 {0B11111, 0B00100, 0B00100, 0B00100, 0B00100},//T
 {0B10001, 0B10001, 0B10001, 0B10001, 0B01110},//U
 {0B10001, 0B10001, 0B01010, 0B01010, 0B00100},//V
 {0B10001, 0B10001, 0B10101, 0B10101, 0B01010},//W
 {0B10001, 0B01010, 0B00100, 0B01010, 0B10001},//X
 {0B10001, 0B10001, 0B01010, 0B00100, 0B00100},//Y
 {0B11111, 0B01000, 0B00100, 0B00010, 0B11111},//Z
 {0B11000, 0B01000, 0B01000, 0B01000, 0B11000},//[
 {0B00001, 0B00100, 0B00100, 0B01000, 0B10000},//¥
 {0B00011, 0B00010, 0B00010, 0B00010, 0B00011},//]
 {0B00100, 0B01010, 0B0, 0B0, 0B0},//^
 {0B0, 0B0, 0B0, 0B0, 0B11111},//_
 {0B00010, 0B00100, 0B0, 0B0, 0B0},//`
 {0B0, 0B00110, 0B01000, 0B01010, 0B01110},//a
 {0B0, 0B00010, 0B00110, 0B01010, 0B00110},//b
 {0B0, 0B0, 0B01110, 0B00010, 0B01110},//c
 {0B0, 0B01000, 0B01100, 0B01010, 0B01100},//d
 {0B0, 0B01110, 0B01110, 0B00010, 0B01110},//e
 {0B0, 0B01100, 0B00100, 0B01110, 0B00100},//f
 {0B01000, 0B00110, 0B00110, 0B01010, 0B00110},//g
 {0B0, 0B00010, 0B00110, 0B01010, 0B01010},//h
 {0B0, 0B00100, 0B0, 0B00100, 0B00100},//i
 {0B0, 0B00100, 0B0, 0B00100, 0B00110},//j
 {0B0, 0B00010, 0B01010, 0B00110, 0B01010},//k
 {0B0, 0B00010, 0B00010, 0B00010, 0B00010},//l
 {0B0, 0B01010, 0B10101, 0B10101, 0B10101},//m
 {0B0, 0B0, 0B01110, 0B01010, 0B01010},//n
 {0B0, 0B0, 0B01110, 0B01010, 0B01110},//o
 {0B0, 0B00110, 0B01010, 0B00110, 0B00010},//p
 {0B0, 0B01100, 0B01010, 0B01100, 0B01000},//q
 {0B0, 0B0, 0B01110, 0B00010, 0B00010},//r
 {0B01100, 0B00010, 0B00100, 0B01000, 0B00110},//s
 {0B0, 0B00100, 0B01110, 0B00100, 0B01100},//t
 {0B0, 0B0, 0B01010, 0B01010, 0B01100},//u
 {0B0, 0B0, 0B01010, 0B01010, 0B00100},//v
 {0B0, 0B0, 0B10101, 0B10101, 0B01010},//w
 {0B0, 0B0, 0B01010, 0B00100, 0B01010},//x
 {0B0, 0B01010, 0B01100, 0B01000, 0B00100},//y
 {0B0, 0B01110, 0B01000, 0B00100, 0B01110},//z
 {0B11000, 0B01000, 0B00100, 0B01000, 0B11000},//{
 {0B00100, 0B00100, 0B00100, 0B00100, 0B00100},//|
 {0B00011, 0B00010, 0B00100, 0B00010, 0B00011},//}
 {0B0, 0B00010, 0B10101, 0B01000, 0B0}//~
 };
 
 void ioex_init() {
 //アウトプット設定
 Wire.beginTransmission(addr);
 Wire.write(0x06);
 Wire.write(0xE0);
 Wire.write(0xE0);
 Wire.endTransmission();
 delay(1);
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
 
 //ディスプレイ1列を描画する関数
 void lightProc(byte x, byte y) {
 //一度画面を消去
 Wire.beginTransmission(addr);
 Wire.write(0x03);
 Wire.write(0x1F);
 Wire.endTransmission();
 delayMicroseconds(10);
 
 //画面描画
 //0x02から0x03まで2バイト連続で送信
 Wire.beginTransmission(addr);
 Wire.write(0x02);
 Wire.write(x);
 Wire.write(y);
 Wire.endTransmission();
 delayMicroseconds(10);
 }
 
 void slide(String slide_string , unsigned int slide_speed) {
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
 
 */
