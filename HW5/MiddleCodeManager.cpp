#include "MiddleCodeManager.h"

MiddleCode::MiddleCode(Operator op, Symbol id1, Symbol id2, Symbol id3) : op(op), id1(id1), id2(id2), id3(id3)
{}

MiddleCodeManager::MiddleCodeManager()
{
    strList.emplace_back("\\n");
}

int MiddleCodeManager::addStr(std::string str)
{
    int ret = strList.size();
    strList.emplace_back(str);
    return ret;
}

void MiddleCodeManager::addMiddleCode(Operator op, Symbol id1)
{
    middleCodeList.emplace_back(MiddleCode(op, id1, nullSymbol, nullSymbol));
}

void MiddleCodeManager::addMiddleCode(Operator op, Symbol id1, Symbol id2)
{
    middleCodeList.emplace_back(MiddleCode(op, id1, id2, nullSymbol));
}

void MiddleCodeManager::addMiddleCode(Operator op, Symbol id1, Symbol id2, Symbol id3)
{
    middleCodeList.emplace_back(MiddleCode(op, id1, id2, id3));
}

void MiddleCodeManager::beginGenerate()
{
    outputMiddleCode();
    generateStr();
    generateGlobalVar();
    generateFunc("main");
    outputMips();
}

void MiddleCodeManager::generateStr()
{
    mipsList.emplace_back(".data");
    int index = 0;
    std::vector<std::string>::iterator itt;
    for (itt = strList.begin(); itt != strList.end(); itt ++)
    {
        std::string mips = "string";
        mips.append(std::to_string(index));
        mips.append(": .asciiz \"");
        mips.append((*itt));
        mips.append("\"");
        mipsList.emplace_back(mips);
        index ++;
    }
    mipsList.emplace_back("\n");
}

void MiddleCodeManager::generateGlobalVar()
{
    mipsList.emplace_back(".text");
    int offset = 0;
    std::vector<MiddleCode>::iterator it;
    for (it = middleCodeList.begin(); it != middleCodeList.end(); it ++)
    {
        if ((*it).op == OpLabel)
            break;
        if ((*it).op == OpDeclare)
        {
            Symbol symbol = (*it).id1;
            symbol.offset = offset;
            globalSymbolList.emplace_back(symbol);
            std::string mips = "li $t0, ";
            mips.append(std::to_string(symbol.value));
            mipsList.emplace_back(mips);
            mips = "sw $t0, ";
            mips.append(std::to_string(symbol.offset));
            mips.append("($gp)        # ");
            mips.append(symbol.name);
            mipsList.emplace_back(mips);
            offset += 4;
        }
    }
    mipsList.emplace_back("j main\n");
}

