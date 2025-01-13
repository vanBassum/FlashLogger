#pragma once
#include "../FlashLogger/IFlashStorage.h"
#include <cstdio> // For printf

class MyLogger {
private:
    const IFlashStorage* _flash = nullptr;

public:
    MyLogger() = default; // Default constructor

    bool init(const IFlashStorage& flash) {
        _flash = &flash;


        return false;
    }

    bool formatAndInit(const IFlashStorage& flash) {
        _flash = &flash;

        return true;
    }


    // Additional logger methods can be added here
};
