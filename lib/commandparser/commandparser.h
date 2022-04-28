#ifndef commandparser_h
#define commandparser_h

#include <Stream.h>
#include <stdint.h>

#define MAX_COMMAND 16

class CommandParser
{
public:
    CommandParser(Stream *input, char commandStart = ':', char commandEnd = '#');

    bool isCommandReady() { return _commandReady; }
    void resetCommand();
    bool read();

    char *getCommand() { return _cmd; }
    char *getParam() { return _param; }
    char *getLine() { return _line; }

private:
    Stream *_input = nullptr;

    char _commandStart;
    char _commandEnd;

    char _inChar;
    int _idx = 0;

    char _cmd[MAX_COMMAND];
    char _param[MAX_COMMAND];
    char _line[MAX_COMMAND];

    bool _commandReady;
};

#endif