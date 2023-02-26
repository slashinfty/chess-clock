#include <EEPROM.h>
#include <EncoderButton.h>
#include <TM1637.h>

// Initialize
EncoderButton Dial(4, 5, 6); // encoder, encoder, switch
EncoderButton BtnL(7);
EncoderButton BtnR(14);
TM1637 DispL(8, 9); // clk, dio
TM1637 DispR(16, 10); // clk, dio

// Global variables
int state = 0;
int setting = 0;
char turn = 'N';
char extra = 'N';
bool paused = false;
unsigned long pLTime = 0UL;
unsigned long pRTime = 0UL;
unsigned long pLExtra = 0UL;
unsigned long pRExtra = 0UL;
unsigned long playerTimeStart = 0UL;
unsigned long turnTimeStart = 0UL;
int lastDispTime = 0;

// Encoder & button handlers
void onBtnLPress(EncoderButton& eb) {
  if (state == 0) {
    state = 1;
    storeValues();
  }
  if (turn == 'L' && extra == 'B') {
    pLTime = pLTime + pLExtra;
    updateDisplay(DispL, pLTime);
  }
  if (turn != 'R') {
    turn = 'R';
    playerTimeStart = pRTime;
    turnTimeStart = millis();
    lastDispTime = int(playerTimeStart / 1000UL);
  }
}

void onBtnRPress(EncoderButton& eb) {
  if (state == 0) {
    state = 1;
    storeValues();
  }
  if (turn == 'R' && extra == 'B') {
    pRTime = pRTime + pRExtra;
    updateDisplay(DispR, pRTime);
  }
  if (turn != 'L') {
    turn = 'L';
    playerTimeStart = pLTime;
    turnTimeStart = millis();
    lastDispTime = int(playerTimeStart / 1000UL);
  }
}

void onDialPress(EncoderButton& eb) {
  if (state == 0) {
    setting = (setting + 1) % 7;
    if (setting == 0) {
      updateDisplay(DispL, pLTime);
      updateDisplay(DispR, pRTime);
    } else if (setting == 4) {
      updateDisplay(DispL, pLExtra);
      updateDisplay(DispR, pRExtra);
    } else if (setting == 6) {
      if (extra == 'B') {
        updateDisplay(DispL, {' ', ' ', 'B', 'O'});
        updateDisplay(DispR, {'N', 'U', 'S', ' '});
      } else if (extra == 'D') {
        updateDisplay(DispL, {' ', ' ', 'D', 'E'});
        updateDisplay(DispR, {'L', 'A', 'Y', ' '});
      } else if (extra == 'N') {
        updateDisplay(DispL, {' ', ' ', 'N', 'O'});
        updateDisplay(DispR, {'N', 'E', ' ', ' '});
      }
    }
  } else if (state == 1) {
    if (paused == false) {
      paused = true;
    } else if (paused == true) {
      paused = false;
      playerTimeStart = turn == 'L' ? pLTime : pRTime;
      turnTimeStart = millis();
    }
  }
}

void onDialTurn(EncoderButton& eb) {
  if (state == 0) {
    bool positive = eb.increment() > 0;
    int increment = abs(eb.increment());
    switch (setting) {
      case 0:
        pLTime = positive ? min(pLTime + (increment * 60000UL), 5999000UL) : increment * 60000UL > pLTime ? 0 : pLTime - (increment * 60000UL);
        updateDisplay(DispL, pLTime);
        break;
      case 1:
        pLTime = positive ? min(pLTime + (increment * 1000UL), 5999000UL) : increment * 1000UL > pLTime ? 0 : pLTime - (increment * 1000UL);
        updateDisplay(DispL, pLTime);
        break;
      case 2:
        pRTime = positive ? min(pRTime + (increment * 60000UL), 5999000UL) : increment * 60000UL > pRTime ? 0 : pRTime - (increment * 60000UL);
        updateDisplay(DispR, pRTime);
        break;
      case 3:
        pRTime = positive ? min(pRTime + (increment * 1000UL), 5999000UL) : increment * 1000UL > pRTime ? 0 : pRTime - (increment * 1000UL);
        updateDisplay(DispR, pRTime);
        break;
      case 4:
        pLExtra = positive ? min(pLExtra + (increment * 1000UL), 300000UL) : increment * 1000UL > pLExtra ? 0 : pLExtra - (increment * 1000UL);
        updateDisplay(DispL, pLExtra);
        break;
      case 5:
        pRExtra = positive ? min(pRExtra + (increment * 1000UL), 300000UL) : increment * 1000UL > pRExtra ? 0 : pRExtra - (increment * 1000UL);
        updateDisplay(DispR, pRExtra);
        break;
      case 6:
        if (extra == 'N') {
          extra = 'B';
          updateDisplay(DispL, {' ', ' ', 'B', 'O'});
          updateDisplay(DispR, {'N', 'U', 'S', ' '});
        } else if (extra == 'B') {
          extra = 'D';
          updateDisplay(DispL, {' ', ' ', 'D', 'E'});
          updateDisplay(DispR, {'L', 'A', 'Y', ' '});
        } else if (extra == 'D') {
          extra = 'N';
          updateDisplay(DispL, {' ', ' ', 'N', 'O'});
          updateDisplay(DispR, {'N', 'E', ' ', ' '});
        }
        break;
    }
    storeValues();
  }
}

