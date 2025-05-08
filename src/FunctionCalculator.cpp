#include "FunctionCalculator.h"
#include "SquareMatrix.h"
#include "Add.h"
#include "Sub.h"
#include "Comp.h"
#include "Identity.h"
#include "Transpose.h"
#include "Scalar.h"
#include "InputException.h"
#include "ReadFile.h"

#include <iostream>
#include <algorithm>



const auto MAX_MAT_SIZE = 5;

FunctionCalculator::FunctionCalculator(std::istream& istr, std::ostream& ostr)
    : m_actions(createActions()), m_operations(createOperations()), m_istr(istr), m_ostr(ostr) {}


void FunctionCalculator::run()
{

	// ask the user number of operations to be performed

    do
    {
        m_ostr << '\n';
        printOperations();
        m_ostr << "Enter command ('help' for the list of available commands): ";
        readLine();
        try {
            const auto action = readAction();
            runAction(action);
        }
        catch (const InputException& e)
        {
            // if mode file = true 
			// ask user if to continue next row or not
			// if yes -> continue and ignore this line.
			// else mode file = false . close file .
            m_ostr << e.what();
        }
    } while (m_running);
}


void FunctionCalculator::eval()
{
    if (auto index = readOperationIndex(); index)
    {
        const auto& operation = m_operations[*index];
		int inputCount = operation->inputCount();
        int size = 0;
        //m_istr >> size;
        m_iss >> size;

        // Throws an exception if the entered matrix size exceeds the allowed maximum (5)
        if (size > MAX_MAT_SIZE)
        {
            throw InputException("The entered matrix size is larger than MAX_MAT_SIZE (5)");
        }

        
        // Throws an exception if too many arguments were provided for the command
      /*  auto str = std::string();
        m_istr >> str;
        if ( str != "")
        {
            throw InputException("Too many arguments for this command");
        }*/

        if (hasNonWhitespace())
            throw InputException("Too many arguments for this command");


		auto matrixVec = std::vector<Operation::T>();
        if (inputCount > 1)
            m_ostr << "\nPlease enter " << inputCount << " matrices:\n";

		for (int i = 0; i < inputCount; ++i)
		{
            auto input = Operation::T(size); // Operation::T == SquareMatrix<int>
            m_ostr << "\nEnter a " << size << "x" << size << " matrix:\n";
            m_istr >> input;
			matrixVec.push_back(input);

		}
        m_ostr << "\n";
        operation->print(m_ostr, matrixVec);
        m_ostr << " = \n" << operation->compute(matrixVec);
    }
}


void FunctionCalculator::del()
{
	// update the number of operations are leagelly -- ??? 
    if (auto i = readOperationIndex(); i)
    {
        m_operations.erase(m_operations.begin() + *i);
    }
}


void FunctionCalculator::help()
{
    m_ostr << "The available commands are:\n";
    for (const auto& action : m_actions)
    {
        m_ostr << "* " << action.command << action.description << '\n';
    }
    m_ostr << '\n';
}


void FunctionCalculator::exit()
{
    m_ostr << "Goodbye!\n";
    m_running = false;
}

void FunctionCalculator::read()
{
    std::string file_path = "new1.txt";
    ReadFile file1(file_path);

    std::string line;
    while (file1.getline(line))
    {
		// read the line from the file and send the string to readAction function.
		// readAction();


    }
}

void FunctionCalculator::printOperations() const
{
	// print number of operations are leagelly
    m_ostr << "List of available matrix operations:\n";
    for (decltype(m_operations.size()) i = 0; i < m_operations.size(); ++i)
    {
        m_ostr << i << ". ";
        m_operations[i]->print(m_ostr,true);
        m_ostr << '\n';
    }
    m_ostr << '\n';
}


std::optional<int> FunctionCalculator::readOperationIndex() 
{
    int i = 0;
    //m_istr >> i;
    m_iss >> i;

    // if the read operation failed (e.g. characters were entered instead of a number)
    if (m_iss.fail())
    {
        m_iss.clear();
        m_iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InputException("must enter numbers, not characters.");
    }

    // if i out of range the vector operation : -> throw
    if (i >= static_cast<int>(m_operations.size()) || i<0)
    {
        throw InputException("out of range the vector operation");
    }

    return i;
}


FunctionCalculator::Action FunctionCalculator::readAction() 
{

    /*The code should be modified as follows:
The user will input data into m_istr.
This input will be inserted into std::istringstream m_iss.
The readAction function should then read the command from the istringstream.

We will also support reading from a file:
The string received from the function ReadFile::getline(std::string& outLine) will be inserted into the istringstream, and from there, the code will call the readAction function.

-- The readAction function will likely no longer be const.*/
    //auto line = std::string();
    //std::getline(m_istr, line);
    
    auto action = std::string();
    //m_istr >> action;
    m_iss >> action;

    const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);

   // If a number was entered outside the range of the operation vector
    if (i == m_actions.end())
	{
		throw InputException("number outside the range of the operation vector\n");
	}
    
   
    return i->action;
}


void FunctionCalculator::runAction(Action action)
{
    switch (action)
    {
        default:
            m_ostr << "Unknown enum entry used!\n";
            break;

        case Action::Invalid:
            m_ostr << "Command not found\n";
            break;

        case Action::Eval:     eval();                      break;
        case Action::Add:      binaryFunc<Add>();           break;
        case Action::Sub:      binaryFunc<Sub>();           break;
        case Action::Comp:     binaryFunc<Comp>();          break;
        case Action::Del:      del();                       break;
        case Action::Help:     help();                      break;
        case Action::Exit:     exit();                      break;
        case Action::Iden:     unaryFunc<Identity>();       break;
        case Action::Tran:     unaryFunc<Transpose>();      break;
        case Action::Scal:     unaryWithIntFunc<Scalar>();  break;
        case Action::Read:     read();                      break;
            // reaize
    }
}


FunctionCalculator::ActionMap FunctionCalculator::createActions() const
{
    return ActionMap
    {
        {
            "eval",
            "(uate) num n - compute the result of function #num on an n×n matrix "
			"(that will be prompted)",
            Action::Eval
        },
        {
            "scal",
            "(ar) val - creates an operation that multiplies the "
			"given matrix by scalar val",
            Action::Scal
        },
        {
            "add",
            " num1 num2 - creates an operation that is the addition of the result of operation #num1 "
			"and the result of operation #num2",
            Action::Add
        },
         {
            "sub",
            " num1 num2 - creates an operation that is the subtraction of the result of operation #num1 "
			"and the result of operation #num2",
            Action::Sub
        },
        {
            "comp",
            "(osite) num1 num2 - creates an operation that is the composition of operation #num1 "
			"and operation #num2",
            Action::Comp
        },
        {
            "del",
            "(ete) num - delete operation #num from the operation list",
            Action::Del
        },
        {
            "help",
            " - print this command list",
            Action::Help
        },
        {
            "exit",
            " - exit the program",
            Action::Exit
        }
    };
}


FunctionCalculator::OperationList FunctionCalculator::createOperations() const
{
    return OperationList
    {
        std::make_shared<Identity>(),
        std::make_shared<Transpose>(),
    };
}

void FunctionCalculator::readLine()
{
	//if mode read from file -> getline(file , line) // if EOF change mode to console // close file
	//else getline(std::cin, line);
    std::getline(m_istr, m_line);
    m_iss.str(m_line);
}

bool FunctionCalculator::hasNonWhitespace()
{
    char ch;
    while (m_iss >> std::noskipws >> ch) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            return true;
        }
    }
    return false;
}
