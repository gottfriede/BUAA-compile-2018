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
    std::vector<MiddleCode>::iterator it;
    for (it = middleCodeList.begin(); it != middleCodeList.end(); it ++)
    {
        if ((*it).op == OpFuncDeclare)
            generateFunc((*it).id1.name);
    }
    generateProgramEnd();
    someOptimize();
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
        if ((*it).op == OpFuncDeclare)
            break;
        if ((*it).op == OpDeclare)
        {
            if ((*it).id1.dim1 == 1 && (*it).id1.dim2 == 1)
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
            else {
                Symbol symbol = (*it).id1;
                symbol.offset = offset;
                globalSymbolList.emplace_back(symbol);
                offset += symbol.dim1*symbol.dim2*4;
            }
        }
    }
    for (it = middleCodeList.begin(); it != middleCodeList.end(); it ++)
    {
        if ((*it).op == OpFuncDeclare)
            break;
        if ((*it).op == OpArrayInit)
        {
            Symbol symbol = (*it).id1;
            int baseOffset = getOffset(symbol.name).second;
            int newOffset = baseOffset + ((*it).id2.value)*4;
            std::string mips = "li $t0, ";
            mips.append(std::to_string((*it).id3.value));
            mipsList.emplace_back(mips);
            mips = "sw $t0, ";
            mips.append(std::to_string(newOffset));
            mips.append("($gp)        # ");
            mips.append(symbol.name + "[" + std::to_string((*it).id2.value) + "]");
            mipsList.emplace_back(mips);
        }
    }
    mipsList.emplace_back("j FUNC_main");
}

