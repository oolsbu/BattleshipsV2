#pragma once
#include <Arduino.h>

inline void getReadings(int& dx, int& dy, int& joyBtn, int& button) {
    const int LOW_THRESHOLD = 400;
    const int HIGH_THRESHOLD = 600;
    const int JOY_BTN_THRESHOLD = 850;

    int valueX = analogRead(A0);
    int valueY = 1023; // analogRead(A1); // Placeholder for Y-axis reading

    dx = 0;
    dy = 0;

    if (valueX < LOW_THRESHOLD) dx = 1;
    else if (valueX > HIGH_THRESHOLD) dx = -1;

    if (valueY < LOW_THRESHOLD) dy = 1;
    else if (valueY > HIGH_THRESHOLD) dy = -1;
    
    button = digitalRead(2);
    joyBtn = (valueX > JOY_BTN_THRESHOLD) ? 1 : 0;
}