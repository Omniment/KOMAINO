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
void lightProc(byte x, byte y);  //画面描画
void slide();           //スライド動作
void dspWrite();        //ディスプレイ描画
void loopManager();     //ループで必ず行う処理

class KomainoControl {
public:
    void print(String slide_string , unsigned int slide_speed = 80);
    void printOnLoop(String slide_string , unsigned int slide_speed = 80);
    void drawDisplay(byte l1,byte l2,byte l3,byte l4,byte l5);
    void wifiSta(char* ssid_sta,char* password_sta);
    String wifiIP();
};

extern KomainoControl komaino;
