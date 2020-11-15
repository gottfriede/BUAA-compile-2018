#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include "SymbolTable.h"

#ifndef HW5_MIDDLECODEMANAGER_H
#define HW5_MIDDLECODEMANAGER_H

#endif //HW5_MIDDLECODEMANAGER_H

enum Operator
{
    OpAdd,
    OpMinus,
    OpMulti,
    OpDiv,
    OpAssign,
    OpScan,
    OpPrint,
    OpLabel,
    OpDeclare
};

struct MiddleCode
{
    MiddleCode(Operator op, Symbol id1, Symbol id2, Symbol id3);

    Operator op;
    Symbol id1;
    Symbol id2;
    Symbol id3;
};

class MiddleCodeManager
{
private:
    Symbol nullSymbol = Symbol("18373584null", NOTEXISTS, NOEXISTS);
    std::vector<MiddleCode> middleCodeList;
    std::vector<std::string> strList;
    std::vector<std::string> mipsList;
    std::vector<Symbol> globalSymbolList;
    std::vector<Symbol> tmpSymbolList;
    std::pair<bool, int> getOffset(std::string name);
    std::string lw(std::string grf, std::string idName);
    std::string sw(std::string grf, std::string idName);

public:
    friend class SymbolTableManager;
    MiddleCodeManager();
    void addMiddleCode(Operator op, Symbol id1);
    int addStr(std::string str);
    void addMiddleCode(Operator op, Symbol id1, Symbol id2);
    void addMiddleCode(Operator op, Symbol id1, Symbol id2, Symbol id3);
    void beginGenerate();
    void generateStr();
    void generateGlobalVar();
    void generateFunc(std::string funcName);
    void outputMiddleCode();
    void outputMips();
};