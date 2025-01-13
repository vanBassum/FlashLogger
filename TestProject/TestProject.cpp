#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/MyLogger.h"
#include "MockFlashStorage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTests
{

    TEST_CLASS(EmptyFlashTests)
    {
    private:
        static constexpr uint32_t SIGNATURE = 0x16FC69AE;
        static constexpr size_t HEADER_SIZE = 4;
        static constexpr uint8_t CORRUPTED_HEADER[4] = { 0x12, 0x24, 0x56, 0x78 };
        static constexpr uint8_t PARTIAL_HEADER[2] = { 0xAE, 0x69 };
        static constexpr size_t FLASH_SIZE = 1024;  // Total flash size in bytes
        static constexpr size_t SECTOR_SIZE = 64;   // Size of a sector in bytes

        MockFlashStorage flashStorage;
        MyLogger logger;

    public:
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            Assert::IsTrue(flashStorage.init(FLASH_SIZE, SECTOR_SIZE));
            Assert::IsTrue(flashStorage.eraseAll());
        }

        TEST_METHOD(Init_UnformattedFlash_ShouldFail)
        {
            Assert::IsFalse(logger.init(flashStorage));
        }

        TEST_METHOD(Init_CorruptedHeader_ShouldFail)
        {
            Assert::IsTrue(flashStorage.write(0, CORRUPTED_HEADER, sizeof(CORRUPTED_HEADER)));
            Assert::IsFalse(logger.init(flashStorage));
        }

        TEST_METHOD(Init_PartialHeader_ShouldFail)
        {
            Assert::IsTrue(flashStorage.write(0, PARTIAL_HEADER, sizeof(PARTIAL_HEADER)));
            Assert::IsFalse(logger.init(flashStorage));
        }

        TEST_METHOD(FormatAndInit_ShouldWriteSignature)
        {
            Assert::IsTrue(logger.formatAndInit(flashStorage));

            uint32_t signature;
            Assert::IsTrue(flashStorage.read(0, &signature, sizeof(signature)));
            Assert::AreEqual(signature, SIGNATURE);
        }
    };


    TEST_CLASS(LoadedFlashTests)
    {
    private:
        static constexpr uint32_t SIGNATURE = 0x16FC69AE;
        static constexpr size_t HEADER_SIZE = 4;
        static constexpr uint8_t primeData[1024] = { 0xAE, 0x69, 0xFC, 0x16, 0xAA, 0xBB, 0xCC, 0xDD }; // Added some extra data to simulate data after the header
        static constexpr size_t FLASH_SIZE = 1024;  // Total flash size in bytes
        static constexpr size_t SECTOR_SIZE = 64;   // Size of a sector in bytes

        MockFlashStorage flashStorage;
        MyLogger logger;

    public:
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            Assert::IsTrue(flashStorage.init(FLASH_SIZE, SECTOR_SIZE));
            Assert::IsTrue(flashStorage.eraseAll());
            Assert::IsTrue(flashStorage.write(0, primeData, sizeof(primeData)));
        }

        TEST_METHOD(Init_ValidHeader_ShouldPass)
        {
            Assert::IsTrue(logger.init(flashStorage));
        }

        TEST_METHOD(FormatAndInit_ShouldClearDataAfterHeader)
        {
            Assert::IsTrue(logger.formatAndInit(flashStorage));

            // Verify the header is written correctly
            uint32_t signature;
            Assert::IsTrue(flashStorage.read(0, &signature, sizeof(signature)));
            Assert::AreEqual(signature, SIGNATURE); // Check the signature in the header

            // Verify that the data after the header is cleared
            uint8_t readData[FLASH_SIZE] = { 0 }; // Initialize with zeros
            Assert::IsTrue(flashStorage.read(0, readData, sizeof(readData))); // Read after the header
            for (size_t i = HEADER_SIZE; i < sizeof(readData); ++i)
            {
                Assert::AreEqual(readData[i], (uint8_t)0xff);
            }
        }
    };
}


// Boundary Tests for Flash Storage: Assert::IsTrue(smallFlashStorage.init(4, 1)); // Very small flash size
// Flash Storage Write Failures (crc?)
