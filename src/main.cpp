#include <Arduino.h>
#include <IRsend.h>

#define IR_LED_PIN 6   // Пін ІЧ-діода (ТІЛЬКИ ЧЕРЕЗ ТРАНЗИСТОР, коли припаяєш назад!)
#define BUTTON_PIN 5   

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
uint16_t rawBuffer[512]; // Збільшив буфер із запасом

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

bool sendRegionCodes(const IrCode* const* regionCodes, uint8_t numCodes, const char* regionName) {
  Serial.printf("\n>>> СТАРТ АТАКИ: %s (%d кодів) <<<\n", regionName, numCodes);
  
  for (uint8_t i = 0; i < numCodes; i++) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
      if (digitalRead(BUTTON_PIN) == LOW) {
        Serial.println("\n[!] АТАКУ СКАСОВАНО КОРИСТУВАЧЕМ!");
        while(digitalRead(BUTTON_PIN) == LOW) { delay(10); } 
        return false; 
      }
    }

    currentPowerCode = regionCodes[i];
    uint8_t freq = currentPowerCode->timer_val;
    uint8_t numpairs = currentPowerCode->numpairs;
    uint8_t bitcompression = currentPowerCode->bitcompression;

    // ЗАПОБІЖНИК: Уникаємо ділення на нуль
    if (freq == 0) {
      freq = 38; 
    }

    bitsLeft = 0;
    codePtr = 0;

    // Захист від переповнення буфера
    if (numpairs * 2 > 512) {
      Serial.println("Помилка: Код занадто великий!");
      continue;
    }

    for (uint8_t k = 0; k < numpairs; k++) {
      uint16_t ti = (readBits(bitcompression)) * 2;
      rawBuffer[k * 2] = currentPowerCode->times[ti] * 10;
      rawBuffer[(k * 2) + 1] = currentPowerCode->times[ti + 1] * 10;
    }

    irsend.sendRaw(rawBuffer, (numpairs * 2), freq);
    
    if (i % 5 == 0) {
      Serial.printf("Прогрес %s: %d/%d (Freq: %dkHz)\n", regionName, i + 1, numCodes, freq);
    }
    
    delay(50); 
  }
  return true; 
}

void setup() {
  // Даємо час USB підключитись і прокинутись після ребуту
  delay(3000); 
  
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  
  Serial.println("\n--- ЗАПУСК СИСТЕМИ ---");
  irsend.begin();
  
  Serial.println("==================================");
  Serial.println("  TV-B-Gone ESP32-S3 ГОТОВИЙ");
  Serial.println("  Натисни кнопку BOOT для запуску");
  Serial.println("==================================");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); 
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("\nКнопка натиснута. Відпусти для старту...");
      
      while(digitalRead(BUTTON_PIN) == LOW) { delay(10); }
      
      bool notCancelled = sendRegionCodes(EUpowerCodes, num_EUcodes, "Європа (EU)");
      if (notCancelled) {
        sendRegionCodes(NApowerCodes, num_NAcodes, "Америка (NA)");
      }
      
      Serial.println(">>> АТАКА ЗАКІНЧЕНА АБО ЗУПИНЕНА <<<");
      Serial.println("Натисни кнопку знову для нового запуску.\n");
    }
  }
}