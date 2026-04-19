#include <Arduino.h>
#include <IRsend.h>

#define IR_LED_PIN 6 
#define BUTTON_PIN 5 
#define ONBOARD_LED 8 

struct IrCode {
  uint8_t timer_val;
  uint8_t numpairs;
  uint8_t bitcompression;
  const uint16_t* times;
  const uint8_t* codes;
};

template <typename T, size_t N>
constexpr size_t NUM_ELEM(const T (&)[N]) { return N; }

static inline constexpr uint8_t freq_to_timerval(uint32_t hz) {
  return hz / 1000;
}

#include "tvbgcodes.h"

IRsend irsend(IR_LED_PIN);

uint8_t bitsLeft = 0;
uint8_t currentBits = 0;
uint8_t codePtr = 0;
const IrCode* currentPowerCode = nullptr;
uint16_t rawBuffer[512];

bool sendRegionCodes(const IrCode* const* regionCodes, uint8_t numCodes, const char* regionName);

uint8_t readBits(uint8_t count) {
  if (currentPowerCode == nullptr) return 0;
  uint8_t value = 0;
  for (uint8_t i = 0; i < count; i++) {
    if (bitsLeft == 0) {
      currentBits = currentPowerCode->codes[codePtr++];
      bitsLeft = 8;
    }
    bitsLeft--;
    value |= (((currentBits >> bitsLeft) & 1) << (count - 1 - i));
  }
  return value;
}

void runJammer() {
  digitalWrite(ONBOARD_LED, LOW);
  while (true) {
    unsigned long startBurst = millis();
    while (millis() - startBurst < 200) {
      irsend.enableIROut(38);
      irsend.mark(500); 
      irsend.space(500);
      if (digitalRead(BUTTON_PIN) == LOW) {
        goto stop_jammer;
      }
    }
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) break;
  }
stop_jammer:
  digitalWrite(ONBOARD_LED, HIGH);
  while (digitalRead(BUTTON_PIN) == LOW) { delay(10); }
}

bool sendRegionCodes(const IrCode* const* regionCodes, uint8_t numCodes, const char* regionName) {
  for (uint8_t i = 0; i < numCodes; i++) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
      if (digitalRead(BUTTON_PIN) == LOW) {
        while(digitalRead(BUTTON_PIN) == LOW) { delay(10); } 
        return false; 
      }
    }
    currentPowerCode = regionCodes[i];
    uint8_t freq = currentPowerCode->timer_val ? currentPowerCode->timer_val : 38;
    bitsLeft = 0;
    codePtr = 0;
    if (currentPowerCode->numpairs * 2 > 512) continue;
    for (uint8_t k = 0; k < currentPowerCode->numpairs; k++) {
      uint16_t ti = (readBits(currentPowerCode->bitcompression)) * 2;
      rawBuffer[k * 2] = currentPowerCode->times[ti] * 10;
      rawBuffer[(k * 2) + 1] = currentPowerCode->times[ti + 1] * 10;
    }
    irsend.sendRaw(rawBuffer, (currentPowerCode->numpairs * 2), freq);
    delay(50); 
  }
  return true; 
}

void setup() {
  delay(3000); 
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, HIGH);
  irsend.begin();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); 
    while(digitalRead(BUTTON_PIN) == LOW); 
    unsigned long firstRelease = millis();
    bool doubleClick = false;
    while (millis() - firstRelease < 400) {
      if (digitalRead(BUTTON_PIN) == LOW) {
        doubleClick = true;
        while(digitalRead(BUTTON_PIN) == LOW);
        break;
      }
    }
    if (doubleClick) {
      runJammer();
    } else {
      if (sendRegionCodes(EUpowerCodes, num_EUcodes, "EU")) {
        sendRegionCodes(NApowerCodes, num_NAcodes, "NA");
      }
    }
  }
}
