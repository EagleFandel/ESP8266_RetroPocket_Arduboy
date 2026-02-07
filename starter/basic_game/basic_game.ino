#include <Arduboy2.h>
#include <EEPROM.h>

Arduboy2 arduboy;

static uint32_t bootCount = 0;

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30);

#ifdef ESP8266
  EEPROM.begin(1024);
#endif

  bootCount = EEPROM.read(0) |
              (EEPROM.read(1) << 8) |
              (EEPROM.read(2) << 16) |
              (EEPROM.read(3) << 24);

  bootCount++;

  EEPROM.write(0, bootCount & 0xFF);
  EEPROM.write(1, (bootCount >> 8) & 0xFF);
  EEPROM.write(2, (bootCount >> 16) & 0xFF);
  EEPROM.write(3, (bootCount >> 24) & 0xFF);

#ifdef ESP8266
  EEPROM.commit();
#endif
}

void loop() {
  if (!arduboy.nextFrame()) return;

  arduboy.pollButtons();
  arduboy.clear();

  arduboy.setCursor(0, 0);
  arduboy.print(F("BASIC GAME"));

  arduboy.setCursor(0, 10);
  arduboy.print(F("Boots: "));
  arduboy.print(bootCount);

  arduboy.setCursor(0, 22);
  arduboy.print(F("Pressed: "));
  arduboy.print(arduboy.buttonsState(), BIN);

  if (arduboy.pressed(LEFT_BUTTON)) {
    arduboy.fillRect(10, 40, 8, 8, WHITE);
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    arduboy.fillRect(30, 40, 8, 8, WHITE);
  }
  if (arduboy.pressed(UP_BUTTON)) {
    arduboy.fillRect(50, 40, 8, 8, WHITE);
  }
  if (arduboy.pressed(DOWN_BUTTON)) {
    arduboy.fillRect(70, 40, 8, 8, WHITE);
  }
  if (arduboy.pressed(A_BUTTON)) {
    arduboy.fillRect(90, 40, 8, 8, WHITE);
  }
  if (arduboy.pressed(B_BUTTON)) {
    arduboy.fillRect(110, 40, 8, 8, WHITE);
  }

  arduboy.display();
}

