#include "core/bus_HAL.h"
#include "core/powerSave.h"

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/

// Power handler for battery detection
#ifdef XPOWERS_CHIP_BQ25896
#include <Wire.h>
#include <XPowersLib.h>
XPowersPPM PPM;
#endif

void _setup_gpio() {

    pinMode(UP_BTN, INPUT_PULLUP);
    pinMode(DW_BTN, INPUT_PULLUP);

    button_config_t bt1 = {
            .type = BUTTON_TYPE_GPIO,
                .long_press_time = 250,
                    .short_press_time = 40,
                        .gpio_button_config = {
                                .gpio_num = DW_BTN,
                                        .active_level = 0,
                                            },
                                            };

                                            button_config_t bt2 = {
                                                .type = BUTTON_TYPE_GPIO,
                                                    .long_press_time = 250,
                                                        .short_press_time = 40,
                                                            .gpio_button_config = {
                                                                    .gpio_num = UP_BTN,
                                                                            .active_level = 0,
                                                                                },
                                                                                };
                                                                                p
    }
    pinMode(CC1101_SS_PIN, OUTPUT);
    pinMode(NRF24_SS_PIN, OUTPUT);

    digitalWrite(CC1101_SS_PIN, HIGH);
    digitalWrite(NRF24_SS_PIN, HIGH);
    // Starts SPI instance for CC1101 and NRF24 with CS pins blocking communication at start

    bruceConfigPins.rfModule = CC1101_SPI_MODULE;
    bruceConfigPins.irRx = RXLED;
    setSysI2CBus(&Wire); // PMU lives on the default Wire object
    Wire.setPins(SYS_I2C_SDA, SYS_I2C_SCL);
    // Wire.begin();
    bool pmu_ret = false;
    Wire.begin(SYS_I2C_SDA, SYS_I2C_SCL);
    pmu_ret = PPM.init(Wire, SYS_I2C_SDA, SYS_I2C_SCL, BQ25896_SLAVE_ADDRESS);
    if (pmu_ret) {
        PPM.setSysPowerDownVoltage(3300);
        PPM.setInputCurrentLimit(3250);
        Serial.printf("getInputCurrentLimit: %d mA\n", PPM.getInputCurrentLimit());
        PPM.disableCurrentLimitPin();
        PPM.setChargeTargetVoltage(4208);
        PPM.setPrechargeCurr(64);
        PPM.setChargerConstantCurr(832);
        PPM.getChargerConstantCurr();
        Serial.printf("getChargerConstantCurr: %d mA\n", PPM.getChargerConstantCurr());
        PPM.enableMeasure(PowersBQ25896::CONTINUOUS);
        PPM.disableOTG();
        PPM.enableCharge();
    }
}
bool isCharging() {
    // PPM.disableBatterPowerPath();
    return PPM.isCharging();
}

int getBattery() {
    int voltage = PPM.getBattVoltage();
    int percent = (voltage - 3300) * 100 / (float)(4150 - 3350);

    if (percent < 0) return 1;
    if (percent > 100) percent = 100;

    if (PPM.isCharging() && percent >= 97) {
        PPM.disableBatLoad();
        percent = 95; // estimate still charging
    }

    if (PPM.isChargeDone()) { percent = 100; }

    return percent;
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if (brightval == 0) {
        analogWrite(TFT_BL, brightval);
    } else {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
        analogWrite(TFT_BL, bl);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
        static unsigned long tm = 0;
            static unsigned long upPressTime = 0;

                if (millis() - tm < 200 && !LongPress) return;

                    bool up = digitalRead(UP_BTN);
                        bool dw = digitalRead(DW_BTN);

                            // Giữ nút UP 3 giây để tắt nguồn
                                if (up == BTN_ACT) {
                                        if (upPressTime == 0) upPressTime = millis();
                                                if (millis() - upPressTime >= 3000) {
                                                            powerOff();
                                                                    }
                                                                        } else {
                                                                                upPressTime = 0;
                                                                                    }

                                                                                        if (up == BTN_ACT || dw == BTN_ACT) {
                                                                                                tm = millis();
                                                                                                        if (!wakeUpScreen()) AnyKeyPress = true;
                                                                                                                else return;
                                                                                                                    }

                                                                                                                        if (up == BTN_ACT) {
                                                                                                                                PrevPress = true;
                                                                                                                                        UpPress = true;
                                                                                                                                                PrevPagePress = true;
                                                                                                                                                    }

                                                                                                                                                        if (dw == BTN_ACT) {
                                                                                                                                                                NextPress = true;
                                                                                                                                                                        DownPress = true;
                                                                                                                                                                                NextPagePress = true;
                                                                                                                                                                                    }
                                                                                                                                                                                    }
}
/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {
    esp_sleep_enable_ext0_wakeup((gpio_num_t)UP_BTN, BTN_ACT);
    esp_deep_sleep_start();
}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turn off the device (name is odd btw)
**********************************************************************/
/*
void checkReboot() {
    int countDown = 0;
   if (digitalRead(L_BTN) == BTN_ACT && digitalRead(R_BTN) == BTN_ACT) {
        uint32_t time_count = millis();
       while (digitalRead(L_BTN) == BTN_ACT && digitalRead(R_BTN) == BTN_ACT) {
            // Display poweroff bar only if holding button
            if (millis() - time_count > 500) {
                if (countDown == 0) {
                    int textWidth = tft.textWidth("PWR OFF IN 3/3", 1);
                    tft.fillRect(tftWidth / 2 - textWidth / 2, 7, textWidth, 18, bruceConfig.bgColor);
                }
                tft.setTextSize(1);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                if (countDown < 4)
                    tft.drawCentreString("PWR OFF IN " + String(countDown) + "/3", tftWidth / 2, 12, 1);
                else {
                    tft.fillScreen(bruceConfig.bgColor);
                    while (digitalRead(L_BTN) == BTN_ACT || digitalRead(R_BTN) == BTN_ACT);
                    delay(200);
                    powerOff();
                }
                delay(10);
            }
       }

        // Clear text after releasing the button
        delay(30);
        if (millis() - time_count > 500) {
            tft.fillRect(60, 12, tftWidth - 60, tft.fontHeight(1), bruceConfig.bgColor);
            drawStatusBar();
        }
    }
}*/
void checkReboot() {
        return;
        }

