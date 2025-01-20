#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/MyLogger.h"
#include "MockFlashStorage.h"
#include "Macros.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTests
{
    TEST_CLASS(FormatTests)
    {
    private:
        static constexpr size_t FLASH_SIZE = 1024;
        static constexpr size_t SECTOR_SIZE = 64;
        static constexpr uint32_t VALID_SIGNATURE = 0x16FC69AE;
        static constexpr uint32_t INVALID_SIGNATURE = 0x15613843;

        // Test data for various scenarios
        static constexpr uint8_t validData[] = {
            UINT32_TO_BYTES(VALID_SIGNATURE),
            0xAA, 0xBB, 0xCC, 0xDD   // some random data
        };

        static constexpr uint8_t invalidData[] = {
            UINT32_TO_BYTES(INVALID_SIGNATURE),
            0xAA, 0xBB, 0xCC, 0xDD   // some random data
        };

        MockFlashStorage flashStorage;
        MyLogger logger;

    public:
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            Assert::IsTrue(flashStorage.init(FLASH_SIZE, SECTOR_SIZE));
        }

        TEST_METHOD(Format_Unattached_ShouldReturnFalse)
        {
            Assert::IsFalse(logger.format());
        }

        TEST_METHOD(Format_EmptyFlash_ShouldReturnTrue)
        {
            Assert::IsTrue(flashStorage.eraseAll());
            Assert::IsTrue(logger.attachStorage(&flashStorage));
            Assert::IsTrue(logger.format());
        }

        TEST_METHOD(FormatAndInit_EmptyFlash_ShouldReturnTrue)
        {
            Assert::IsTrue(flashStorage.eraseAll());
            Assert::IsTrue(logger.attachStorage(&flashStorage));
            Assert::IsTrue(logger.format());
            Assert::IsTrue(logger.init());
        }

        TEST_METHOD(FormatAndInit_ValidFlash_ShouldReturnTrue)
        {
            Assert::IsTrue(flashStorage.eraseAll());
            Assert::IsTrue(flashStorage.write(0, validData, sizeof(validData)));
            Assert::IsTrue(logger.attachStorage(&flashStorage));
            Assert::IsTrue(logger.format());
            Assert::IsTrue(logger.init());
        }

        TEST_METHOD(FormatAndInit_InvalidFlash_ShouldReturnTrue)
        {
            Assert::IsTrue(flashStorage.eraseAll());
            Assert::IsTrue(flashStorage.write(0, invalidData, sizeof(invalidData)));
            Assert::IsTrue(logger.attachStorage(&flashStorage));
            Assert::IsTrue(logger.format());
            Assert::IsTrue(logger.init());
        }

        TEST_METHOD(Format_AfterInit_ShouldBeAbleToInitAgain)
        {
            Assert::IsTrue(flashStorage.eraseAll());
            Assert::IsTrue(flashStorage.write(0, validData, sizeof(validData)));
            Assert::IsTrue(logger.attachStorage(&flashStorage));
            Assert::IsTrue(logger.init());

            // Formatting should reset the logger and allow re-initialization
            Assert::IsTrue(logger.format());
            Assert::IsTrue(logger.init());
        }
    };
}
