#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/MyLogger.h"
#include "MockFlashStorage.h"
#include "Macros.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTests
{
    TEST_CLASS(AppendLogItemTests)
    {
    private:
        static constexpr size_t FLASH_SIZE = 1024;
        static constexpr size_t SECTOR_SIZE = 64;
        static constexpr uint32_t VALID_SIGNATURE = 0x16FC69AE;
        static constexpr MyLoggerConfig DEFAULT_CONFIG = { 1, 8 };

        static constexpr uint8_t validData[] = {
            UINT32_TO_BYTES(VALID_SIGNATURE),
            DEFAULT_CONFIG.keySize,
            DEFAULT_CONFIG.valueSize,
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

        TEST_METHOD(Append_MinimalLogItem_ShouldReturnTrue)
        {
            LogEntry entry;
            Assert::IsTrue(entry.setTimestamp(123456));
            Assert::IsTrue(logger.appendLog(entry));
        }


        TEST_METHOD(getFirstLogPosition_ShouldReturnInvalid)
        {
            LogPosition position = logger.getFirstLogPosition();
            Assert::AreEqual(LogPosition::Invalid().getValue(), position.getValue());
        }




        
        //TEST_METHOD(AppendAndReadBack_MinimalLogItem_ShouldMatch)
        //{
        //    time_t testTimestamp = 123456;
        //    LogEntry minimalEntry;
        //    minimalEntry.withTimestamp(testTimestamp);
        //
        //    Assert::IsTrue(logger.appendLog(minimalEntry));
        //
        //    LogPosition pos = logger.getFirstEntryPosition();
        //    LogEntry readBackEntry;
        //
        //
        //    Assert::IsTrue(logger.readLogEntry(pos, readBackEntry));
        //    Assert::AreEqual(testTimestamp, readBackEntry.getTimestamp());
        //
        //}
        








        // TEST_METHOD(Append_MinimalLogItem_ShouldSucceed)
        // TEST_METHOD(Append_LogItemWithMultipleFields_ShouldStoreAll)
        // TEST_METHOD(Append_LogItemWithRepeatedKey_ShouldConcatenateData)
        // TEST_METHOD(Append_LogItem_ExceedsFlashCapacity_ShouldFail)
        // TEST_METHOD(Append_LogItem_WhenPartiallyUsedRingBuffer_ShouldAppendAtEnd)
        // TEST_METHOD(Append_LogItem_TriggersRingBufferWrap_ShouldStoreCorrectly)
        // TEST_METHOD(Append_LogItem_InvalidOrCorruptEntry_ShouldFailOrIgnore)
        // TEST_METHOD(Append_LogItemAndReadBack_ShouldReturnExactData)
        // TEST_METHOD(Append_LogItem_MultipleConsecutiveEntries_ShouldIncrementWriteIndexCorrectly)

    };
}
