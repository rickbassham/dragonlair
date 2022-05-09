#include <Arduino.h>

// #define DEBUG

#define MAC_ADDRESS {0x02, 0x0A, 0x41, 0xA0, 0x14, 0xB4};
#define UDP_PORT 40000
#define UDP_ADDRESS "192.168.10.5"

/*
Ethernet uses the following pins:
16
17
18
19
20
21
*/

#define SAFETY_FULLY_OPEN_PIN 2
#define SAFETY_FULLY_CLOSED_PIN 3

#define OPEN_TOGGLE_PIN 4
#define CLOSE_TOGGLE_PIN 5

#define R_PWM 9
#define L_PWM 8
#define R_EN 7
#define L_EN 6
#define R_IS A0
#define L_IS A1

#define EMERGENCY_CLOSE_PIN 13

#define RUN_SPEED 255

#define MAX_CLIENTS 5

#define TCP_PORT 3000
