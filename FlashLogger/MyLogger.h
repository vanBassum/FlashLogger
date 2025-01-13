#pragma once
#include "../FlashLogger/IFlashStorage.h"
#include <cstdio> // For printf




class MyLogger {
public:
    struct Config
    {
        uint8_t keySize = 0;
    };

    struct Header
    {
        uint32_t signature = 0;
        Config config;
        uint8_t padding[3] = { 0xFF, 0xFF, 0xFF };
    };

private:
    constexpr static const uint32_t SIGNATURE = 0x16FC69AE;
    IFlashStorage* _flash = nullptr;
    Header _header;



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

    bool readHeader()
    {
        if (!_flash->read(0, &_header, sizeof(_header))) {
            return false;
        }

        if (_header.signature != SIGNATURE) {
            return false;
        }

        return true;
    }

    bool writeHeader()
    {
        return _flash->write(0, &_header, sizeof(_header));
    }

public:

    MyLogger() = default; // Default constructor

    bool init(IFlashStorage& flash) {
        _flash = &flash;

        if (!readHeader()) {
            return false;
        }

        return true;
    }

    bool formatAndInit(IFlashStorage& flash, const Config& config) {
        _flash = &flash;
        _header.signature = SIGNATURE;
        _header.config = config;

        if (!clearFlash()) {
            return false;
        }

        if (!writeHeader()) {
            return false;
        }

        return true;
    }

    bool getConfig(Config& config)
    {
        config.keySize = 2;
        return true;
    }

};
