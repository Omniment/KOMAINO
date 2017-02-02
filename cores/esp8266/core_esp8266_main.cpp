/*
 main.cpp - platform initialization and context switching
 emulation
 
 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 main.cpp
 Copyright (c) 2017 Omniment INC. All rights reserved.
 
 This library is free software.
 
 ChangeLog
 - add include ArduinoOTA.h
 - add arduino_ota_init
 - add safeMode
 - change loop_wrapper
 
 */

//This may be used to change user task stack size:
//#define CONT_STACKSIZE 4096
#include <Arduino.h>

//#include "komaino.h"

#include "Schedule.h"
extern "C" {
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "cont.h"
}
#include <core_version.h>

#include "ArduinoOTA/ArduinoOTA.h"

#define LOOP_TASK_PRIORITY 1
#define LOOP_QUEUE_SIZE    1

#define OPTIMISTIC_YIELD_TIME_US 16000

void arduino_ota_init();
void safeMode();

struct rst_info resetInfo;

extern "C" {
    extern const uint32_t __attribute__((section(".ver_number"))) core_version = ARDUINO_ESP8266_GIT_VER;
    const char* core_release =
#ifdef ARDUINO_ESP8266_RELEASE
    ARDUINO_ESP8266_RELEASE;
#else
    NULL;
#endif
} // extern "C"

int atexit(void (*func)()) {
    return 0;
}

extern "C" void ets_update_cpu_frequency(int freqmhz);
void initVariant() __attribute__((weak));
void initVariant() {
}

extern void loop();
extern void setup();

void preloop_update_frequency() __attribute__((weak));
void preloop_update_frequency() {
#if defined(F_CPU) && (F_CPU == 160000000L)
    REG_SET_BIT(0x3ff00014, BIT(0));
    ets_update_cpu_frequency(160);
#endif
}

extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);

cont_t g_cont __attribute__ ((aligned (16)));
static os_event_t g_loop_queue[LOOP_QUEUE_SIZE];

static uint32_t g_micros_at_task_start;

extern "C" void esp_yield() {
    if (cont_can_yield(&g_cont)) {
        cont_yield(&g_cont);
    }
}

extern "C" void esp_schedule() {
    ets_post(LOOP_TASK_PRIORITY, 0, 0);
}

extern "C" void __yield() {
    if (cont_can_yield(&g_cont)) {
        esp_schedule();
        esp_yield();
    }
    else {
        panic();
    }
}

extern "C" void yield(void) __attribute__ ((weak, alias("__yield")));

extern "C" void optimistic_yield(uint32_t interval_us) {
    if (cont_can_yield(&g_cont) &&
        (system_get_time() - g_micros_at_task_start) > interval_us)
    {
        yield();
    }
}

static void loop_wrapper() {
    static bool setup_done = false;
    preloop_update_frequency();
    if(!setup_done) {
        
        //KOMAINO初期化
        komaino_init();
        arduino_ota_init();
        
        safeMode();
        
        Serial.println("Ready");
        
        setup();
#ifdef DEBUG_ESP_PORT
        DEBUG_ESP_PORT.setDebugOutput(true);
#endif
        setup_done = true;
    }
    
    ArduinoOTA.handle();
    severHandle();
    
    loop();
    
    dspWrite();
    
    run_scheduled_functions();
    esp_schedule();
}

static void loop_task(os_event_t *events) {
    g_micros_at_task_start = system_get_time();
    cont_run(&g_cont, &loop_wrapper);
    if (cont_check(&g_cont) != 0) {
        panic();
    }
}

static void do_global_ctors(void) {
    void (**p)(void) = &__init_array_end;
    while (p != &__init_array_start)
        (*--p)();
}

extern "C" void __gdb_init() {}
extern "C" void gdb_init(void) __attribute__ ((weak, alias("__gdb_init")));

extern "C" void __gdb_do_break(){}
extern "C" void gdb_do_break(void) __attribute__ ((weak, alias("__gdb_do_break")));

void init_done() {
    system_set_os_print(1);
    gdb_init();
    do_global_ctors();
    printf("\n%08x\n", core_version);
    esp_schedule();
}


extern "C" void user_init(void) {
    struct rst_info *rtc_info_ptr = system_get_rst_info();
    memcpy((void *) &resetInfo, (void *) rtc_info_ptr, sizeof(resetInfo));
    
    uart_div_modify(0, UART_CLK_FREQ / (115200));
    
    init();
    
    initVariant();
    
    cont_init(&g_cont);
    
    ets_task(loop_task,
             LOOP_TASK_PRIORITY, g_loop_queue,
             LOOP_QUEUE_SIZE);
    
    system_init_done_cb(&init_done);
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

void safeMode(){
    unsigned int push_time_safe = millis() + 3000;
    while(digitalRead(butA) == 0 & digitalRead(butB) == 0){
        if(push_time_safe < millis()){
            Serial.println("safeMode");
            while(1){
                lightProc(0x1F, 0x00);
                delay(300);
                lightProc(0x00, 0x00);
                delay(300);
                ArduinoOTA.handle();
            }
        }
    }
}
