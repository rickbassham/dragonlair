#include <Arduino.h>
#include "config.h"

// mbed libs
#include <Watchdog.h>
#include <Thread.h>
#include <Queue.h>

// 3rd party libs
#include <Ethernet.h>
#include <Bounce2.h>

// local libs
#include <bts7960.h>
#include <EthernetInit.h>
#include <commandparser.h>

typedef enum {
    cmd_close,
    cmd_open,
    cmd_stop,
} command;

typedef enum {
    state_idle,
    state_opening,
    state_closing,
    state_closed,
    state_open,
    state_unknown,
} state;

rtos::Queue<command, 10> command_queue;
#define WAIT_TIME rtos::Kernel::Clock::duration_u32(100)

rtos::Thread sensors;

Bounce safetyFullOpen = Bounce(SAFETY_FULLY_OPEN_PIN, 50);
Bounce safetyFullClose = Bounce(SAFETY_FULLY_CLOSED_PIN, 50);
Bounce openToggle = Bounce(OPEN_TOGGLE_PIN, 50);
Bounce closeToggle = Bounce(CLOSE_TOGGLE_PIN, 50);
Bounce emergencyClose = Bounce(EMERGENCY_CLOSE_PIN, 50);

BTS7960 motor(R_PWM, L_PWM, R_EN, L_EN, R_IS, L_IS);

state current_state = state_unknown;

byte mac[6] = MAC_ADDRESS;
EthernetUDP udp;
IPAddress remoteUDP;

double motor_current = 0;

void handleCommand(Stream *client, char *command, char *param);
EthernetServer server(TCP_PORT);

mbed::Watchdog &watchdog = mbed::Watchdog::get_instance();

bool isFullyOpen()
{
    return digitalRead(SAFETY_FULLY_OPEN_PIN) == LOW;
}

bool isFullyClosed()
{
    return digitalRead(SAFETY_FULLY_CLOSED_PIN) == LOW;
}

bool isMoving()
{
    return motor.isMoving();
}

bool isOpening()
{
    return isMoving() && motor.getDirection() == BTS7960_LEFT;
}

bool isClosing()
{
    return isMoving() && motor.getDirection() == BTS7960_RIGHT;
}

void updateState()
{
    if (isFullyClosed())
    {
        current_state = state_closed;
    }
    else if (isFullyOpen())
    {
        current_state = state_open;
    }
    else if (motor.isMoving())
    {
        if (motor.getDirection() == BTS7960_LEFT)
        {
            current_state = state_opening;
        }
        else
        {
            current_state = state_closing;
        }
    }
    else
    {
        current_state = state_idle;
    }

    double motor_current_left = motor.readLeftCurrent();
    double motor_current_right = motor.readRightCurrent();

    motor_current = max(motor_current_left, motor_current_right);
}

void checkStatus(Stream *client)
{
    updateState();

    switch (current_state)
    {
    case state_idle:
        client->print("ST#");
        #ifdef DEBUG
        Serial.print("ST#");
        #endif
        break;
    case state_opening:
        client->print("MO#");
        #ifdef DEBUG
        Serial.print("MO#");
        #endif
        break;
    case state_closing:
        client->print("MC#");
        #ifdef DEBUG
        Serial.print("MC#");
        #endif
        break;
    case state_closed:
        client->print("CL#");
        #ifdef DEBUG
        Serial.print("CL#");
        #endif
        break;
    case state_open:
        client->print("OP#");
        #ifdef DEBUG
        Serial.print("OP#");
        #endif
        break;
    }
}

void sendCommand(command cmd)
{
    command_queue.try_put(new command(cmd));
}

