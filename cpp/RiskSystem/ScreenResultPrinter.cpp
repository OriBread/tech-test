#include "ScreenResultPrinter.h"
#include <iostream>

void ScreenResultPrinter::printResults(ScalarResults& results) {
    for (const auto& result : results) {
        std::string id = result.getTradeId();
        auto optResult = result.getResult();
        auto optError = result.getError();

        if (optResult.has_value() && optError.has_value()) {
            std::cout << id << " : " << optResult.value() << " : " << optError.value() << "\n";
        }
        else if (optResult.has_value()) {
            std::cout << id << " : " << optResult.value() << "\n";
        }
        else if (optError.has_value()) {
            std::cout << id << " : " << optError.value() << "\n";
        }
    }
}
