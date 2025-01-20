#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/MyLogger.h"
#include "MockFlashStorage.h"
#include "Macros.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTests
{
    TEST_CLASS(ConfigurationTests)
    {
    private:
        static constexpr size_t FLASH_SIZE = 1024;
        static constexpr size_t SECTOR_SIZE = 64;
        static constexpr uint32_t VALID_SIGNATURE = 0x16FC69AE;
        static constexpr MyLogger::Config DEFAULT_CONFIG = { 1, 8 };
        static constexpr MyLogger::Config NEW_CONFIG = { 2, 5 };

        // Test data for various scenarios
        static constexpr uint8_t validData[] = {
            UINT32_TO_BYTES(VALID_SIGNATURE),
            DEFAULT_CONFIG.keySize,
            DEFAULT_CONFIG.valueSize,
            0xAA, 0xBB, 0xCC, 0xDD      // some random data
        };

        MockFlashStorage flashStorage;
        MyLogger logger;


    public:

        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            Assert::IsTrue(flashStorage.init(FLASH_SIZE, SECTOR_SIZE));
            Assert::IsTrue(flashStorage.eraseAll());
            Assert::IsTrue(flashStorage.write(0, validData, sizeof(validData)));
            Assert::IsTrue(logger.attachStorage(&flashStorage));
            Assert::IsTrue(logger.init());
        }


        TEST_METHOD(GetConfig_Initialized_ShouldReturnTrue)
        {
            MyLogger::Config config;
            Assert::IsTrue(logger.getConfig(&config));
        }

        TEST_METHOD(GetConfig_AfterFormat_ShouldReturnFalse)
        {
            MyLogger::Config config;
            Assert::IsTrue(logger.getConfig(&config));
            Assert::IsTrue(logger.format(config));
            Assert::IsFalse(logger.getConfig(&config));
        }


        TEST_METHOD(GetConfig_AfterValidInit_ShouldMatchFlashData)
        {
            MyLogger::Config config;
            Assert::IsTrue(logger.getConfig(&config));
            Assert::AreEqual(DEFAULT_CONFIG.keySize, config.keySize);
            Assert::AreEqual(DEFAULT_CONFIG.valueSize, config.valueSize);
        }

        TEST_METHOD(Format_ChangesKeyAndValueSizes_ShouldReflectAfterInit)
        {
            MyLogger::Config config;
            Assert::IsTrue(logger.format(NEW_CONFIG));
            Assert::IsTrue(logger.init());
            Assert::IsTrue(logger.getConfig(&config));

            Assert::AreEqual(NEW_CONFIG.keySize, config.keySize);
            Assert::AreEqual(NEW_CONFIG.valueSize, config.valueSize);
        }

    };
}
