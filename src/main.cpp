#include "FunctionCalculator.h"

#include <string>
#include <iostream>



int main()
{
  
    
	try
	{
		FunctionCalculator(std::cout).run();
	}
	catch (...)
	{
		std::cout << "\n ERROR";
	}
   

}
