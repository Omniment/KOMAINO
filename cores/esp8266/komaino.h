//
//  komaino.h
//
//
//  Created by 田村 塁 on 2017/01/25.
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
};

extern KomainoControl komaino;
