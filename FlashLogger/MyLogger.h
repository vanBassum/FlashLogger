#pragma once
#include "../FlashLogger/IFlashStorage.h"
#include <cstdio> // For printf


class MyLogger {
private:
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

    bool writeHeaderSignature()
    {
        uint32_t signature = 0xAE69FC16;
        return _flash->write(0, &signature, 4);
    }

public:
    MyLogger() = default; // Default constructor

    bool init(IFlashStorage& flash) {
        _flash = &flash;


        return false;
    }

    bool formatAndInit(IFlashStorage& flash) {
        _flash = &flash;

        if (!clearFlash()) {
            return false;
        }

        if (!writeHeaderSignature()) {
            return false;
        }

        return true;
    }


    // Additional logger methods can be added here
};
