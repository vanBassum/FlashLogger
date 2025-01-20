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

public:

    MyLogger() = default; // Default constructor

    bool init() {

        if (!_flash)
            return false;

        uint32_t signature;
        if (!_flash->read(0, &signature, 4))
            return false;

        if (signature != SIGNATURE)
            return false;

        return true;
    }

    bool attachStorage(IFlashStorage* flash)
    {
        if (_flash)
            return false;

        _flash = flash;
        return true;
    }

    bool format()
    {
        if (!_flash)
            return false;

        if (!clearFlash())
            return false;

        if (!_flash->write(0, &SIGNATURE, 4))
            return false;

        return true;
    }

};
