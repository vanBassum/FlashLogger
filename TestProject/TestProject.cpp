#include "pch.h"
#include "CppUnitTest.h"
#include "../FlashLogger/MyLogger.h"
#include "MockFlashStorage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTests
{
    TEST_CLASS(MyLogger_FormatAndInit)
    {
    private:


    public:
        MyLogger_FormatAndInit() {

        }

        TEST_METHOD(InitWithUnformattedFlash_ShouldFail)
        {
            MockFlashStorage flashStorage;
            MyLogger logger;

            Assert::IsTrue(flashStorage.init(1024, 64));
            Assert::IsFalse(logger.init(flashStorage));
        }

        TEST_METHOD(InitWithUninitializedFlash_ShouldFail)
        {
            MockFlashStorage flashStorage;
            MyLogger logger;

            Assert::IsFalse(logger.init(flashStorage));
        }

        TEST_METHOD(FormatAndInit_ShouldPass)
        {
            MockFlashStorage flashStorage;
            MyLogger logger;

            Assert::IsTrue(flashStorage.init(1024, 64));
            Assert::IsTrue(logger.formatAndInit(flashStorage));
        }

        TEST_METHOD(FormatAndInitWithUninitializedFlash_ShouldFail)
        {
            MockFlashStorage flashStorage;
            MyLogger logger;

            Assert::IsFalse(logger.formatAndInit(flashStorage));

            // In order to make this pass, we have to implement
        }


        // Next test case, initialize from loaded flash

    };
}