void MiddleCodeManager::generateFunc(std::string funcName)
{
    std::string mips = funcName;
    mips.append(":");
    mipsList.emplace_back(mips);
    std::vector<MiddleCode>::iterator it;
    for (it = middleCodeList.begin(); it != middleCodeList.end(); it ++)
    {
        if ((*it).op == OpLabel && (*it).id1.name == funcName)
            break;
    }
    auto funcBegin = it;
    int offset = 0;
    for (it = funcBegin; it != middleCodeList.end(); it ++)
    {
        Symbol symbol1("18373584null", -1);
        if ((*it).op == OpDeclare)
            symbol1 = (*it).id1;
        else if ((*it).op == OpAdd || (*it).op == OpMinus || (*it).op == OpMulti || (*it).op == OpDiv)
            symbol1 = (*it).id3;
        if (symbol1.name != "18373584null")
        {
            symbol1.offset = offset;
            tmpSymbolList.emplace_back(symbol1);
            offset += 4;
        }
    }
    mips = "addi $sp, $sp, -";
    mips.append(std::to_string(offset));
    mipsList.emplace_back(mips);
    std::vector<Symbol>::iterator symbolIt;
    for (symbolIt = tmpSymbolList.begin(); symbolIt != tmpSymbolList.end(); symbolIt ++)
    {
        Symbol symbol = (*symbolIt);
        mips = "li $t0, ";
        mips.append(std::to_string(symbol.value));
        mipsList.emplace_back(mips);
        mips = "sw $t0, ";
        mips.append(std::to_string(symbol.offset));
        mips.append("($sp)        # ");
        mips.append(symbol.name);
        mipsList.emplace_back(mips);
    }
    for (it = funcBegin; it != middleCodeList.end(); it ++)
    {
        if ((*it).op == OpScan)
        {
            identifierBaseType baseType = (*it).id1.baseType;
            if (baseType == INT)
                mips = "li $v0, 5";
            if (baseType == CHAR)
                mips = "li $v0, 12";
            mipsList.emplace_back(mips);
            mips = "syscall        # scanf ";
            mips.append((*it).id1.name);
            mipsList.emplace_back(mips);
            mips = sw("$v0", (*it).id1.name);
            mipsList.emplace_back(mips);
        }
        if ((*it).op == OpPrint)
        {
            if ((*it).id1.name == "expression")
            {
                if ((*it).id2.name == "18373584CONSTINT" || (*it).id2.name == "18373584CONSTCHAR")
                {
                    mips = "li $a0, ";
                    mips.append(std::to_string((*it).id2.value));
                }
                else
                    mips = lw("$a0", (*it).id2.name);
                mipsList.emplace_back(mips);
                identifierBaseType baseType = (*it).id2.baseType;
                if (baseType == INT)
                    mips = "li $v0, 1";
                if (baseType == CHAR)
                    mips = "li $v0, 11";
                mipsList.emplace_back(mips);
                mips = "syscall        # printf ";
                mips.append((*it).id2.name);
                mipsList.emplace_back(mips);
            }
            else if ((*it).id1.name == "string")
            {
                mips = "li $v0, 4";
                mipsList.emplace_back(mips);
                mips = "la $a0, ";
                mips.append((*it).id2.name);
                mips.append(std::to_string((*it).id2.value));
                mipsList.emplace_back(mips);
                mipsList.emplace_back("syscall        # printf string");
            }
        }
        if ((*it).op == OpAdd || (*it).op == OpMinus || (*it).op == OpMulti || (*it).op == OpDiv)
        {
            Symbol symbol1 = (*it).id1;
            Symbol symbol2 = (*it).id2;
            Symbol symbol3 = (*it).id3;
            if (symbol1.name == "18373584CONSTINT" || symbol1.name == "18373584CONSTCHAR")
            {
                mips = "li $t0, ";
                mips.append(std::to_string(symbol1.value));
                mipsList.emplace_back(mips);
            }
            else
            {
                mips = lw("$t0", symbol1.name);
                mipsList.emplace_back(mips);
            }
            if (symbol2.name == "18373584CONSTINT" || symbol2.name == "18373584CONSTCHAR")
            {
                mips = "li $t1, ";
                mips.append(std::to_string(symbol2.value));
                mipsList.emplace_back(mips);
            }
            else
            {
                mips = lw("$t1", symbol2.name);
                mipsList.emplace_back(mips);
            }
            if ((*it).op == OpAdd)
                mips = "addu $t2, $t0, $t1";
            else if ((*it).op == OpMinus)
                mips = "subu $t2, $t0, $t1";
            else if ((*it).op == OpMulti)
                mips = "mult $t0, $t1";
            else if ((*it).op == OpDiv)
                mips = "div $t0, $t1";
            mipsList.emplace_back(mips);
            if ((*it).op == OpMulti || (*it).op == OpDiv)
            {
                mips = "mflo $t2";
                mipsList.emplace_back(mips);
            }
            mips = sw("$t2", symbol3.name);
            mipsList.emplace_back(mips);
        }
        if ((*it).op == OpAssign)
        {
            if ((*it).id2.name == "18373584CONSTINT" || (*it).id2.name == "18373584CONSTCHAR")
            {
                mips = "li $t0, ";
                mips.append(std::to_string((*it).id2.value));
            }
            else
                mips = lw("$t0", (*it).id2.name);
            mipsList.emplace_back(mips);
            mips = sw("$t0", (*it).id1.name);
            mipsList.emplace_back(mips);
        }
    }
}

