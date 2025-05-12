#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>
#include <optional>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

class Operation;


class FunctionCalculator
{
public:
    FunctionCalculator(std::ostream& ostr);
    void run();
    void run(std::istream& istr);

private:
    void eval(std::istringstream&, std::istream&);
    void del(std::istringstream&);
    void help();
    void exit();
    void read(std::istringstream&);
    void resize(std::istream&);

    template <typename FuncType>
    void binaryFunc(std::istringstream& iss)
    {
        if (auto f0 = readOperationIndex(iss), f1 = readOperationIndex(iss); f0 && f1)
        {
            addOperation(std::make_shared<FuncType>(m_operations[*f0], m_operations[*f1]));
        }
    }

    template <typename FuncType>
    void unaryFunc()
    {
        addOperation(std::make_shared<FuncType>());
    }

    template <typename FuncType>
    void unaryWithIntFunc(std::istringstream& iss)
    {
        int i = 0;
        iss >> i;
        if (iss.fail())
        {
            throw std::runtime_error("Invalid input: expected an integer.");
        }
        addOperation(std::make_shared<FuncType>(i));
    }

    void printOperations() const;

    enum class Action
    {
        Invalid,
        Eval,
        Iden,
        Tran,
        Scal,
        Sub,
        Add,
        Mul,
        Comp,
        Del,
        Help,
        Exit,
        Read,
        Resize
    };

    // Command line
    struct ActionDetails
    {
        std::string command; 
        std::string description; 
        Action action;
    };

    using ActionMap = std::vector<ActionDetails>;
    using OperationList = std::vector<std::shared_ptr<Operation>>;

    const ActionMap m_actions;
    OperationList m_operations;
    bool m_running = true;
    //std::istream& m_istr;
    std::ostream& m_ostr;
	int m_maxOperation = 3; // number of operations are leagelly
    //std::istringstream m_iss;
    std::string m_line;
	// Can be wrapped inside a class. ReadFile Class

	


    std::optional<int> readOperationIndex(std::istringstream&) ;
    Action readAction(std::istringstream& iss);
    
    void runAction(Action action , std::istringstream&, std::istream&);

    ActionMap createActions() const;
    OperationList createOperations() const ;

    bool hasNonWhitespace(std::istringstream&);
    void updateMaxFunc();
    void addOperation(std::shared_ptr<Operation>);
};
