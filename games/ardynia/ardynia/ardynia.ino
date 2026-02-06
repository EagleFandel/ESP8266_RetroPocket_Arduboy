#include <EEPROM.h>
#include <Arduboy2.h>
#include <math.h>

#include "src/sfx.h"
#include "src/game.h"
#include "src/renderer.h"
#include "src/util.h"
#include "src/tileBitmaps.h"

Arduboy2Base arduboy;
ArduboyPlaytune tones(arduboy.audio.enabled);

Renderer renderer(arduboy);
Game game;

void setup() {
    #ifdef ESP8266
    EEPROM.begin(1024);
    
    // ESP8266: 手动初始化OLED
    extern SSD1306Brzo oled;
    oled.init();
    oled.flipScreenVertically();
    arduboy.sBuffer = oled.buffer;
    #endif

    arduboy.boot();
    arduboy.audio.begin();
    Sfx::init(&tones);

#ifdef SERIAL_LOG
    Serial.begin(9600);
    delay(1500);
    LOG("setup done");
#endif
}

uint8_t loopCounter = 1;

void loop() {
    if (!arduboy.nextFrame()) {
        return;
    }

    loopCounter += 1;

    if (loopCounter == 61) {
        loopCounter = 1;
    }

    arduboy.clear();
    arduboy.pollButtons();
    
    game.update(loopCounter);
    game.render(loopCounter);
    arduboy.display();
}

