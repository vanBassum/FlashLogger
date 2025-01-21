#pragma once


class LogEntry
{
    time_t _timestamp;
public:
    LogEntry() = default;

    bool setTimestamp(time_t timestamp)
    {
        _timestamp = timestamp;
        return true;
    }

    time_t getTimestamp()
    {
        return _timestamp;
    }

};


