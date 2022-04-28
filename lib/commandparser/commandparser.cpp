#include "commandparser.h"

CommandParser::CommandParser(Stream *input, char commandStart, char commandEnd) : _commandStart(commandStart), _commandEnd(commandEnd)
{
    _input = input;
    resetCommand();
}

void CommandParser::resetCommand()
{
    memset(_line, 0, MAX_COMMAND);
    memset(_cmd, 0, MAX_COMMAND);
    memset(_param, 0, MAX_COMMAND);
    _commandReady = false;
    _idx = 0;
}

bool CommandParser::read()
{
    bool anyRead = false;

    while (_input->available())
    {
        _inChar = _input->read();

        anyRead = true;

        if (_inChar == _commandStart)
        {
            resetCommand();
        }
        else if (_inChar == _commandEnd)
        {
            _commandReady = true;

            memset(_cmd, 0, MAX_COMMAND);
            memset(_param, 0, MAX_COMMAND);

            int len = strlen(_line);

            if (len >= 2)
            {
                strncpy(_cmd, _line, 2);
            }
            else
            {
                strncpy(_cmd, _line, len);
            }

            if (len > 2)
            {
                strncpy(_param, _line + 2, len - 2);
            }

            break;
        }
        else if (_idx < MAX_COMMAND)
        {
            _line[_idx++] = _inChar;
        }
        else
        {
            break;
        }
    } // end while _input->available()

    return anyRead;
}