std::pair<bool, int> MiddleCodeManager::getOffset(std::string name)
{
    std::vector<Symbol>::iterator it;
    for (it = tmpSymbolList.begin(); it != tmpSymbolList.end(); it ++)
    {
        if ((*it).name == name)
            return std::pair<bool, int>(true, (*it).offset);
    }
    for (it = globalSymbolList.begin(); it != globalSymbolList.end(); it ++)
    {
        if ((*it).name == name)
            return std::pair<bool, int>(false, (*it).offset);
    }
    return std::pair<bool, int>(false, -1);
}

std::string MiddleCodeManager::lw(std::string grf, std::string idName)
{
    std::string mips = "lw ";
    mips.append(grf);
    mips.append(", ");
    std::pair<bool, int> ans = getOffset(idName);
    int offset = ans.second;
    std::string base = (ans.first) ? "$sp" : "$gp";
    mips.append(std::to_string(offset));
    mips.append("(");
    mips.append(base);
    mips.append(")");
    return mips;
}

std::string MiddleCodeManager::sw(std::string grf, std::string idName)
{
    std::string mips = "sw ";
    mips.append(grf);
    mips.append(", ");
    std::pair<bool, int> ans = getOffset(idName);
    int offset = ans.second;
    std::string base = (ans.first) ? "$sp" : "$gp";
    mips.append(std::to_string(offset));
    mips.append("(");
    mips.append(base);
    mips.append(")");
    return mips;
}

void MiddleCodeManager::outputMiddleCode()
{
    int index = 0;
    std::vector<std::string>::iterator itt;
    for (itt = strList.begin(); itt != strList.end(); itt ++)
    {
        std::cout << "string" << index << ": " << (*itt)  << std::endl;
        index ++;
    }
    std::cout << "-------------------------------------" << std::endl;
    std::vector<MiddleCode>::iterator it;
    for (it = middleCodeList.begin(); it != middleCodeList.end(); it ++)
    {
        if ((*it).op == OpDeclare)
        {
            std::cout << "DECLARE" << " " << (*it).id1.name << " (" << (*it).id1.type << " " << (*it).id1.baseType << " " << (*it).id1.value << ")" << std::endl;
        }
        if ((*it).op == OpLabel)
        {
            std::cout << "LABEL" << " " << (*it).id1.name << std::endl;
        }
        if ((*it).op == OpScan)
        {
            std::cout << "SCANF" << " " << (*it).id1.name << " ("  << (*it).id1.baseType  << ")" << std::endl;
        }
        if ((*it).op == OpPrint)
        {
            std::cout << "PRINT" << " " << (*it).id2.name ;
            if ((*it).id1.name == "string")
                std::cout << (*it).id2.value << std::endl;
            else if ((*it).id1.name == "expression")
                std::cout << " (" << (*it).id1.baseType << ")" << std::endl;
        }
        if ((*it).op == OpAssign)
        {
            std::cout << "ASSIGN" << " " << (*it).id1.name << " ("  << (*it).id1.baseType  << ")" << " " << (*it).id2.name << " ("  << (*it).id2.baseType  << ")" << std::endl;
        }
        if ((*it).op == OpAdd)
        {
            std::cout << "ADD" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpMinus)
        {
            std::cout << "MINUS" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpMulti)
        {
            std::cout << "MULTI" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpDiv)
        {
            std::cout << "DIV" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
    }
}

void MiddleCodeManager::outputMips()
{
    std::ofstream outputFile;
    outputFile.open("mips.txt");
    std::vector<std::string>::iterator ittt;
    for (ittt = mipsList.begin(); ittt != mipsList.end(); ittt ++)
        outputFile << (*ittt) << std::endl;
}
