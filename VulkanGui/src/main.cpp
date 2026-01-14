#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "HelloTrangle.h"

int main()
{
    HelloTrangle app;

    try {
        app.Run();

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}