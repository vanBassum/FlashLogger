#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/LogEntry.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LogEntryTests
{
    TEST_CLASS(LogEntryTests)
    {
    private:


    public:


        TEST_METHOD(SetTimestamp_ShouldReturnTrue)
        {
            LogEntry entry;
            Assert::IsTrue(entry.setTimestamp(123456));
        }

        TEST_METHOD(SetTimestamp_GetTimestamp_ShouldMatch)
        {
            LogEntry entry;
            time_t testTimestamp = 123456;
            Assert::IsTrue(entry.setTimestamp(testTimestamp));
            time_t readTimestamp = entry.getTimestamp();
            Assert::AreEqual(testTimestamp, readTimestamp);

        }

    };
}
