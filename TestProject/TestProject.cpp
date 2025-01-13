#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/MyLogger.h"
#include "MockFlashStorage.h"

#define UINT32_TO_BYTES(value) \
    (static_cast<uint8_t>((value >> 0) & 0xFF)), \
    (static_cast<uint8_t>((value >> 8) & 0xFF)), \
    (static_cast<uint8_t>((value >> 16) & 0xFF)), \
    (static_cast<uint8_t>((value >> 24) & 0xFF))

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTests
{

    TEST_CLASS(EmptyFlashTests)
    {
    private:
        static constexpr size_t FLASH_SIZE = 1024; 
        static constexpr size_t SECTOR_SIZE = 64;   
        static constexpr uint32_t SIGNATURE = 0x16FC69AE;
        static constexpr uint8_t CORRUPTED_HEADER[4] = { 0x12, 0x24, 0x56, 0x78 };
        static constexpr uint8_t PARTIAL_HEADER[2] = { 0xAE, 0x69 };
        static constexpr MyLogger::Config CONFIG = { 2 };

        MockFlashStorage flashStorage;
        MyLogger logger;

    public:
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            Assert::IsTrue(flashStorage.init(FLASH_SIZE, SECTOR_SIZE));
            Assert::IsTrue(flashStorage.eraseAll());
        }

        TEST_METHOD(Init_ShouldReturnFalse)
        {
            Assert::IsFalse(logger.init(flashStorage));
        }

        TEST_METHOD(Init_WithCorruptedHeader_ShouldReturnFalse)
        {
            Assert::IsTrue(flashStorage.write(0, CORRUPTED_HEADER, sizeof(CORRUPTED_HEADER)));
            Assert::IsFalse(logger.init(flashStorage));
        }

        TEST_METHOD(Init_WithPartialHeader_ShouldReturnFalse)
        {
            Assert::IsTrue(flashStorage.write(0, PARTIAL_HEADER, sizeof(PARTIAL_HEADER)));
            Assert::IsFalse(logger.init(flashStorage));
        }

        TEST_METHOD(FormatAndInit_ShouldWriteCorrectSignature)
        {
            Assert::IsTrue(logger.formatAndInit(flashStorage, CONFIG));

            uint32_t signature;
            Assert::IsTrue(flashStorage.read(0, &signature, sizeof(signature)));
            Assert::AreEqual(SIGNATURE, signature);
        }
    };


    TEST_CLASS(LoadedFlashTests)
    {
    private:
        static constexpr size_t FLASH_SIZE = 1024;
        static constexpr size_t SECTOR_SIZE = 64; 
        static constexpr uint32_t SIGNATURE = 0x16FC69AE;
        static constexpr size_t HEADER_SIZE = sizeof(MyLogger::Header);
        static constexpr MyLogger::Config CONFIG = { 2 };

        static constexpr uint8_t primeData[FLASH_SIZE] =
        {
            UINT32_TO_BYTES(SIGNATURE),
            CONFIG.keySize,
            0, 0, 0,                    // reserved
            0xAA, 0xBB, 0xCC, 0xDD      // some random data
        };

        MockFlashStorage flashStorage;
        MyLogger logger;

    public:
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            Assert::IsTrue(flashStorage.init(FLASH_SIZE, SECTOR_SIZE));
            Assert::IsTrue(flashStorage.eraseAll());
            Assert::IsTrue(flashStorage.write(0, primeData, sizeof(primeData)));
        }

        TEST_METHOD(Init_ShouldReturnTrue)
        {
            Assert::IsTrue(logger.init(flashStorage));
        }

        TEST_METHOD(Init_ShouldLoadCorrectKeySize)
        {
            MyLogger::Config loadedConfig;
            Assert::IsTrue(logger.init(flashStorage));
            Assert::IsTrue(logger.getConfig(loadedConfig));
            Assert::AreEqual(CONFIG.keySize, loadedConfig.keySize);
        }



        TEST_METHOD(FormatAndInit_ShouldClearDataBeyondHeader)
        {
            MyLogger::Config newConfig = { 5 }; 
            Assert::IsTrue(logger.formatAndInit(flashStorage, newConfig));

            // Verify the header is written correctly
            uint32_t signature;
            Assert::IsTrue(flashStorage.read(0, &signature, sizeof(signature)));
            Assert::AreEqual(SIGNATURE, signature);

            // Verify that the data after the header is cleared
            uint8_t readData[FLASH_SIZE] = { 0 }; // Initialize with zeros
            Assert::IsTrue(flashStorage.read(0, readData, sizeof(readData))); // Read after the header
            for (size_t i = HEADER_SIZE; i < sizeof(readData); ++i)
            {
                Assert::AreEqual((uint8_t)0xff, readData[i]);
            }
        }
    };

    TEST_CLASS(FormattingTests)
    {
    private:
        static constexpr size_t FLASH_SIZE = 1024;
        static constexpr size_t SECTOR_SIZE = 64;
        static constexpr uint32_t SIGNATURE = 0x16FC69AE;
        static constexpr size_t HEADER_SIZE = sizeof(MyLogger::Header);
        static constexpr MyLogger::Config CONFIG = { 2 };

        MockFlashStorage flashStorage;
        MyLogger logger;

    public:
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            Assert::IsTrue(flashStorage.init(FLASH_SIZE, SECTOR_SIZE));
            Assert::IsTrue(flashStorage.eraseAll());
            Assert::IsTrue(logger.formatAndInit(flashStorage, CONFIG));
        }

        TEST_METHOD(FormatAndInit_ShouldWriteCorrectSignature)
        {
            uint32_t signature;
            Assert::IsTrue(flashStorage.read(0, &signature, sizeof(signature)));
            Assert::AreEqual(SIGNATURE, signature);
        }

        TEST_METHOD(FormatAndInit_ShouldWriteCorrectKeySize)
        {
            uint8_t keySize;
            Assert::IsTrue(flashStorage.read(4, &keySize, sizeof(keySize)));
            Assert::AreEqual(CONFIG.keySize, keySize);
        }

        TEST_METHOD(FormatAndInit_ShouldClearDataAfterHeader)
        {
            // Verify that the data after the header is cleared
            uint8_t readData[FLASH_SIZE] = { 0 }; // Initialize with zeros
            Assert::IsTrue(flashStorage.read(0, readData, sizeof(readData))); // Read after the header
            for (size_t i = HEADER_SIZE; i < sizeof(readData); ++i)
            {
                Assert::AreEqual((uint8_t)0xff, readData[i]);
            }
        }
    };
}





// Boundary Tests for Flash Storage: Assert::IsTrue(smallFlashStorage.init(4, 1)); // Very small flash size
// Flash Storage Write Failures (crc?)
