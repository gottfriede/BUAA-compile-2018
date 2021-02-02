#include "SymbolTable.h"

#include <utility>
#include <iostream>

SymbolTableManager::SymbolTableManager()
{
    global = true;
}

Symbol::Symbol(std::string name, identifierType type, identifierBaseType baseType) :
    name(name), type(type), baseType(baseType)
{
    dim1 = 1;
    dim2 = 1;
    value = 0;
    offset = 0;
}

Symbol::Symbol(std::string name, int value) : name(name), value(value)
{
    type = VAR;
    if (name == "18373584CONSTCHAR")
        baseType = CHAR;
    else if (name == "18373584CONSTINT")
        baseType = INT;
    else baseType = NOEXISTS;
    dim1 = 1;
    dim2 = 1;
    offset = 0;
}

bool SymbolTableManager::insert(std::string name, identifierType type, identifierBaseType baseType)
{
    if (findInNowFunc(name))
        return false;
    if (global)
        globalTable.emplace_back(name, type, baseType);
    else
        tmpTable.emplace_back(name, type, baseType);
    if (type == FUNC)
        global = false;
    if (type == PARAM)
        (*(globalTable.end()-1)).params.emplace_back(name, type, baseType);
    return true;
}

bool SymbolTableManager::findInNowFunc(std::string name)
{
    if (global)
    {
        std::vector<Symbol>::iterator it;
        for (it = globalTable.begin(); it != globalTable.end(); it ++)
        {
            if ((*it).name == name)
                return true;
        }
    }
    else{
        std::vector<Symbol>::iterator it;
        for (it = tmpTable.begin(); it != tmpTable.end(); it ++)
        {
            if ((*it).name == name)
                return true;
        }
    }
    return false;
}

identifierType SymbolTableManager::findInAllFunc(std::string name)
{
    std::vector<Symbol>::iterator it;
    for (it = tmpTable.begin(); it != tmpTable.end(); it ++)
    {
        if ((*it).name == name)
            return (*it).type;
    }
    for (it = globalTable.begin(); it != globalTable.end(); it ++)
    {
        if ((*it).name == name)
            return (*it).type;
    }
    return NOTEXISTS;
}

identifierBaseType SymbolTableManager::findBaseInAllFunc(std::string name)
{
    std::vector<Symbol>::iterator it;
    for (it = tmpTable.begin(); it != tmpTable.end(); it ++)
    {
        if ((*it).name == name)
            return (*it).baseType;
    }
    for (it = globalTable.begin(); it != globalTable.end(); it ++)
    {
        if ((*it).name == name)
            return (*it).baseType;
    }
    return NOEXISTS;
}

Symbol SymbolTableManager::findSymbol(std::string name)
{
    std::vector<Symbol>::iterator it;
    for (it = tmpTable.begin(); it != tmpTable.end(); it ++)
    {
        if ((*it).name == name)
            return (*it);
    }
    for (it = globalTable.begin(); it != globalTable.end(); it ++)
    {
        if ((*it).name == name)
            return (*it);
    }
    return Symbol("18373584null",NOTEXISTS,NOEXISTS);
}

void SymbolTableManager::clearClosetFunc()
{
    tmpTable.clear();
    global = true;
}

std::vector<Symbol> SymbolTableManager::getFuncParams(std::string name)
{
    std::vector<Symbol>::iterator it;
    for (it = globalTable.begin(); it != globalTable.end(); it ++)
    {
        if ((*it).name == name)
            return (*it).params;
    }
    std::vector<Symbol> emptyVector;
    return emptyVector;
}

void SymbolTableManager::changeVarType(int dim, int dim1, int dim2)
{
    identifierType idType = (dim == 1) ? ARRAY1 : ARRAY2;
    if (global)
    {
        (*(globalTable.end()-1)).type = idType;
        (*(globalTable.end()-1)).dim1 = dim1;
        (*(globalTable.end()-1)).dim2 = dim2;
    }
    else {
        (*(tmpTable.end()-1)).type = idType;
        (*(tmpTable.end()-1)).dim1 = dim1;
        (*(tmpTable.end()-1)).dim2 = dim2;
    }
}

void SymbolTableManager::changeVarValue(int value)
{
    if (global)
        (*(globalTable.end()-1)).value = value;
    else
        (*(tmpTable.end()-1)).value = value;
}

identifierBaseType SymbolTableManager::findClosetBaseType()
{
    if (global)
    {
        return (*(globalTable.end()-1)).baseType;
    }
    else return (*(tmpTable.end()-1)).baseType;
}

Symbol SymbolTableManager::findClosetSymbol()
{
    if (global)
    {
        return (*(globalTable.end()-1));
    }
    else return (*(tmpTable.end()-1));
}

Symbol SymbolTableManager::getNowFunc()
{
    return (*(globalTable.end()-1));
}

void SymbolTableManager::outputGlobalSymbolTable()
{
//    std::vector<Symbol>::iterator it;
//    for (it = globalTable.begin(); it != globalTable.end(); it ++)
//    {
//        std::cout << (*it).name << " ";
//        switch ((*it).type)
//        {
//            case CONST:
//                std::cout << "CONST" << " ";
//                break;
//            case VAR:
//                std::cout << "VAR" << " ";
//                break;
//            case ARRAY1:
//                std::cout << "ARRAY1" << " ";
//                break;
//            case ARRAY2:
//                std::cout << "ARRAY2" << " ";
//                break;
//            case FUNC:
//                std::cout << "FUNC" << " ";
//                break;
//            case PARAM:
//                std::cout << "PARAM" << " ";
//                break;
//        }
//        switch ((*it).baseType)
//        {
//            case INT:
//                std::cout << "INT" << " ";
//                break;
//            case CHAR:
//                std::cout << "CHAR" << " ";
//                break;
//            case VOID:
//                std::cout << "VOID" << " ";
//                break;
//        }
//        if ((*it).type == FUNC)
//        {
//            std::cout << std::endl << "        ";
//            std::vector<Symbol>::iterator it2;
//            for (it2 = (*it).params.begin(); it2 != (*it).params.end(); it2 ++)
//                std::cout << (*it2).name << " " << (*it2).baseType << " ";
//        }
//        std::cout << std::endl;
//    }
}