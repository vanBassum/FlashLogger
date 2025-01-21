#pragma once
#include "../FlashLogger/IFlashStorage.h"
#include <cstdio> // For printf
#include "LogEntry.h"

struct MyLoggerConfig
{
    uint8_t keySize = 0;
    uint8_t valueSize = 0;
};


struct MyLoggerHeader
{
    uint32_t signature = 0;
    MyLoggerConfig config;
};

class LogPosition {
    int32_t _value = -1;
public:
    int32_t getValue() const { return _value; }
    static LogPosition Invalid() { return LogPosition(); }
};


class MyLogger {
public:

private:
    constexpr static const uint32_t SIGNATURE = 0x16FC69AE;
    constexpr static const uint8_t MAX_KEY_SIZE = 4;
    constexpr static const uint8_t MAX_VALUE_SIZE = 32;

    IFlashStorage* _flash = nullptr;
    bool _initialized = false;
    MyLoggerConfig _config;

    bool clearFlash()
    {
        int sectors = _flash->getNumSectors();
        int sectorSize = _flash->getSectorSize();
        for (int i = 0; i < sectors; i++)
        {
            if (!_flash->eraseSector(i * sectorSize)) {
                return false;
            }
        }

        return true;
    }


    bool writeHeader(const MyLoggerHeader* header)
    {
        if (!_flash)
            return false;

        if (!_flash->write(0, header, sizeof(MyLoggerHeader)))
            return false;

        return true;
    }

    bool readHeader(MyLoggerHeader* header)
    {
        if (!_flash)
            return false;

        if (!_flash->read(0, header, sizeof(MyLoggerHeader)))
            return false;

        return true;
    }

    bool verifyHeaderSignature(MyLoggerHeader* header)
    {
        return header->signature == SIGNATURE;
    }

public:

    MyLogger() = default; // Default constructor

    bool init() {
        MyLoggerHeader header;

        if (!_flash)
            return false;

        if (!readHeader(&header))
            return false;

        if (!verifyHeaderSignature(&header))
            return false;

        memcpy(&_config, &header.config, sizeof(_config));

        _initialized = true;
        return true;
    }

    bool attachStorage(IFlashStorage* flash)
    {
        if (_initialized)
            return false;

        _flash = flash;
        return true;
    }

    bool format(const MyLoggerConfig& config)
    {
        MyLoggerHeader header;

        if (config.keySize == 0 || config.keySize > MAX_KEY_SIZE)
            return false;

        if (config.valueSize == 0 || config.valueSize > MAX_VALUE_SIZE)
            return false;

        if (!_flash)
            return false;

        if (!clearFlash())
            return false;

        header.signature = SIGNATURE;
        memcpy(&header.config, &config, sizeof(_config));
        
        if (!writeHeader(&header))
            return false;

        _initialized = false;

        return true;
    }

    bool getConfig(MyLoggerConfig* config)
    {
        if (!_initialized)
            return false;
        memcpy(config, &_config, sizeof(_config));
        return true;
    }


    bool appendLog(const LogEntry& entry)
    {
        return true;
    }

    LogPosition getFirstLogPosition()
    {
        return LogPosition();
    }
    


    // 
    //bool readLogEntry(const LogPosition& position, LogEntry& entry)
    //{
    //    return false;
    //}

};















