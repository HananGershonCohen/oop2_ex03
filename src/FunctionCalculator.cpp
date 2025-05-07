#include "FunctionCalculator.h"
#include "SquareMatrix.h"
#include "Add.h"
#include "Sub.h"
#include "Comp.h"
#include "Identity.h"
#include "Transpose.h"
#include "Scalar.h"
#include "InputException.h"
#include <iostream>
#include <algorithm>



const auto MAX_MAT_SIZE = 5;

FunctionCalculator::FunctionCalculator(std::istream& istr, std::ostream& ostr)
    : m_actions(createActions()), m_operations(createOperations()), m_istr(istr), m_ostr(ostr) {}


void FunctionCalculator::run()
{
    do
    {
        m_ostr << '\n';
        printOperations();
        m_ostr << "Enter command ('help' for the list of available commands): ";
        try {
            const auto action = readAction();
            runAction(action);
        }
        catch (const InputException& e)
        {
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
        m_istr >> size;
        
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


void FunctionCalculator::printOperations() const
{
    m_ostr << "List of available matrix operations:\n";
    for (decltype(m_operations.size()) i = 0; i < m_operations.size(); ++i)
    {
        m_ostr << i << ". ";
        m_operations[i]->print(m_ostr,true);
        m_ostr << '\n';
    }
    m_ostr << '\n';
}


std::optional<int> FunctionCalculator::readOperationIndex() const
{
    int i = 0;
    m_istr >> i;

    // if the read operation failed (e.g. characters were entered instead of a number)
    if (m_istr.fail())
    {
        m_istr.clear(); 
        m_istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InputException("must enter numbers, not characters.");
    }

    // if i out of range the vector operation : -> throw
    if (i >= static_cast<int>(m_operations.size()) || i<0)
    {
        throw InputException("out of range the vector operation");
    }

    return i;
}


FunctionCalculator::Action FunctionCalculator::readAction() const
{
    auto action = std::string();
    m_istr >> action;

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
