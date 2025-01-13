#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/MyLogger.h"
#include "MockFlashStorage.h"
#include "Macros.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTests
{
    TEST_CLASS(LoadedFlashTests)
    {
    private:
        static constexpr size_t FLASH_SIZE = 1024;
        static constexpr size_t SECTOR_SIZE = 64; 
        static constexpr uint32_t SIGNATURE = 0x16FC69AE;
        static constexpr size_t HEADER_SIZE = 8;
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
}

