#include "FunctionCalculator.h"

#include <string>
#include <iostream>

#include "ReadFile.h"

int main()
{
    std::string str = "new1.txt";
    ReadFile file1(str);

    while(file1.getline(str))
    std::cout << str;

    FunctionCalculator(std::cin, std::cout).run();
   

}