void onDialLongPress(EncoderButton& eb) {
  state = 0;
  setting = 0;
  turn = 'N';
  paused = false;
  EEPROM.get(0, pLTime);
  EEPROM.get(4, pRTime);
  EEPROM.get(8, pLExtra);
  EEPROM.get(12, pRExtra);
  EEPROM.get(16, extra);
  updateDisplay(DispL, pLTime);
  updateDisplay(DispR, pRTime);
}

// Utility functions
void storeValues() {
  EEPROM.put(0, pLTime);
  EEPROM.put(4, pRTime);
  EEPROM.put(8, pLExtra);
  EEPROM.put(12, pRExtra);
  EEPROM.put(16, extra);
}

void updateDisplay(TM1637 disp, char str[]) {
  disp.clearDisplay();
  disp.point(false);
  disp.displayStr(str);
}

void updateDisplay(TM1637 disp, unsigned long ms) {
  disp.clearDisplay();
  char str[4] = {};
  int s = int(ms / 1000UL);
  int m = s / 60;
  s = s % 60;
  disp.point(m > 0);
  str[0] = m > 9 ? char(48 + (m / 10 % 10)) : ' ';
  str[1] = m > 0 ? char(48 + (m % 10)) : ' ';
  str[2] = m > 0 || s > 9 ? char(48 + (s / 10 % 10)) : ' ';
  str[3] = char(48 + (s % 10));
  disp.displayStr(str);
}

// Main functions
void setup() {
  EEPROM.get(0, pLTime);
  EEPROM.get(4, pRTime);
  EEPROM.get(8, pLExtra);
  EEPROM.get(12, pRExtra);
  EEPROM.get(16, extra);
  DispL.init();
  DispL.set(2);
  DispR.init();
  DispR.set(2);
  updateDisplay(DispL, pLTime);
  updateDisplay(DispR, pRTime);
  BtnL.setClickHandler(onBtnLPress);
  BtnR.setClickHandler(onBtnRPress);
  Dial.setClickHandler(onDialPress);
  Dial.setEncoderHandler(onDialTurn);
  Dial.setLongClickDuration(1500);
  Dial.setLongClickHandler(onDialLongPress);
}

void loop() {
  BtnL.update();
  BtnR.update();
  Dial.update();
  if (state == 1 && paused == false && pLTime > 0 && pRTime > 0) {
    unsigned long now = millis();
    unsigned long spent = now - turnTimeStart;
    if (extra == 'D') {
      unsigned long delayLength = turn === 'L' ? pLExtra : pRExtra;
      spent = spent <= delayLength ? 0 : spent - delayLength;
    }
    if (turn == 'L') {
      pLTime = spent > playerTimeStart ? 0 : playerTimeStart - spent;
      if (int(pLTime / 1000UL) != lastDispTime) {
        updateDisplay(DispL, pLTime);
        lastDispTime = int(pLTime / 1000UL);
      }
    } else if (turn == 'R') {
      pRTime = spent > playerTimeStart ? 0 : playerTimeStart - spent;
      if (int(pRTime / 1000UL) != lastDispTime) {
        updateDisplay(DispR, pRTime);
        lastDispTime = int(pRTime / 1000UL);
      }
    }
  } else if (pLTime <= 0 || pRTime <= 0) {
    exit(0);
  }
}
