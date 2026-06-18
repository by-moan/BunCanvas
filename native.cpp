#include <iostream>
#include <string>

namespace internals {
    void PrintTest(std::string args) {
        std::cout << args << "\n";
    }
}

extern "C" {
    void PrintTest(const char* args) {
        internals::PrintTest(std::string(args));
    }
}
