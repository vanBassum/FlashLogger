#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/MyLogger.h"
#include "MockFlashStorage.h"
#include "Macros.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTests
{

    TEST_CLASS(FormattingTests)
    {
    private:
        static constexpr size_t FLASH_SIZE = 1024;
        static constexpr size_t SECTOR_SIZE = 64;
        static constexpr uint32_t SIGNATURE = 0x16FC69AE;
        static constexpr size_t HEADER_SIZE = 8;
        static constexpr MyLogger::Config CONFIG = { 2, 5 };

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

        TEST_METHOD(FormatAndInit_ShouldWriteCorrectValueSize)
        {
            uint8_t valueSize;
            Assert::IsTrue(flashStorage.read(5, &valueSize, sizeof(valueSize)));
            Assert::AreEqual(CONFIG.valueSize, valueSize);
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
