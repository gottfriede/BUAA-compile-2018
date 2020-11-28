#include <string>
#include <vector>

#ifndef HW4_SYMBOLTABLE_H
#define HW4_SYMBOLTABLE_H

#endif //HW4_SYMBOLTABLE_H

enum identifierType
{
    CONST,
    VAR,
    ARRAY1,
    ARRAY2,
    FUNC,
    PARAM,
    NOTEXISTS
};

enum identifierBaseType
{
    INT,
    CHAR,
    VOID,
    NOEXISTS
};

struct Symbol
{
    Symbol(std::string name, identifierType type, identifierBaseType baseType);
    Symbol(std::string name, int value);

    std::string name;
    identifierType type;
    identifierBaseType baseType;
    std::vector<Symbol> params;
    int dim1;
    int dim2;
    int value;
    int offset;
};

class SymbolTableManager
{
private:
    std::vector<Symbol> globalTable;
    std::vector<Symbol> tmpTable;
    bool global;

public:
    SymbolTableManager();
    bool insert(std::string name, identifierType type, identifierBaseType baseType);
    void clearClosetFunc();
    bool findInNowFunc(std::string name);
    identifierType findInAllFunc(std::string name);
    identifierBaseType findBaseInAllFunc(std::string name);
    Symbol findSymbol(std::string name);
    std::vector<Symbol> getFuncParams(std::string name);
    void changeVarType(int dim, int dim1, int dim2);
    void changeVarValue(int value);
    identifierBaseType findClosetBaseType();
    Symbol findClosetSymbol();
    void outputGlobalSymbolTable();
};
