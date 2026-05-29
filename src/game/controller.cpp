#include "game/controller.h"

void getReadings(int &dx, int &dy, int &joyBtn, int &button)
{
    const unsigned long INPUT_DEBOUNCE_MS = 250;
    static unsigned long lastInputMs = 0;

    int valueX = analogRead(JOY_X_PIN);
    int valueY = analogRead(JOY_Y_PIN);

    dx = 0;
    dy = 0;

    int nextDx = 0;
    int nextDy = 0;

    if (valueX < JOY_LOW_THRESHOLD)
        nextDx = -1;
    else if (valueX > JOY_HIGH_THRESHOLD)
        nextDx = 1;

    if (valueY < JOY_LOW_THRESHOLD)
        nextDy = 1;
    else if (valueY > JOY_HIGH_THRESHOLD)
        nextDy = -1;

    bool nextButton = digitalRead(BTN_PIN) == HIGH;
    bool nextJoyBtn = digitalRead(JOY_SW_PIN) == LOW;

    unsigned long now = millis();
    bool hasInput = (nextDx != 0 || nextDy != 0 || nextButton || nextJoyBtn);

    if (hasInput && (now - lastInputMs >= INPUT_DEBOUNCE_MS))
    {
        dx = nextDx;
        dy = nextDy;
        button = nextButton ? 1 : 0;
        joyBtn = nextJoyBtn ? 1 : 0;
        lastInputMs = now;
    }
    else
    {
        button = 0;
        joyBtn = 0;
    }
}