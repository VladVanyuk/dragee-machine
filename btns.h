#ifndef BTNS_DEF_H
#define BTNS_DEF_H

#include <Arduino.h>
#include <EncButton.h>
#include "settings.h"


#define FULL_OFF_COOLING \
  { \
    COOLING_OURSCREEN; \
    COOLING_OFF; \
  }

#define FULL_OFF_HEAP \
  { \
    HEAP_OURSCREEN; \
    HEAP_OFF; \
  }


class ButtonSwitch : public VirtButton {
public:

  void (*callbackOnPress)() = NULL;

  ButtonSwitch() {}

  ButtonSwitch(uint8_t btnPin, uint8_t ledPin, uint8_t btnMode = INPUT_PULLUP, uint8_t btnLevel = LOW, uint8_t ledInitState = LOW) {
    init(btnPin, ledPin, btnMode, btnLevel, ledInitState);
  }

  void init(uint8_t btnPin, uint8_t ledPin, uint8_t btnMode = INPUT_PULLUP, uint8_t btnLevel = LOW, uint8_t ledInitState = LOW) {
    this->btnPin = btnPin;
    EB_mode(btnPin, btnMode);
    setBtnLevel(btnLevel);

    setLed(ledPin, ledInitState);
  }

  bool read() {
    return EB_read(btnPin) ^ bf.read(EB_INV);
  }

  bool tick() {
    bool tick = VirtButton::tick(EB_read(btnPin));
    if (tick) {
      pressedBtn();

      return true;
    }
    return false;
  }

  bool tickRaw() {
    return VirtButton::tickRaw(EB_read(btnPin));
  }


  void setLed(uint8_t ledPin, uint8_t ledState = LOW) {
    this->ledPin = ledPin;
    this->ledState = ledState;
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, ledState);
  }



  void toggleLed() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }


  void pressedBtn() {
    uint16_t btnState = VirtButton::action();
    switch (btnState) {

      case EB_CLICK:
        Serial.println("click");
        toggleLed();
        callCallback();
        break;
      default:
        Serial.println("other action");
    }
  }

  void attachCallback(void (*callback)()) {
    this->callbackOnPress = callback;
  }

  void callCallback() {
    if (callbackOnPress != NULL) {
      callbackOnPress();
    }
  }

private:
  uint8_t btnPin;
  uint8_t ledPin;
  uint8_t ledState;
};


ButtonSwitch btn1(BTN1_PIN, LED_PIN1, INPUT_PULLUP, LOW);
ButtonSwitch btn2(BTN2_PIN, LED_PIN2, INPUT_PULLUP, LOW);
ButtonSwitch btnSwitch(BTN3_PIN, LED_PIN3, INPUT_PULLUP, LOW);


void callbackSwitch() {

  switch (btnSwitch.action()) {

    case EB_HOLD:
      FAN_ON;
      Serial.println("HOLD");
      break;

    case EB_RELEASE:
      FAN_OFF;
      Serial.println("RELEASE");
      break;
    default:
      Serial.println("ERROR");
  }
}



void callbackBtn1() {
  switch (btn1.action()) {

    case EB_CLICK:
      Serial.println("Btn1 clicked");
      if (!(HEAP_ONSCREEN)) {
        HEAP_ONSCREEN;
        FULL_OFF_COOLING;
        STOP_OFF;
      } else if (HEAP_ONSCREEN && FAN_CHECK && !(HEAP_CHECK)) {
        HEAP_ON;
        FULL_OFF_COOLING;
        STOP_OFF;
      } else if (HEAP_ONSCREEN && !(HEAP_CHECK) && !(FAN_CHECK)) {
        //Here plase code to print in MENU such as "FAN is not on !"

      } else if (HEAP_ONSCREEN && HEAP_CHECK) {
        FULL_OFF_HEAP;
        STOP_ON;
      }
      break;

    default:
      break;
  }
}

void callbackBtn2() {
  switch (btn2.action()) {
    Serial.println("Btn2 clicked");
    case EB_CLICK:
      if (!(COOLING_ONSCREEN)) {
        COOLING_ONSCREEN;
        FULL_OFF_HEAP;
        STOP_OFF;
      } else if (COOLING_ONSCREEN && FAN_CHECK && !(COOLING_CHECK)) {
        COOLING_ON;
        FULL_OFF_HEAP;
        STOP_OFF;
      } else if (COOLING_ONSCREEN && !(COOLING_CHECK) && !(FAN_CHECK)) {
        //Here plase code to print in MENU such as "FAN is not on !"

      } else if (COOLING_ONSCREEN && COOLING_CHECK) {
        FULL_OFF_COOLING;
        STOP_ON;
      }
      break;

    default:
      break;
  }
}



void btnsSetup() {
  btn1.attachCallback(callbackBtn1);
  btn2.attachCallback(callbackBtn2);
  btnSwitch.attachCallback(callbackSwitch);
}

void btnsLoop() {
  btn1.tick();
  btn2.tick();
  btnSwitch.tick();
}


#endif