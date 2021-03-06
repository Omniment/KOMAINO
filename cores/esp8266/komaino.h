//
//  komaino.h
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

#ifndef komaino_h
#define komaino_h

#endif /* komaino_h */

#include "Arduino.h"

void komaino_init(); //KOMAINOイニシャライズ
void severHandle();
void ioex_init(); //IOEXイニシャライズ
void slide();           //スライド動作
void lightProc(byte x, byte y);  //画面描画
void dspWrite();        //ディスプレイ描画
void loopManager();     //ループで必ず行う処理

class KomainoControl {
public:
    void print(String slide_string , unsigned int slide_speed = 80);
    void printOnLoop(String slide_string , unsigned int slide_speed = 80);
    void printStop();
    void drawDisplay(byte l1,byte l2,byte l3,byte l4,byte l5);
    boolean wifiSta(char* ssid_sta,char* password_sta);
    String wifiIP();
    
    void webConnect(char connect_server[256], unsigned int port = 80);
    void webPrint(char post_data[256] = "");
    void webPrintln(char post_data[256] = "");
    //IFTTT
    void postIFTTT(char IFTTT_event[256],char IFTTT_key[256]);
};

extern KomainoControl komaino;
