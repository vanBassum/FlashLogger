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

    struct Header
    {
        uint32_t signature = 0;
    };


    bool writeHeader(const Header* header)
    {
        if (!_flash)
            return false;

        if (!_flash->write(0, header, sizeof(Header)))
            return false;

        return true;
    }

    bool readHeader(Header* header)
    {
        if (!_flash)
            return false;

        if (!_flash->read(0, header, sizeof(Header)))
            return false;

        return true;
    }

    bool verifyHeaderSignature(Header* header)
    {
        return header->signature == SIGNATURE;
    }

public:

    MyLogger() = default; // Default constructor

    bool init() {
        Header header;

        if (!_flash)
            return false;

        if (!readHeader(&header))
            return false;

        if (!verifyHeaderSignature(&header))
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
        Header header;

        if (!_flash)
            return false;

        if (!clearFlash())
            return false;

        header.signature = SIGNATURE;

        if (!writeHeader(&header))
            return false;

        return true;
    }
};
