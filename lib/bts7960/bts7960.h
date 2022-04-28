#ifndef bts7960_h
#define bts7960_h

#define BTS7960_STOPPED 0
#define BTS7960_LEFT 1
#define BTS7960_RIGHT 2

#ifndef BTS7960_CURRENT_SAMPLES
#define BTS7960_CURRENT_SAMPLES 10
#endif

class BTS7960
{
public:
    BTS7960(int r_pwm, int l_pwm, int r_en = -1, int l_en = -1, int r_is = -1, int l_is = -1);

    void setup();

    void left(int speed);
    void right(int speed);

    void stop();

    bool isMoving()
    {
        return _isMoving;
    }

    int getDirection()
    {
        return _direction;
    }

    double readRightCurrent();
    double readLeftCurrent();

private:
    void start();

    double readCurrent(int pin);

    int _r_pwm;
    int _l_pwm;
    int _r_en;
    int _l_en;
    int _r_is;
    int _l_is;

    bool _isMoving;
    int _direction;

    double _current[BTS7960_CURRENT_SAMPLES];
    int _currentIndex = 0;
};

#endif