void MiddleCodeManager::generateFunc(std::string funcName)
{
    tmpSymbolList.clear();
    regSMap.clear();
    regSIndex = 0;
    for (int i = 0; i < 7 ;i ++)
        dirty[i] = 0;
    mipsList.emplace_back("\n");
    std::string mips = funcName;
    mips.append(":");
    mipsList.emplace_back(mips);
    std::vector<MiddleCode>::iterator it;
    for (it = middleCodeList.begin(); it != middleCodeList.end(); it ++)
    {
        if ((*it).op == OpFuncDeclare && (*it).id1.name == funcName)
            break;
    }
    auto funcBegin = it;
    int offset = 0;
    std::vector<Symbol>::iterator itt;
    for (itt = (*funcBegin).id1.params.begin(); itt != (*funcBegin).id1.params.end(); itt++)
    {
        Symbol symbol1 = *itt;
        if (regSIndex <= 7)
        {
            symbol1.offset = -2;
            tmpSymbolList.emplace_back(symbol1);
            std::string reg = "$s" + std::to_string(regSIndex);
            regSMap.insert(std::pair<std::string, Symbol>(reg, symbol1));
            regSIndex ++;
        } else {
            symbol1.offset = offset;
            tmpSymbolList.emplace_back(symbol1);
            offset += 4;
        }
    }
    for (it = funcBegin; it != middleCodeList.end(); it ++)
    {
        Symbol symbol1("18373584null", -1);
        if ((*it).op == OpFuncDeclare && (*it).id1.name != funcName)
            break;
        if ((*it).op == OpDeclare)
        {
            symbol1 = (*it).id1;
            if (regSIndex <= 7 && (symbol1.type == VAR || symbol1.type == CONST))
            {
                symbol1.offset = -2;
                tmpSymbolList.emplace_back(symbol1);
                std::string reg = "$s" + std::to_string(regSIndex);
                regSMap.insert(std::pair<std::string, Symbol>(reg, symbol1));
                regSIndex ++;
            } else {
                symbol1.offset = offset;
                tmpSymbolList.emplace_back(symbol1);
                offset += symbol1.dim1*symbol1.dim2*4;
            }
        }
        else if ((*it).op == OpAdd || (*it).op == OpMinus || (*it).op == OpMulti || (*it).op == OpDiv)
        {
            symbol1 = (*it).id1;
            if (symbol1.name.find("18373584tmp") == 0  && getOffset(symbol1.name).second == -1)
            {
                symbol1.offset = offset;
                tmpSymbolList.emplace_back(symbol1);
                offset += 4;
            }
            symbol1 = (*it).id2;
            if (symbol1.name.find("18373584tmp") == 0  && getOffset(symbol1.name).second == -1)
            {
                symbol1.offset = offset;
                tmpSymbolList.emplace_back(symbol1);
                offset += 4;
            }
            symbol1 = (*it).id3;
            if (symbol1.name.find("18373584tmp") == 0  && getOffset(symbol1.name).second == -1)
            {
                symbol1.offset = offset;
                tmpSymbolList.emplace_back(symbol1);
                offset += 4;
            }
        }
        else if ((*it).op == OpArrayGet)
        {
            symbol1 = (*it).id2;
            if (symbol1.name.find("18373584tmp") == 0  && getOffset(symbol1.name).second == -1)
            {
                symbol1.offset = offset;
                tmpSymbolList.emplace_back(symbol1);
                offset += 4;
            }
            symbol1 = (*it).id3;
            if (symbol1.name.find("18373584tmp") == 0  && getOffset(symbol1.name).second == -1)
            {
                symbol1.offset = offset;
                tmpSymbolList.emplace_back(symbol1);
                offset += 4;
            }
        }
        else if ((*it).op == OpFuncCall)
        {
            symbol1 = (*it).id2;
            if (symbol1.name.find("18373584tmp") == 0  && getOffset(symbol1.name).second == -1)
            {
                symbol1.offset = offset;
                tmpSymbolList.emplace_back(symbol1);
                offset += 4;
            }
        }
    }
    mips = "addi $sp, $sp, -";
    mips.append(std::to_string(offset));
    mipsList.emplace_back(mips);
    std::vector<Symbol>::iterator symbolIt;
    int paramNumber = (*funcBegin).id1.params.size();
    int tmpParamNumber = 0;
    for (symbolIt = tmpSymbolList.begin(); symbolIt != tmpSymbolList.end(); symbolIt ++)
    {
        Symbol symbol = (*symbolIt);
        if (symbol.type != PARAM)
        {
            if (getRegS(symbol.name) != "")
            {
                mips = "li ";
                mips.append( getRegS(symbol.name) );
                mips.append(", ");
                mips.append( std::to_string(symbol.value) );
                mips.append("            # ");
                mips.append(symbol.name);
                mipsList.emplace_back(mips);
            } else {
                mips = "li $t0, ";
                mips.append(std::to_string(symbol.value));
                mipsList.emplace_back(mips);
                mips = "sw $t0, ";
                mips.append(std::to_string(symbol.offset));
                mips.append("($sp)        # ");
                mips.append(symbol.name);
                mipsList.emplace_back(mips);
            }
        }
        else
        {
            tmpParamNumber ++;
            mips = "lw $t0, ";
            mips.append(std::to_string(offset+12+4*paramNumber-4*tmpParamNumber));
            mips.append("($sp)");
            mipsList.emplace_back(mips);

            if (getRegS(symbol.name) != "")
            {
                mips = "addu ";
                mips.append( getRegS(symbol.name) );
                mips.append(", $t0, $0");
                mips.append("            # ");
                mips.append(symbol.name);
                mipsList.emplace_back(mips);
            } else {
                mips = "sw $t0, ";
                mips.append(std::to_string(symbol.offset));
                mips.append("($sp)        # ");
                mips.append(symbol.name);
                mipsList.emplace_back(mips);
            }
        }
    }
    int funcOffset = 0;
    for (it = funcBegin; it != middleCodeList.end(); it ++)
    {
        if ((*it).op == OpFuncDeclare && (*it).id1.name != funcName)
            break;
        if ((*it).op == OpArrayInit)
        {
            Symbol symbol = (*it).id1;
            int baseOffset = getOffset(symbol.name).second;
            int newOffset = baseOffset + ((*it).id2.value)*4;
            std::string mips = "li $t0, ";
            mips.append(std::to_string((*it).id3.value));
            mipsList.emplace_back(mips);
            mips = "sw $t0, ";
            mips.append(std::to_string(newOffset));
            mips.append("($sp)        # ");
            mips.append(symbol.name + "[" + std::to_string((*it).id2.value) + "]");
            mipsList.emplace_back(mips);
        }
    }
    for (it = funcBegin; it != middleCodeList.end(); it ++)
    {
        if ((*it).op == OpFuncDeclare && (*it).id1.name != funcName)
            break;
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
            std::string reg1;
            std::string reg2;
            std::string reg3;
            if (symbol1.name == "18373584CONSTINT" || symbol1.name == "18373584CONSTCHAR")
            {
                mips = "li $t0, " + std::to_string(symbol1.value);
                mipsList.emplace_back(mips);
                reg1 = "$t0";
            }
            else if (!getRegS(symbol1.name).empty())
            {
                reg1 = getRegS(symbol1.name);
            }
            else if (findRegT(symbol1.name) != -1)
            {
                int tReg = findRegT(symbol1.name);
                reg1 = "$t" + std::to_string(tReg+3);
                dirty[tReg] = 0;
            }
            else
            {
                mips = lw("$t0", symbol1.name);
                mipsList.emplace_back(mips);
                reg1 = "$t0";
            }
            if (symbol2.name == "18373584CONSTINT" || symbol2.name == "18373584CONSTCHAR")
            {
                mips = "li $t1, " + std::to_string(symbol2.value);
                mipsList.emplace_back(mips);
                reg2 = "$t1";
            }
            else if (!getRegS(symbol2.name).empty())
            {
                reg2 = getRegS(symbol2.name);
            }
            else if (findRegT(symbol2.name) != -1)
            {
                int tReg = findRegT(symbol2.name);
                reg2 = "$t" + std::to_string(tReg+3);
                dirty[tReg] = 0;
            }
            else
            {
                mips = lw("$t1", symbol2.name);
                mipsList.emplace_back(mips);
                reg2 = "$t1";
            }
            if (!getRegS(symbol3.name).empty())
                reg3 = getRegS(symbol3.name);
            else {
                int reg3Index = getNewRegT();
                if (reg3Index != -1)
                {
                    dirty[reg3Index] = 1;
                    regT[reg3Index] = symbol3.name;
                    reg3 = "$t" + std::to_string(reg3Index+3);
                }
                else
                    reg3 = "$t2";
            }
            if ((*it).op == OpAdd)
                mips = "addu " + reg3 + ", " + reg1 + ", " + reg2;
            else if ((*it).op == OpMinus)
                mips = "subu " + reg3 + ", " + reg1 + ", " + reg2;
            else if ((*it).op == OpMulti)
                mips = "mult " + reg1 + ", " + reg2;
            else if ((*it).op == OpDiv)
                mips = "div " + reg1 + ", " + reg2;
            mipsList.emplace_back(mips);
            if ((*it).op == OpMulti || (*it).op == OpDiv)
            {
                mips = "mflo " + reg3;
                mipsList.emplace_back(mips);
            }
            if (reg3 == "$t2" || symbol3.name.find("18373584tmp") == std::string::npos)
            {
                mips = sw(reg3, symbol3.name);
                mipsList.emplace_back(mips);
            }
        }
        if ((*it).op == OpAssign)
        {
            Symbol symbolFrom = (*it).id2;
            Symbol symbolTo = (*it).id1;
            std::string regFrom;
            if (symbolFrom.name == "18373584CONSTINT" || symbolFrom.name == "18373584CONSTCHAR")
            {
                mips = "li $t0, ";
                mips.append(std::to_string(symbolFrom.value));
                mipsList.emplace_back(mips);
                regFrom = "$t0";
            }
            else if (!getRegS(symbolFrom.name).empty())
            {
                regFrom = getRegS(symbolFrom.name);
            }
            else if (findRegT(symbolFrom.name) != -1)
            {
                int tReg = findRegT(symbolFrom.name);
                regFrom = "$t" + std::to_string(tReg+3);
                dirty[tReg] = 0;
            }
            else
            {
                mips = lw("$t0", symbolFrom.name);
                mipsList.emplace_back(mips);
                regFrom = "$t0";
            }
            mips = sw(regFrom, (*it).id1.name);
            mipsList.emplace_back(mips);
        }
        if ((*it).op == OpArrayGet)
        {
            if ((*it).id2.name == "18373584CONSTINT")
            {
                int offset = getOffset((*it).id1.name).second + 4*(*it).id2.value;
                mips = "lw $t0, " + std::to_string(offset) + "(";
                std::string baseReg = getOffset((*it).id1.name).first ? "$sp" : "$gp";
                mips.append(baseReg + ")");
                mipsList.emplace_back(mips);
            } else {
                mips = lw("$t0", (*it).id2.name);
                mipsList.emplace_back(mips);
                mips = "sll $t0, $t0, 2";
                mipsList.emplace_back(mips);

                mips = "addi $t0, $t0, " + std::to_string(getOffset((*it).id1.name).second);
                mipsList.emplace_back(mips);
                mips = getOffset((*it).id1.name).first ? "addu $t0, $t0, $sp" : "addu $t0, $t0, $gp";
                mipsList.emplace_back(mips);
                mips = "lw $t0, 0($t0)";
                mipsList.emplace_back(mips);
                mips = sw("$t0", (*it).id3.name);
                mipsList.emplace_back(mips);
            }
        }
        if ((*it).op == OpArrayPush)
        {
            if ((*it).id2.name == "18373584CONSTINT")
            {
                mips = "li $t0, ";
                mips.append(std::to_string(4*(*it).id2.value));
                mipsList.emplace_back(mips);
            } else {
                mips = lw("$t0", (*it).id2.name);
                mipsList.emplace_back(mips);
                mips = "sll $t0, $t0, 2";
                mipsList.emplace_back(mips);
            }
            mips = "addi $t0, $t0, " + std::to_string(getOffset((*it).id1.name).second);
            mipsList.emplace_back(mips);
            mips = getOffset((*it).id1.name).first ? "addu $t0, $t0, $sp" : "addu $t0, $t0, $gp";
            mipsList.emplace_back(mips);

            if ((*it).id3.name == "18373584CONSTINT" || (*it).id3.name == "18373584CONSTCHAR")
            {
                mips = "li $t1, ";
                mips.append(std::to_string((*it).id3.value));
            }
            else
                mips = lw("$t1", (*it).id3.name);
            mipsList.emplace_back(mips);
            mips = "sw $t1, 0($t0)";
            mipsList.emplace_back(mips);
        }
        if ((*it).op == OpBgt || (*it).op == OpBge || (*it).op == OpBlt || (*it).op == OpBle || (*it).op == OpBne || (*it).op == OpBeq)
        {
            Operator op = (*it).op;
            if ((*it).id1.name == "18373584CONSTINT" || (*it).id1.name == "18373584CONSTCHAR")
            {
                mips = "li $t0, " + std::to_string((*it).id1.value);
                mipsList.emplace_back(mips);
            } else{
                mips = lw("$t0", (*it).id1.name);
                mipsList.emplace_back(mips);
            }
            if ((*it).id2.name == "18373584CONSTINT" || (*it).id2.name == "18373584CONSTCHAR")
            {
                mips = "li $t1, " + std::to_string((*it).id2.value);
                mipsList.emplace_back(mips);
            } else{
                mips = lw("$t1", (*it).id2.name);
                mipsList.emplace_back(mips);
            }
            mips = "";
            if (op == OpBgt)
                mips = "bgt";
            else if (op == OpBge)
                mips = "bge";
            else if (op == OpBlt)
                mips = "blt";
            else if (op == OpBle)
                mips = "ble";
            else if (op == OpBeq)
                mips = "beq";
            else if (op == OpBne)
                mips = "bne";
            mips.append(" $t0, $t1, " + (*it).id3.name);
            mipsList.emplace_back(mips);
        }
        if ((*it).op == OpJ)
        {
            mips = "j " + (*it).id1.name;
            mipsList.emplace_back(mips);
        }
        if ((*it).op == OpLabel)
        {
            mips = (*it).id1.name + " :";
            mipsList.emplace_back(mips);
        }
        if ((*it).op == OpPushParam)
        {
            paramStack.push((*it).id1);
        }
        if ((*it).op == OpFuncCall)
        {
            for (int i = 0; i < 7; i ++)
            {
                if (dirty[i])
                {
                    mips = sw("$t" + std::to_string(i+3), regT[i]);
                    mipsList.emplace_back(mips);
                    dirty[i] = 0;
                }
            }
            for (int i = 0; i < regSIndex; i ++)
            {
                funcOffset += 4;
                mips = "sw $s" + std::to_string(i) + ", ";
                mips.append(std::to_string(-1*funcOffset));
                mips.append("($sp)");
                mipsList.emplace_back(mips);
            }
            std::stack<Symbol> tmpParamStack;
            int paramNum = (*it).id1.params.size();
            for (int i = 0 ; i < paramNum; i ++)
            {
                Symbol paramSymbol = paramStack.top();
                paramStack.pop();
                tmpParamStack.push(paramSymbol);
            }
            while (! tmpParamStack.empty())
            {
                Symbol paramSymbol = tmpParamStack.top();
                tmpParamStack.pop();

                funcOffset += 4;
                if (paramSymbol.name == "18373584CONSTINT" || paramSymbol.name == "18373584CONSTCHAR")
                {
                    mips = "li, $t0, ";
                    mips.append(std::to_string(paramSymbol.value));
                }
                else mips=lw("$t0", paramSymbol.name);
                mipsList.emplace_back(mips);
                mips = "sw $t0, ";
                mips.append(std::to_string(-1*funcOffset));
                mips.append("($sp)");
                mipsList.emplace_back(mips);
            }
            funcOffset += 4;
            mips = "sw $ra, ";
            mips.append(std::to_string(-1*funcOffset));
            mips.append("($sp)");
            mipsList.emplace_back(mips);
            funcOffset += 4;
            mips = "sw $sp, ";
            mips.append(std::to_string(-1*funcOffset));
            mips.append("($sp)");
            mipsList.emplace_back(mips);
            mips = "addi $sp, $sp, ";
            funcOffset += 4;
            mips.append(std::to_string(-funcOffset));
            mipsList.emplace_back(mips);
            mips = "jal ";
            mips.append((*it).id1.name);
            mipsList.emplace_back(mips);
            mips = "lw $ra, ";
            mips.append(std::to_string(-funcOffset+8));
            mips.append("($sp)");
            mipsList.emplace_back(mips);
            for (int i = regSIndex-1; i >= 0; i --)
            {
                mips = "lw $s" + std::to_string(i) + ", ";
                mips.append(std::to_string(-funcOffset+8+4*(paramNum)+4*(regSIndex-i)));
                mips.append("($sp)");
                mipsList.emplace_back(mips);
            }
            if ((*it).id2.name != "18373584null")
            {
                mips = sw("$v0", (*it).id2.name);
                mipsList.emplace_back(mips);
            }
            funcOffset = 0;
        }
        if ((*it).op == OpFuncReturn)
        {
            if (funcName == "main")
            {
                mips = "j programEnd_18373584";
                mipsList.emplace_back(mips);
            } else {
                if ((*it).id1.name != "18373584null")
                {
                    if ((*it).id1.name == "18373584CONSTINT" || (*it).id1.name == "18373584CONSTCHAR")
                        mips = "li $v0, " + std::to_string((*it).id1.value);
                    else
                        mips = lw("$v0", (*it).id1.name);
                    mipsList.emplace_back(mips);
                }
                mips = "addi $sp, $sp, ";
                mips.append(std::to_string(offset));
                mipsList.emplace_back(mips);
                mips = "lw $sp, 4($sp)";
                mipsList.emplace_back(mips);
                mips = "jr $ra";
                mipsList.emplace_back(mips);
            }
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
    if (offset == -2)
    {
        mips = "addu ";
        mips.append(grf);
        mips.append(", ");
        std::string idReg = getRegS(idName);
        mips.append(idReg);
        mips.append(", $0");
        return mips;
    }
    if (findRegT(idName) != -1)
    {
        mips = "addu " + grf + ", $t" + std::to_string(findRegT(idName)+3) + ", $0";
        return mips;
    }
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
    if (offset == -2)
    {
        mips = "addu ";
        std::string idReg = getRegS(idName);
        mips.append(idReg);
        mips.append(", ");
        mips.append(grf);
        mips.append(", $0");
        return mips;
    }
    std::string base = (ans.first) ? "$sp" : "$gp";
    mips.append(std::to_string(offset));
    mips.append("(");
    mips.append(base);
    mips.append(")");
    return mips;
}

void MiddleCodeManager::generateProgramEnd()
{
    std::string mips = "\nprogramEnd_18373584:";
    mipsList.emplace_back(mips);
    mips = "li $v0, 10";
    mipsList.emplace_back(mips);
    mips = "syscall";
    mipsList.emplace_back(mips);
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
            std::cout << "DECLARE" << " " << (*it).id1.name << " (" << (*it).id1.type << " " << (*it).id1.baseType << " " << (*it).id1.value << ")";
            if ((*it).id1.dim1 != 1 || (*it).id1.dim2 != 1)
            {
                std::cout << "    (" << (*it).id1.dim1 << " " << (*it).id1.dim2 << ")" << std::endl;
            }
            else std::cout << std::endl;
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
        if ((*it).op == OpArrayInit)
        {
            std::cout << "ArrayInit" << " " << (*it).id1.name << " " << (*it).id2.value << " " << (*it).id3.value << std::endl;
        }
        if ((*it).op == OpArrayGet)
        {
            std::cout << "ArrayGet" << " " <<(*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpArrayPush)
        {
            std::cout << "ArrayPush" << " " <<(*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpBeq)
        {
            std::cout << "Beq" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpBne)
        {
            std::cout << "Bne" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpBgt)
        {
            std::cout << "Bgt" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpBge)
        {
            std::cout << "Bge" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpBlt)
        {
            std::cout << "Blt" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpBle)
        {
            std::cout << "Ble" << " " << (*it).id1.name << " " << (*it).id2.name << " " << (*it).id3.name << std::endl;
        }
        if ((*it).op == OpJ)
        {
            std::cout << "J" << " " << (*it).id1.name << std::endl;
        }
        if ((*it).op == OpFuncDeclare)
        {
            std::cout << std::endl << "FuncDeclare" << " " << (*it).id1.name;
            std::cout << "    (params: " ;
            std::vector<Symbol>::iterator itt;
            for (itt = (*it).id1.params.begin(); itt != (*it).id1.params.end(); itt ++)
            {
                std::cout << (*itt).name << " ";
            }
            std::cout << " )" << std::endl;
        }
        if ((*it).op == OpFuncReturn)
        {
            std::cout << "FuncReturn" << " " << (*it).id1.name << std::endl;
        }
        if ((*it).op == OpFuncCall)
        {
            std::cout << "FuncCall" << " " << (*it).id1.name << " " << (*it).id2.name << std::endl;
        }
        if ((*it).op == OpPushParam)
        {
            std::cout << "PushParam" << " " << (*it).id1.name << std::endl;
        }
    }
}

void MiddleCodeManager::someOptimize() {
    std::vector<std::string>::iterator it;
    for (it = mipsList.begin(); it != mipsList.end(); it ++)
    {
        std::string mips = (*it);
        if (mips.find("lw") == 0)
        {
            std::string pre = (*(it-1));
            if (pre.find("sw") == 0)
            {
                int firstDollar1 = mips.find_first_of("$");
                int firstComma1 = mips.find_first_of(",");
                int firstLParent1 = mips.find_first_of("(");
                std::string reg1 = mips.substr(firstDollar1, firstComma1-firstDollar1);
                std::string offset1 = mips.substr(firstComma1, firstLParent1-firstComma1);
                std::string pos1 = mips.substr(firstLParent1);
                int firstDollar2 = pre.find_first_of("$");
                int firstComma2 = pre.find_first_of(",");
                int firstLParent2 = pre.find_first_of("(");
                std::string reg2 = pre.substr(firstDollar2, firstComma2-firstDollar2);
                std::string offset2 = pre.substr(firstComma2, firstLParent2-firstComma2);
                std::string pos2 = pre.substr(firstLParent2);
                if (offset1 == offset2 && pos1 == pos2)
                {
                    std::string optimize = "addu " + reg1 + ", " + reg2 + ", $0";
                    (*it) = optimize;
                }
            }
        }
    }
    for (it = mipsList.begin(); it != mipsList.end(); it ++)
    {
        std::string mips = (*it);
        if (mips.find("addi") == 0)
        {
            int firstDollar = mips.find_first_of("$");
            int lastDollar = mips.find_last_of("$");
            int firstComma = mips.find_first_of(",");
            int lastComma = mips.find_last_of(",");
            std::string reg1 = mips.substr(firstDollar, firstComma-firstDollar);
            std::string reg2 = mips.substr(lastDollar, lastComma-lastDollar);
            std::string value = mips.substr(lastComma+2);
            if (reg1 == reg2 && value == "0")
            {
                mipsList.erase(it);
                it--;
            }
        }
    }
//    for (it = mipsList.begin(); it != mipsList.end(); it ++)
//    {
//        std::string mips = (*it);
//        if (mips.find("sw") == 0 && mips.find("tmp") != std::string::npos)
//        {
//            bool hasLw = false;
//            std::vector<std::string>::iterator itt;
//            for (itt = it; itt != mipsList.end(); itt ++)
//            {
//                std::string lw = (*itt);
//                if (lw.find("lw") == 0)
//                {
//                    int firstDollar1 = mips.find_first_of("$");
//                    int firstComma1 = mips.find_first_of(",");
//                    int firstLParent1 = mips.find_first_of("(");
//                    int firstRParent1 = mips.find_first_of(")");
//                    std::string offset1 = mips.substr(firstComma1, firstLParent1-firstComma1);
//                    std::string pos1 = mips.substr(firstLParent1, firstRParent1-firstLParent1);
//                    int firstDollar2 = lw.find_first_of("$");
//                    int firstComma2 = lw.find_first_of(",");
//                    int firstLParent2 = lw.find_first_of("(");
//                    int firstRParent2 = mips.find_first_of(")");
//                    std::string offset2 = lw.substr(firstComma2, firstLParent2-firstComma2);
//                    std::string pos2 = lw.substr(firstLParent2, firstRParent2-firstLParent2);
//                    if (offset1 == offset2 && pos1 == pos2)
//                    {
//                        hasLw = true;
//                        break;
//                    }
//                }
//            }
//            if (!hasLw)
//            {
//                mipsList.erase(it);
//                it --;
//                mipsList.erase(it);
//                it --;
//            }
//        }
//    }
}

void MiddleCodeManager::outputMips()
{
    std::ofstream outputFile;
    outputFile.open("mips.txt");
    std::vector<std::string>::iterator ittt;
    for (ittt = mipsList.begin(); ittt != mipsList.end(); ittt ++)
        outputFile << (*ittt) << std::endl;
}

std::string MiddleCodeManager::getRegS(std::string idName)
{
    std::map<std::string, Symbol>::iterator it;
    for (it = regSMap.begin(); it != regSMap.end(); it ++)
    {
        if ((*it).second.name == idName)
            return (*it).first;
    }
    return "";
}

int MiddleCodeManager::findRegT(std::string idName)
{
    for (int i = 0; i < 7; i ++)
        if (regT[i] == idName)
            return i;
    return -1;
}

int MiddleCodeManager::getNewRegT()
{
    for (int i = 0; i < 7; i ++)
        if (dirty[i] == 0)
            return i;
    return -1;
}