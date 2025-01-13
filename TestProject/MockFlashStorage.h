#pragma once
#include <cstdint>
#include <cstring> // For std::memcpy and std::memset
#include <array>
#include <cstdio> // For printf
#include "../FlashLogger/IFlashStorage.h"

class MockFlashStorage : public IFlashStorage {
private:
    static constexpr size_t MaxTotalSize = 1024 * 1024; // Example: 1 MiB max size
    uint8_t storage[MaxTotalSize]{};  // Statically allocated memory for flash storage
    size_t totalSize = 0;
    size_t sectorSize = 0;
    bool initialized = false;

public:
    MockFlashStorage() = default; // Default constructor

    bool init(size_t totalSize, size_t sectorSize) {
        if (totalSize > MaxTotalSize) {
            printf("Error: Total size exceeds max limit\n");
            return false;
        }
        if (totalSize % sectorSize != 0) {
            printf("Error: Total size must be a multiple of sector size\n");
            return false;
        }

        this->totalSize = totalSize;
        this->sectorSize = sectorSize;

        // Optionally, clear the storage to simulate freshly erased flash
        //std::memset(storage, 0xFF, MaxTotalSize);

        initialized = true;
        return true;
    }

    bool read(uint32_t address, void* buffer, size_t length) override {
        if (!initialized)
        {
            printf("Error: Not initialized\n");
            return false;
        }
        if (address + length > totalSize) {
            printf("Error: Read out of bounds\n");
            return false;
        }
        if (buffer == nullptr) {
            printf("Error: Buffer is null\n");
            return false;
        }

        std::memcpy(buffer, &storage[address], length);
        return true;
    }

    bool write(uint32_t address, const void* buffer, size_t length) override {
        if (!initialized)
        {
            printf("Error: Not initialized\n");
            return false;
        }
        if (address + length > totalSize) {
            printf("Error: Write out of bounds\n");
            return false;
        }
        if (buffer == nullptr) {
            printf("Error: Buffer is null\n");
            return false;
        }

        // Flash memory only allows changing bits from 1 to 0
        for (size_t i = 0; i < length; ++i) {
            if ((storage[address + i] & ((const uint8_t*)buffer)[i]) != ((const uint8_t*)buffer)[i]) {
                printf("Error: Writing requires flipping bits from 1 to 0\n");
                return false;
            }
        }

        std::memcpy(&storage[address], buffer, length);
        return true;
    }

    bool eraseSector(uint32_t address) override {
        if (!initialized)
        {
            printf("Error: Not initialized\n");
            return false;
        }
        if (address >= totalSize || address % sectorSize != 0) {
            printf("Error: Erase sector out of bounds or not aligned\n");
            return false;
        }

        // Erase by setting all bits in the sector to 1 (0xFF)
        std::memset(&storage[address], 0xFF, sectorSize);
        return true;
    }

    size_t getTotalSize() const override {
        return totalSize;
    }

    size_t getSectorSize() const override {
        return sectorSize;
    }

    size_t getNumSectors() const override {
        return totalSize / sectorSize;
    }
};
