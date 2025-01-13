#pragma once
#include "../FlashLogger/IFlashStorage.h"
#include <cstdio> // For printf


class MyLogger {
private:
    constexpr static const uint32_t SIGNATURE = 0x16FC69AE;
    IFlashStorage* _flash = nullptr;

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
        uint32_t signature;
        if (!_flash->read(0, &signature, 4)) {
            return false;
        }

        if (signature != SIGNATURE) {
            return false;
        }


        return true;
    }

    bool writeHeader()
    {
        return _flash->write(0, &SIGNATURE, 4);
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

    bool formatAndInit(IFlashStorage& flash) {
        _flash = &flash;

        if (!clearFlash()) {
            return false;
        }

        if (!writeHeader()) {
            return false;
        }

        return true;
    }


    // Additional logger methods can be added here
};
