#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/MyLogger.h"
#include "MockFlashStorage.h"
#include "Macros.h"

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
        static constexpr MyLogger::Config CONFIG = { 2, 5 };

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
}

