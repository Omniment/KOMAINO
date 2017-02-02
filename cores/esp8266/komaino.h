//
//  komaino.h
//
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
void dspWrite();

class KomainoControl {
public:
    void print(String slide_string , unsigned int slide_speed);
    void drawDisplay(byte l1,byte l2,byte l3,byte l4,byte l5);
    void wifiSta(char* ssid_sta,char* password_sta);
};

extern KomainoControl komaino;