void handleCommand(Stream *client, char *command, char *param)
{
#ifdef DEBUG
    Serial.print(command);
    Serial.print(" ");
    Serial.println(param);
#endif
    // CS = Check Status
    if (strncmp(command, "CS", 2) == 0)
    {
        checkStatus(client);
    }
    else
    {
        // OP = Open
        if (strncmp(command, "OP", 2) == 0)
        {
            sendCommand(cmd_open);
        }

        // CL = Close
        if (strncmp(command, "CL", 2) == 0)
        {
            sendCommand(cmd_close);
        }

        // ST = Stop
        if (strncmp(command, "ST", 2) == 0)
        {
            sendCommand(cmd_stop);
        }

        if (strncmp(command, "RB", 2) == 0)
        {
            while (true)
                ;
        }

        checkStatus(client);
    }
}

void sensorThread()
{
    while (true)
    {
        safetyFullOpen.update();
        safetyFullClose.update();
        openToggle.update();
        closeToggle.update();
        emergencyClose.update();

        if (safetyFullOpen.fell() || safetyFullClose.fell())
        {
            sendCommand(cmd_stop);
        }

        if (emergencyClose.fell())
        {
            sendCommand(cmd_close);
        }

        if (openToggle.fell())
        {
            sendCommand(cmd_open);
        }
        else if (openToggle.rose())
        {
            sendCommand(cmd_stop);
        }

        if (closeToggle.fell())
        {
            sendCommand(cmd_close);
        }
        else if (closeToggle.rose())
        {
            sendCommand(cmd_stop);
        }

        delay(10);
    }
}

void setup()
{
    delay(5000);

    Serial.begin(9600);

#ifdef DEBUG
    Serial.println(F("Starting..."));
#endif

    pinMode(OPEN_TOGGLE_PIN, INPUT_PULLUP);
    pinMode(CLOSE_TOGGLE_PIN, INPUT_PULLUP);
    pinMode(SAFETY_FULLY_OPEN_PIN, INPUT_PULLUP);
    pinMode(SAFETY_FULLY_CLOSED_PIN, INPUT_PULLUP);
    pinMode(EMERGENCY_CLOSE_PIN, INPUT_PULLUP);

    motor.setup();

    initializeEthernetDHCP(mac);

    remoteUDP.fromString(UDP_ADDRESS);
    udp.begin(UDP_PORT);

    server.begin();

#ifdef DEBUG
    Serial.println(F("Ready"));
#endif

    sensors.start(sensorThread);

    watchdog.start(2000);
}

void open()
{
    if (!isFullyOpen())
    {
        motor.left(RUN_SPEED);
    }
}

void close()
{
    if (!isFullyClosed())
    {
        motor.right(RUN_SPEED);
    }
}

void stop()
{
    motor.stop();
}

void printState(Stream* s)
{
    s->print("DRAGONLAIR state=");
    s->print(current_state);
    s->print(",motor_current=");
    s->print(motor_current);
    s->print(",state_str=");

    switch (current_state)
    {
    case state_idle:
        s->print(F("idle"));
        break;
    case state_opening:
        s->print(F("opening"));
        break;
    case state_closing:
        s->print(F("closing"));
        break;
    case state_closed:
        s->print(F("closed"));
        break;
    case state_open:
        s->print(F("open"));
        break;
    }

    s->println();
}

void checkInput()
{
    EthernetClient client = server.available();
    if (client)
    {
        CommandParser parser(&client);

        parser.read();

        if (parser.isCommandReady())
        {
            handleCommand(&client, parser.getCommand(), parser.getParam());
        }
    }
}

void executeCommand(command cmd)
{
    switch (cmd)
    {
    case cmd_open:
        open();
        break;
    case cmd_close:
        close();
        break;
    case cmd_stop:
        stop();
        break;
    }

    updateState();
}

void loop()
{
    watchdog.kick();

    checkInput();

    command* cmd;
    if (command_queue.try_get_for(WAIT_TIME, &cmd))
    {
        Serial.println(F("Got command"));
        Serial.println(*cmd);

        executeCommand(*cmd);

        delete cmd;
    }

    ethernetMaintain();

    if (udp.beginPacket(remoteUDP, UDP_PORT))
    {
        printState(&udp);
        udp.endPacket();
    }
    else
    {
        Serial.println(F("Failed to send UDP packet"));
    }

#ifdef DEBUG
    printState(&Serial);
#endif
}
