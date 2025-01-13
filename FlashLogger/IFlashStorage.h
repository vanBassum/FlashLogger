#pragma once
#include <cstdint>

class IFlashStorage {
public:
    virtual ~IFlashStorage() = default;

    virtual bool read(uint32_t address, void* buffer, size_t length) = 0;
    virtual bool write(uint32_t address, const void* buffer, size_t length) = 0;
    virtual bool eraseSector(uint32_t address) = 0;

    // Gets the total size of the flash storage in bytes.
    virtual size_t getTotalSize() const = 0;
    virtual size_t getSectorSize() const = 0;
    virtual size_t getNumSectors() const = 0;
};
