# ⚡ ESP32-C3 Super Mini TV-B-Gone

![PlatformIO](https://img.shields.io/badge/Platform-PlatformIO-orange?style=flat-square&logo=platformio)
![Framework](https://img.shields.io/badge/Framework-Arduino-blue?style=flat-square&logo=arduino)
![MCU](https://img.shields.io/badge/MCU-ESP32--C3-green?style=flat-square&logo=espressif)

Компактный и мощный инструмент для выключения телевизоров. Портированная и оптимизированная версия классического **TV-B-Gone** специально для форм-фактора **ESP32-C3 Super Mini**. 

Скоро будут новые апдейты!

---

## 🌍 English Description

### Features
* **Full Database**: Over 200 IR codes for North America (NA) and Europe (EU).
* **Compact Size**: Designed for the tiny ESP32-C3 Super Mini board.
* **Stop/Start**: Toggle the attack using the on-board button.
* **Serial Feedback**: Real-time progress monitoring via 115200 baud.

### 🔌 Pinout
| component | pin  ESP32-C3 | note |
|-----------|--------------|------------|
| **IR LED** | GPIO 6 | Requires a transistor like 2N2222! |
| **Button** | GPIO 5 | none |


### 🚀 Quick Start
1. Open project in **VS Code** with **PlatformIO**.
2. Connect your ESP32-C3.
3. Hit **Upload**.
4. Point the LED at a TV and press the button!

---

## 🇷🇺 Описание на русском

### Возможности
* **Полная база**: Более 200 ИК-кодов для Северной Америки (NA) и Европы (EU).
* **Миниатюрность**: Идеально ложится в прошивку для платы ESP32-C3 Super Mini.
* **Управление**: Запуск и остановка перебора одной кнопкой.
* **Обратная связь**: Весь процесс логируется в Serial Monitor (115200).

### 🔌 Подключение
| Компонент | Пин ESP32-C3 | Примечание |
|-----------|--------------|------------|
| **IR LED** | GPIO 6 | Подключать только через транзистор! |
| **Кнопка** | GPIO 5 | Нету |

### 🛠 Как прошить
1. Открой папку с проектом в **VS Code** (должен быть установлен PlatformIO).
2. Подключи плату через USB.
3. Нажми кнопку **Upload** (стрелочка внизу).
4. После прошивки направь ИК-диод на ТВ и нажми кнопку для начала атаки.

---

## 📦 Project Structure
* `src/main.cpp` — Logic & PWM generation.
* `src/tvbgcodes.h` — Compressed IR code database.
* `platformio.ini` — Build environment settings.

---

**Contact:**
Discord: den4ik4774
Telegram: den4ik148

## ⚠️ Disclaimer / Отказ от ответственности
* **English**: This project is for educational purposes only. I am not responsible for any misuse. Don't be a nuisance in public places.
* **Русский**: Проект создан исключительно в образовательных целях. Автор не несет ответственности за ваши действия. Пожалуйста, не мешайте людям в общественных местах.

---


