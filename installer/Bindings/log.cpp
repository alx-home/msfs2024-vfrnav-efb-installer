#include "Main.h"

#include <iostream>

Promise<>
Main::log(std::string value) {
   std::cout << value << std::endl;
   co_return;
}