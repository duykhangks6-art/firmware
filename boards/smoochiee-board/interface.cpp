#include "core/bus_HAL.h"
#include "core/powerSave.h"

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/



void _setup_gpio() {

    pinMode(UP_BTN, INPUT_PULLUP);
    pinMode(DW_BTN, INPUT_PULLUP);


    pinMode(CC1101_SS_PIN, OUTPUT);
    pinMode(NRF24_SS_PIN, OUTPUT);

    digitalWrite(CC1101_SS_PIN, HIGH);
    digitalWrite(NRF24_SS_PIN, HIGH);
    // Starts SPI instance for CC1101 and NRF24 with CS pins blocking communication at start

    bruceConfigPins.rfModule = CC1101_SPI_MODULE;
    bruceConfigPins.irRx = RXLED;
}
bool isCharging() {
        return false;
        }

int getBattery() {
        return 100;
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
    static unsigned long upTime = 0;
    static unsigned long dwTime = 0;

        if (millis() - tm < 40) return;

        bool up = (digitalRead(UP_BTN) == BTN_ACT);
        bool dw = (digitalRead(DW_BTN) == BTN_ACT);

    if (up || dw) {
        tm = millis();

        if (!wakeUpScreen())
        AnyKeyPress = true;
    else
    return;
}

// ======= NÚT UP =======
static bool upHandled = false;

if (up) {
    if (upTime == 0) upTime = millis();

        if (!upHandled && millis() - upTime >= 250) {
                EscPress = true;
                        upHandled = true;
                            }
                            } else {
                                if (upTime != 0 && !upHandled) {
                                        PrevPress = true;
                                                UpPress = true;
                                                        PrevPagePress = true;
                                                            }
                                                                upTime = 0;
                                                                    upHandled = false;
                                                                    }

// ======= NÚT DOWN =======
static bool dwHandled = false;

if (dw) {
    if (dwTime == 0) dwTime = millis();

        if (!dwHandled && millis() - dwTime >= 250) {
                SelPress = true;
                        dwHandled = true;
                            }
                            } else {
                                if (dwTime != 0 && !dwHandled) {
                                        NextPress = true;
                                                DownPress = true;
                                                        NextPagePress = true;
                                                            }
                                                                dwTime = 0;
                                                                    dwHandled = false;
                                                                    }
 }

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {
        // Chờ nhả nút
    while (digitalRead(UP_BTN) == BTN_ACT) {
     delay(10);
                        }

                            delay(100);

                                // Đánh thức bằng GPIO0 khi kéo xuống LOW
                                    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);

                                        tft.writecommand(0x10);
                                            esp_deep_sleep_start();
                                            }

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turn off the device (name is odd btw)
**********************************************************************/
/**
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

