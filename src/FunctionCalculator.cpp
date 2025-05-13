#include "FunctionCalculator.h"
#include "SquareMatrix.h"
#include "Add.h"
#include "Sub.h"
#include "Comp.h"
#include "Identity.h"
#include "Transpose.h"
#include "Scalar.h"
#include "InputException.h"
#include "FileException.h"
//#include "ReadFile.h"
#include <iostream>
#include <algorithm>

FunctionCalculator::FunctionCalculator( std::ostream& ostr)
    : m_actions(createActions()), m_operations(createOperations()), m_ostr(ostr) {}

void FunctionCalculator::run()
{
	updateMaxFunc();
    run(std::cin, false);
}

void FunctionCalculator::run(std::istream& istr, bool fileMode)
{
    auto line = std::string();
    auto iss = std::istringstream();
 //   iss.exceptions(std::ios::failbit | std::ios::badbit);

    printOperations();
    while (m_running && std::getline(istr, line))
    {
        iss.str(line);

        try {
            const auto action = readAction(iss);
            runAction(action, iss , istr);
        }
        catch (const InputException& e)
        {        
			if (!fileMode) // if not in file mode
			{
				m_ostr << "Error: " << e.what() << "\n";
			}
			else
			{
				m_ostr << "this line is invalid: " << line << "\n";
				m_ostr << "Error: " << e.what() << "\n";
				m_ostr << "Do you want to continue reading the file? (y/n): ";
				char choice;
				std::cin >> choice;
				if (choice == 'n' || choice == 'N')
				{
					m_running = false;
				}
			}
        //    m_ostr << "Error: " << e.what() << "\n";
        }

        catch (const FileException& e)
        {
           m_ostr << e.what();
        }
        catch (const std::runtime_error& e)
        {
            m_ostr << "Error: " << e.what() << '\n';
        }

        printOperations();
        iss.clear();
        
        if (istr.fail()) {
            istr.clear();
            istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } 
}

void FunctionCalculator::eval(std::istringstream& iss, std::istream& istr)
{
    if (auto index = readOperationIndex(iss); index)
    {
        const auto& operation = m_operations[*index];
		int inputCount = operation->inputCount();
        int size = 0;
        iss >> size;
		if (iss.fail())
		{
			iss.clear();
			iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			throw InputException("Missing arguments for this command, there is no 'SIZE' argument for this command.");
		}

        if (hasNonWhitespace(iss))
            throw InputException("Too many arguments for this command");

		auto matrixVec = std::vector<Operation::T>();
        if (inputCount > 1)
            m_ostr << "\nPlease enter " << inputCount << " matrices:\n";

		for (int i = 0; i < inputCount; ++i)
		{
            auto input = Operation::T(size); // Operation::T == SquareMatrix<int>
            m_ostr << "\nEnter a " << size << "x" << size << " matrix:\n";
            istr >> input;
			matrixVec.push_back(input);
		}
        istr.clear();
        istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        m_ostr << "\n";
        operation->print(m_ostr, matrixVec);
        m_ostr << " = \n" << operation->compute(matrixVec);
    }
}

void FunctionCalculator::del(std::istringstream& iss)
{
	// update the number of operations are leagelly -- ??? 
    if (auto i = readOperationIndex(iss); i)
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

void FunctionCalculator::read(std::istringstream& iss)
{
    auto file_path = std::string();
	auto file = std::ifstream();
    iss >> file_path;
	file.open(file_path);
    if (!file.is_open()){
		throw FileException("File not found. \n path: " + file_path); // WARNING NOT CATCHING!!!
    }
    run(file, true);
}

void FunctionCalculator::resize(std::istream& istr)
{
    m_ostr << "Enter the new maximum number of operations (between 2 and 100): \n";
	auto newMaxOperation = 0;
	istr >> newMaxOperation;
	if (istr.fail())
	{
		istr.clear();
		istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		throw InputException("Invalid input. Please enter a number.");
	}
	if (newMaxOperation < 2 || newMaxOperation > 100)
	{
		throw InputException("The number of operations must be between 2 and 100.");
	}
	if (newMaxOperation < static_cast<int>(m_operations.size()))
	{
		m_ostr << "The new maximum number of operations is less than the current number of operations.\n";
		m_ostr << "Do you want to delete the excess operations? (y/n): ";
		char choice;
		std::cin >> choice;
		if (choice == 'y' || choice == 'Y')
		{
			m_operations.resize(newMaxOperation);
			m_ostr << "Excess operations deleted.\n";
		}
	}
    istr.clear();
    istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void FunctionCalculator::printOperations() const
{
	// print number of operations are leagelly

    m_ostr << " \n List of available matrix operations:\n";
    for (decltype(m_operations.size()) i = 0; i < m_operations.size(); ++i)
    {
        m_ostr << i << ". ";
        m_operations[i]->print(m_ostr,true);
        m_ostr << '\n';
    }
    m_ostr << "\n Enter command ('help' for the list of available commands): ";
}

std::optional<int> FunctionCalculator::readOperationIndex(std::istringstream& iss)
{
    int i = 0;
    iss >> i;

    // if the read operation failed (e.g. characters were entered instead of a number)
    if (iss.fail())
    {
        iss.clear();
        iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InputException("must enter numbers, not characters.");
    }

    // if i out of range the vector operation : -> throw
    if (i >= static_cast<int>(m_operations.size()) || i<0)
    {
        throw InputException("out of range the vector operation");
    }

    return i;
}

FunctionCalculator::Action FunctionCalculator::readAction(std::istringstream& iss)
{
    auto action = std::string();
    iss >> action;

    const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);

   // If a number was entered outside the range of the operation vector
    if (i == m_actions.end())
	{
        return Action::Invalid;
	}
   
    return i->action;
}

void FunctionCalculator::runAction(Action action , std::istringstream& iss , std::istream& istr)
{
    switch (action)
    {
        default:
            m_ostr << "Unknown enum entry used!\n";
            break;

        case Action::Invalid:
            throw InputException("Command not found\n");
            break;

        case Action::Eval:     eval(iss, istr);                 break;
        case Action::Add:      binaryFunc<Add>(iss);            break;
        case Action::Sub:      binaryFunc<Sub>(iss);            break;
        case Action::Comp:     binaryFunc<Comp>(iss);           break;
        case Action::Del:      del(iss);                        break;
        case Action::Help:     help();                          break;
        case Action::Exit:     exit();                          break;
        case Action::Iden:     unaryFunc<Identity>();           break;
        case Action::Tran:     unaryFunc<Transpose>();          break;
        case Action::Scal:     unaryWithIntFunc<Scalar>(iss);   break;
        case Action::Read:     read(iss);                       break;
        case Action::Resize:   resize(istr);                    break;
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
        },
        {
            "read",
            " the file , enter path the file",
            Action::Read
        },
    	{
			"resize",
            " num - change the maximum number of operations",
            Action::Resize
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

bool FunctionCalculator::hasNonWhitespace(std::istringstream& iss)
{
    return (!(iss.eof() || (iss >> std::ws).eof()));
}

void FunctionCalculator::updateMaxFunc()
{
    do {
        try {
            m_ostr << "Enter the number of operations to be performed (between 2 and 100): ";
            std::cin >> m_maxOperation;

            // if the read operation failed (e.g. characters were entered instead of a number)
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw InputException("Invalid input. Please enter a number.");
            }

            if (m_maxOperation < 2 || m_maxOperation > 100) {
                throw InputException("The number of operations must be between 2 and 100.");
            }

            break;
        }
        catch (const InputException& e) {
            m_ostr << "Error: " << e.what() << "\n";
        }
    } while (true);
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void  FunctionCalculator::addOperation(std::shared_ptr<Operation> op)
{
    if (m_operations.size() > m_maxOperation)
    {
        throw InputException("Cannot add more operations: maximum limit of " + std::to_string(m_maxOperation));
    }

    m_operations.push_back(std::move(op));
}
