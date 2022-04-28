#include "bts7960.h"

#include <Arduino.h>

BTS7960::BTS7960(int r_pwm, int l_pwm, int r_en, int l_en, int r_is, int l_is)
{
    _r_pwm = r_pwm;
    _l_pwm = l_pwm;
    _r_en = r_en;
    _l_en = l_en;
    _r_is = r_is;
    _l_is = l_is;

    _isMoving = false;
    _direction = BTS7960_STOPPED;

    for (int i = 0; i < BTS7960_CURRENT_SAMPLES; i++)
    {
        _current[i] = 0;
    }

    _currentIndex = 0;
}

void BTS7960::setup()
{
    pinMode(_r_pwm, OUTPUT);
    pinMode(_l_pwm, OUTPUT);

    if (_r_en > 0)
    {
        pinMode(_r_en, OUTPUT);
    }
    if (_l_en > 0)
    {
        pinMode(_l_en, OUTPUT);
    }
    if (_r_is > 0)
    {
        pinMode(_r_is, INPUT);
    }
    if (_l_is > 0)
    {
        pinMode(_l_is, INPUT);
    }
}

void BTS7960::left(int speed)
{
    stop();

    _direction = BTS7960_LEFT;

    analogWrite(_l_pwm, speed);
    analogWrite(_r_pwm, 0);

    delay(100);

    start();
}

void BTS7960::right(int speed)
{
    stop();

    _direction = BTS7960_RIGHT;

    analogWrite(_l_pwm, 0);
    analogWrite(_r_pwm, speed);

    delay(100);

    start();
}

void BTS7960::stop()
{
    if (_l_en > 0)
    {
        digitalWrite(_l_en, LOW);
    }
    if (_r_en > 0)
    {
        digitalWrite(_r_en, LOW);
    }

    analogWrite(_l_pwm, 0);
    analogWrite(_r_pwm, 0);

    _isMoving = false;
    _direction = BTS7960_STOPPED;

    for (int i = 0; i < BTS7960_CURRENT_SAMPLES; i++)
    {
        _current[i] = 0;
    }

    _currentIndex = 0;

    delay(100);
}

void BTS7960::start()
{
    if (_l_en > 0)
    {
        digitalWrite(_l_en, HIGH);
    }
    if (_r_en > 0)
    {
        digitalWrite(_r_en, HIGH);
    }

    _isMoving = true;

    delay(100);
}

double BTS7960::readCurrent(int pin)
{
    if (pin <= 0)
        return 0;

    int rawValue = analogRead(pin);
    double voltage = rawValue * (3.3 / 1023.0);

    double rawCurrent = voltage / 470;

    double current = rawCurrent * 8500;

    _current[_currentIndex % BTS7960_CURRENT_SAMPLES] = current;
    _currentIndex = (_currentIndex + 1) % BTS7960_CURRENT_SAMPLES;

    double ret = 0;

    for (int i = 0; i < BTS7960_CURRENT_SAMPLES; i++)
    {
        ret += _current[i];
    }

    return ret / BTS7960_CURRENT_SAMPLES;
}

double BTS7960::readRightCurrent()
{
    return readCurrent(_r_is);
}

double BTS7960::readLeftCurrent()
{
    return readCurrent(_l_is);
}
