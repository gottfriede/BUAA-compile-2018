#include "SyntaxAnalyzer.h"

ErrorMessage::ErrorMessage(int line, char type) : line(line), type(type)
{}

SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer &newLexicalAnalyzer, SymbolTableManager &newSymbolTableManager, MiddleCodeManager &newMiddleCodeManager)
    : lexicalAnalyzer(newLexicalAnalyzer), symbolTableManager(newSymbolTableManager), middleCodeManager(newMiddleCodeManager)
{
    nowFuncReturn = VOID;
    hasReturn = false;
    tmpIndex = 0;
    ifId = 0;
    switchId = 0;
    forId = 0;
    whileId = 0;
}

// ＜加法运算符＞ ::= +｜-
Operator SyntaxAnalyzer::additionOperator()
{
    Operator ret = OpAdd;
    if (word.symbol == "PLUS" || word.symbol == "MINU")
    {
        if (word.symbol == "PLUS")
            ret = OpAdd;
        else if (word.symbol == "MINU")
            ret = OpMinus;
        output(word);
        //output("<加法运算符>");
        getWord();
    }
    return ret;
}

// ＜乘法运算符＞  ::= *｜/
void SyntaxAnalyzer::multiplicationOperator()
{
    if (word.symbol == "MULT" || word.symbol == "DIV")
    {
        output(word);
        //output("<乘法运算符>");
        getWord();
    }
    else output("multiplicationOperator Error");
}

// ＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==
Operator SyntaxAnalyzer::relationalOperator()
{
    Operator ret = OpBeq;
    if (word.symbol == "LSS" || word.symbol == "LEQ" || word.symbol == "GRE" || word.symbol == "GEQ" || word.symbol == "EQL" || word.symbol == "NEQ")
    {
        if (word.symbol == "LSS")
            ret = OpBge;
        else if (word.symbol == "LEQ")
            ret = OpBgt;
        else if (word.symbol == "GEQ")
            ret = OpBlt;
        else if (word.symbol == "GRE")
            ret = OpBle;
        else if (word.symbol == "EQL")
            ret = OpBne;
        else if (word.symbol == "NEQ")
            ret = OpBeq;
        output(word);
        getWord();
    }
    return ret;
}

// ＜字母＞   ::= ＿｜a｜．．．｜z｜A｜．．．｜Z
void SyntaxAnalyzer::letter()
{
    if (word.symbol == "CHARCON" && ( isalpha(word.content.at(0)) || word.content.at((0)) == '_' ))
    {
        output(word);
        //output("<字母>");
        getWord();
    }
    else output("letter Error");
}

// ＜数字＞   ::= ０｜１｜．．．｜９
void SyntaxAnalyzer::digital()
{
    if (word.symbol == "CHARCON" && ( isdigit(word.content.at((0))) ))
    {
        output(word);
        //output("<数字>");
        getWord();
    }
    else output("digital Error");
}

// ＜字符＞    ::=  '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞'
int SyntaxAnalyzer::character()
{
    if (word.symbol == "CHARCON")
    {
        if (word.content.length() != 1)
        {
            outputError(word.line, 'a');
            output(word);
            //output("<字符>");
            getWord();
            return -1;
        }
        char tmp = word.content[0];
        if (!(tmp == '+' || tmp == '-' || tmp == '*' || tmp == '/' || tmp == '_' || (tmp >= '0' && tmp <= '9') || (tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z')))
            outputError(word.line, 'a');
        output(word);
        //output("<字符>");
        getWord();
        return tmp;
    }
    return -1;
}

//* ＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
std::string SyntaxAnalyzer::string()
{
    std::string ret;
    if (word.symbol == "STRCON")
    {
        ret = word.content;
        if (word.content.length() == 0)
        {
            outputError(word.line, 'a');
            output(word);
            output("<字符串>");
            getWord();
            return ret;
        }
        for (char tmp : word.content)
        {
            if (! (tmp == 32 || tmp == 33 || (tmp >= 35 && tmp <= 126)))
            {
                outputError(word.line, 'a');
                break;
            }
        }
        output(word);
        output("<字符串>");
        getWord();
    }
    return ret;
}

//* ＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
void SyntaxAnalyzer::program()
{
    if (word.symbol == "CONSTTK")
        constantDescription();
    if (word.symbol == "INTTK" || word.symbol == "CHARTK")
        variableDescription();
    while (word.symbol == "INTTK" || word.symbol == "CHARTK" || word.symbol == "VOIDTK")
    {
        if (word.symbol == "INTTK" || word.symbol == "CHARTK")
            functionDefinitionWithReturn(0);
        if (word.symbol == "VOIDTK")
        {
            output(word);
            getWord();
            if (word.symbol == "IDENFR")
                functionDefinitionNoReturn(1);
            else if (word.symbol == "MAINTK")
            {
                mainFunction();
                break;
            }
        }
    }
    output("<程序>");
    //symbolTableManager.outputGlobalSymbolTable();
}

//* ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
void SyntaxAnalyzer::constantDescription()
{
    bool right = false;
    while (word.symbol == "CONSTTK")
    {
        output(word);
        right = true;
        getWord();
        constantDefinition();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
        else outputError(lastWord.line, 'k');
    }
    if (right)
        output("<常量说明>");
    else
        output("constantDescription Error2");
}

//* ＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}   |   char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
void SyntaxAnalyzer::constantDefinition()
{
    std::string idName;
    if (word.symbol == "INTTK")
    {
        while (true)
        {
            output(word);
            getWord();
            idName = word.content;
            addToMap(word.content, "INT");
            if (! symbolTableManager.insert(word.content, CONST, INT))
                outputError(word.line, 'b');
            identifier(0);
            if (word.symbol == "ASSIGN")
            {
                output(word);
                getWord();
                int value = integer();
                symbolTableManager.changeVarValue(value);
                Symbol symbol = symbolTableManager.findSymbol(idName);
                middleCodeManager.addMiddleCode(OpDeclare, symbol);
                if (word.symbol != "COMMA")
                {
                    output("<常量定义>");
                    break;
                }
            }
        }
    }
    else if (word.symbol == "CHARTK")
    {
        while (true)
        {
            output(word);
            getWord();
            idName = word.content;
            addToMap(word.content, "CHAR");
            if (! symbolTableManager.insert(word.content, CONST, CHAR))
                outputError(word.line, 'b');
            identifier(0);
            if (word.symbol == "ASSIGN")
            {
                output(word);
                getWord();
                int value = character();
                symbolTableManager.changeVarValue(value);
                Symbol symbol = symbolTableManager.findSymbol(idName);
                middleCodeManager.addMiddleCode(OpDeclare, symbol);
                if (word.symbol != "COMMA")
                {
                    output("<常量定义>");
                    break;
                }
            }
        }
    }
    else output("constantDefinition Error");
}

//* ＜无符号整数＞  ::= ＜数字＞｛＜数字＞｝
int SyntaxAnalyzer::unsignedInteger()
{
    int ret = 0;
    if (word.symbol == "INTCON")
    {
        for (char i : word.content)
            ret = ret*10 + (i - '0');
        output(word);
        output("<无符号整数>");
        getWord();
    }
    return ret;
}

//* ＜整数＞        ::= ［＋｜－］＜无符号整数＞
int SyntaxAnalyzer::integer()
{
    bool negative = false;
    if (word.symbol == "PLUS" || word.symbol == "MINU")
    {
        if (word.symbol == "MINU")
            negative = true;
        output(word);
        getWord();
    }
    int ans = unsignedInteger();
    if (negative)
        ans = ans * (-1);
    output("<整数>");
    return ans;
}

// ＜标识符＞    ::=  ＜字母＞｛＜字母＞｜＜数字＞｝
std::string SyntaxAnalyzer::identifier(int mode)
{
    std::string idName = word.content;
    if (word.symbol == "IDENFR")
    {
        if (mode == 1)
        {
            if (symbolTableManager.findInAllFunc(word.content) == NOTEXISTS)
                outputError(word.line, 'c');
        }
        output(word);
        //output("<标识符>");
        getWord();
    }
    return idName;
}

//* ＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞
void SyntaxAnalyzer::descriptionHeader()
{
    if (word.symbol == "INTTK" || word.symbol == "CHARTK")
    {
        identifierBaseType baseType = (word.symbol == "INTTK") ? INT : CHAR;
        nowFuncReturn = baseType;
        hasReturn = false;
        output(word);
        getWord();
        addToMap(word.content, "RETURNFUNC");
        if (! symbolTableManager.insert(word.content, FUNC, baseType))
                outputError(word.line, 'b');
        identifier(0);
        output("<声明头部>");
    }
    else output("descriptionHeader Error");
}

//* ＜常量＞   ::=  ＜整数＞|＜字符＞
int SyntaxAnalyzer::constant()
{
    int ans;
    if (word.symbol == "CHARCON")
        ans = character();
    else ans = integer();
    output("<常量>");
    return ans;
}

//* ＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
void SyntaxAnalyzer::variableDescription()
{
    bool varDefine = false;
    Word type;
    Word id;
    while (word.symbol == "INTTK" || word.symbol == "CHARTK")
    {
        type = word;
        getWord();
        id = word;
        getWord();
        if (word.symbol == "LPARENT")
        {
            if (varDefine)
                output("<变量说明>");
            output(type);
            output(id);
            addToMap(id.content, "RETURNFUNC");
            nowFuncReturn = (type.symbol == "INTTK") ? INT : CHAR;
            hasReturn = false;
            if (type.symbol == "INTTK")
            {
                if (!symbolTableManager.insert(id.content, FUNC, INT))
                    outputError(word.line, 'b');
            }
            else if (! symbolTableManager.insert(id.content, FUNC, CHAR))
                outputError(word.line, 'b');
            output("<声明头部>");
            functionDefinitionWithReturn(1);
            return;
        }
        varDefine = true;
        output(type);
        output(id);
        if (type.symbol == "INTTK")
        {
            if (! symbolTableManager.insert(id.content, VAR, INT))
                outputError(word.line, 'b');
        }
        else if (! symbolTableManager.insert(id.content, VAR, CHAR))
            outputError(word.line, 'b');
        variableDefinition(type);
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
        else outputError(lastWord.line, 'k');
    }
    output("<变量说明>");
}

//* ＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
void SyntaxAnalyzer::variableDefinition(Word type)
{
    bool assign = false;
    identifierBaseType baseType = symbolTableManager.findClosetBaseType();
    while (true)
    {
        int dim = 0, dim1 = 1, dim2 = 1;
        if (word.symbol == "LBRACK")
        {
            dim = 1;
            output(word);
            getWord();
            dim1 = unsignedInteger();
            if (word.symbol != "RBRACK")
                outputError(word.line, 'm');
            else {
                output(word);
                getWord();
            }
            if (word.symbol == "LBRACK")
            {
                dim = 2;
                output(word);
                getWord();
                dim2 = unsignedInteger();
                if (word.symbol != "RBRACK")
                    outputError(word.line, 'm');
                else {
                    output(word);
                    getWord();
                }
            }
            symbolTableManager.changeVarType(dim, dim1, dim2);
        }
        if (word.symbol == "ASSIGN")
        {
            assign = true;
            output(word);
            getWord();
            int tmpDim1 = 1;
            int tmpDim2 = 1;
            if (word.symbol == "LBRACE")
            {
                output(word);
                getWord();
                if (word.symbol == "LBRACE")
                {
                    tmpDim1 = 0;
                    tmpDim2 = 0;
                    int tmp = 0;
                    while (true)
                    {
                        if (word.symbol == "LBRACE")
                        {
                            tmpDim1 ++;
                            tmp = 0;
                            output(word);
                            getWord();
                            while (true)
                            {
                                tmp ++;
                                int tmpValue = constant();
                                Symbol arrayInitIndex("18373584arrayInitIndex", (tmpDim1-1)*(symbolTableManager.findClosetSymbol().dim2)+tmp-1);
                                Symbol arrayInitValue("18373584arrayInitValue", tmpValue);
                                middleCodeManager.addMiddleCode(OpArrayInit, symbolTableManager.findClosetSymbol(), arrayInitIndex, arrayInitValue);
                                if (word.symbol == "COMMA")
                                {
                                    output(word);
                                    getWord();
                                }
                                else break;
                            }
                            if (word.symbol == "RBRACE")
                            {
                                output(word);
                                getWord();
                            }
                            if (tmpDim2 == 0)
                                tmpDim2 = tmp;
                            else if (tmp != tmpDim2)
                                outputError(word.line, 'n');
                            if (word.symbol == "COMMA")
                            {
                                output(word);
                                getWord();
                            }
                            else break;
                        }
                    }
                }
                else {
                    tmpDim1 = 0;
                    tmpDim2 = 1;
                    while (true)
                    {
                        int tmpValue = constant();
                        Symbol arrayInitIndex("18373584arrayInitIndex", (tmpDim2-1)*(symbolTableManager.findClosetSymbol().dim1)+tmpDim1);
                        Symbol arrayInitValue("18373584arrayInitValue", tmpValue);
                        middleCodeManager.addMiddleCode(OpArrayInit, symbolTableManager.findClosetSymbol(), arrayInitIndex, arrayInitValue);
                        tmpDim1 ++;
                        if (word.symbol == "COMMA")
                        {
                            output(word);
                            getWord();
                        }
                        else break;
                    }
                }
                if (word.symbol == "RBRACE")
                {
                    output(word);
                    getWord();
                }
            }
            else
            {
                tmpDim1 = 1;
                tmpDim2 = 1;
                if (word.symbol == "CHARCON" && baseType == INT)
                    outputError(word.line, 'o');
                if ((word.symbol == "PLUS" || word.symbol == "MINUS" || word.symbol == "INTCON") && baseType == CHAR)
                    outputError(word.line, 'o');
                int value = constant();
                symbolTableManager.changeVarValue(value);
            }
            if (dim1 != tmpDim1 || dim2 != tmpDim2)
                outputError(word.line, 'n');
        }
        middleCodeManager.addMiddleCode(OpDeclare, symbolTableManager.findClosetSymbol());
        if (word.symbol == "COMMA")
        {
            std::string idName;
            output(word);
            getWord();
            idName = word.content;
            if (type.symbol == "INTTK")
            {
                if (! symbolTableManager.insert(word.content, VAR, INT))
                    outputError(word.line, 'b');
            }
            else
            {
                if (! symbolTableManager.insert(word.content, VAR, CHAR))
                    outputError(word.line, 'b');
            }
            identifier(0);
        }
        else break;
    }
    if (assign)
        output("<变量定义及初始化>");
    else
        output("<变量定义无初始化>");
    output("<变量定义>");
}

//* ＜变量定义无初始化＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'){,(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
void SyntaxAnalyzer::variableDefinitionNoInitial()
{
}

//* ＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞|＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'|＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
void SyntaxAnalyzer::variableDefinitionWithInitial()
{
}

// ＜类型标识符＞      ::=  int | char
void SyntaxAnalyzer::typeIdentifier()
{
    if (word.symbol == "INTTK" || word.symbol == "CHARTK")
    {
        output(word);
        //output("<类型标识符>");
        getWord();
    }
}

//* ＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
void SyntaxAnalyzer::functionDefinitionWithReturn(int mode)
{
    if (mode == 0)
        descriptionHeader();
    if (word.symbol == "LPARENT")
    {
        output(word);
        getWord();
        parameterTable();
        if (word.symbol != "RPARENT")
            outputError(word.line, 'l');
        else {
            output(word);
            getWord();
        }
        if (word.symbol == "LBRACE")
        {
            output(word);
            getWord();
            compoundStatement();
            if (word.symbol == "RBRACE")
            {
                if (! hasReturn)
                    outputError(word.line, 'h');
                output(word);
                getWord();
            }
        }
    }
    symbolTableManager.clearClosetFunc();
    output("<有返回值函数定义>");
}

//* ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
void SyntaxAnalyzer::functionDefinitionNoReturn(int mode)
{
    if ((word.symbol == "VOIDTK" && mode == 0) || (word.symbol == "IDENFR" && mode == 1))
    {
        if (mode == 0)
        {
            output(word);
            getWord();
        }
        addToMap(word.content, "NORETURNFUNC");
        nowFuncReturn = VOID;
        if (! symbolTableManager.insert(word.content, FUNC, VOID))
            outputError(word.line, 'b');
        identifier(0);
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            parameterTable();
            if (word.symbol != "RPARENT")
                outputError(word.line, 'l');
            else {
                output(word);
                getWord();
            }
            if (word.symbol == "LBRACE")
            {
                output(word);
                getWord();
                compoundStatement();
                if (word.symbol == "RBRACE")
                {
                    output(word);
                    getWord();
                }
            }
        }
        symbolTableManager.clearClosetFunc();
        Symbol nullSymbol("18373584null", 0);
        middleCodeManager.addMiddleCode(OpFuncReturn, nullSymbol);
        output("<无返回值函数定义>");
    }
}

//* ＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
void SyntaxAnalyzer::compoundStatement()
{
    if (word.symbol == "CONSTTK")
        constantDescription();
    if (word.symbol == "INTTK" || word.symbol == "CHARTK")
        variableDescription();
    statementColumn();
    output("<复合语句>");
}

//* ＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
void SyntaxAnalyzer::parameterTable()
{
    while (word.symbol == "INTTK" || word.symbol == "CHARTK")
    {
        if (word.symbol == "INTTK")
        {
            typeIdentifier();
            if (! symbolTableManager.insert(word.content, PARAM, INT))
                outputError(word.line, 'b');
        }
        else
        {
            typeIdentifier();
            if (! symbolTableManager.insert(word.content, PARAM, CHAR))
                outputError(word.line, 'b');
        }
        identifier(0);
        if (word.symbol == "COMMA")
        {
            output(word);
            getWord();
        }
    }
    Symbol func = symbolTableManager.getNowFunc();
    middleCodeManager.addMiddleCode(OpFuncDeclare, func);
    output("<参数表>");
}

//* ＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
void SyntaxAnalyzer::mainFunction()
{
        if (word.symbol == "MAINTK")
        {
            nowFuncReturn = VOID;
            if (! symbolTableManager.insert("main", FUNC, VOID))
                outputError(word.line, 'b');
            output(word);
            getWord();
            if (word.symbol == "LPARENT")
            {
                output(word);
                getWord();
                if (word.symbol != "RPARENT")
                    outputError(word.line, 'l');
                else {
                    output(word);
                    getWord();
                }
                if (word.symbol == "LBRACE")
                {
                    middleCodeManager.addMiddleCode(OpFuncDeclare, Symbol("main",FUNC,VOID));
                    output(word);
                    getWord();
                    compoundStatement();
                    if (word.symbol == "RBRACE")
                    {
                        output(word);
                        getWord();
                    }
                }
            }
        }
        symbolTableManager.clearClosetFunc();
        output("<主函数>");
}

//* ＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
identifierBaseType SyntaxAnalyzer::expression()
{
    bool negative = false;
    identifierBaseType baseType = INT;
    bool baseTypeInt = false;
    if (word.symbol == "PLUS" || word.symbol == "MINU")
    {
        if (word.symbol == "MINU")
            negative = true;
        baseTypeInt = true;
        output(word);
        getWord();
    }
    baseType = item();
    Symbol symbol1 = nowSymbol;
    if (negative)
    {
        if (nowSymbol.name == "18373584CONSTINT" || nowSymbol.name == "18373584CONSTCHAR")
        {
            nowSymbol.value = -1 * nowSymbol.value;
            symbol1 = nowSymbol;
        }
        else
        {
            Symbol newSymbol(newSymbolName(), VAR, INT);
            middleCodeManager.addMiddleCode(OpMulti, nowSymbol, Symbol("18373584CONSTINT", -1), newSymbol);
            symbol1 = newSymbol;
        }
    }
    nowSymbol = symbol1;
    while (word.symbol == "PLUS" || word.symbol == "MINU")
    {
        Operator op = (word.symbol == "PLUS") ? OpAdd : OpMinus;
        baseTypeInt = true;
        additionOperator();
        baseType = item();
        Symbol symbol2 = nowSymbol;
        Symbol symbol3(newSymbolName(), VAR, INT);
        middleCodeManager.addMiddleCode(op, symbol1, symbol2, symbol3);
        nowSymbol = symbol3;
        symbol1 = symbol3;
    }
    output("<表达式>");
    if (baseTypeInt)
        return INT;
    return baseType;
}

//* ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
identifierBaseType SyntaxAnalyzer::item()
{
    identifierBaseType baseType = INT;
    bool baseTypeInt = false;
    baseType = factor();
    Symbol symbol1 = nowSymbol;
    while (word.symbol == "MULT" || word.symbol == "DIV")
    {
        Operator op = (word.symbol == "MULT") ? OpMulti : OpDiv;
        baseTypeInt = true;
        multiplicationOperator();
        baseType = factor();
        Symbol symbol2 = nowSymbol;
        Symbol symbol3(newSymbolName(), VAR, INT);
        middleCodeManager.addMiddleCode(op, symbol1, symbol2, symbol3);
        nowSymbol = symbol3;
        symbol1 = symbol3;
    }
    output("<项>");
    if (baseTypeInt)
        return INT;
    return baseType;
}

//* ＜因子＞    ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'|＜标识符＞'['＜表达式＞']''['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
identifierBaseType SyntaxAnalyzer::factor()
{
    identifierBaseType baseType = INT;
    if (word.symbol == "IDENFR" && idMap.find(word.content) != idMap.end() && idMap.find(word.content)->second == "RETURNFUNC")
    {
        if (symbolTableManager.findBaseInAllFunc(word.content) == CHAR)
            baseType = CHAR;
        functionCallWithReturn();
    }
    else if (word.symbol == "CHARCON")
    {
        baseType = CHAR;
        int value = character();
        Symbol symbol1("18373584CONSTCHAR", value);
        nowSymbol = symbol1;
    }
    else if (word.symbol == "INTCON" || word.symbol == "PLUS" || word.symbol == "MINU")
    {
        int value = integer();
        Symbol symbol1("18373584CONSTINT", value);
        nowSymbol = symbol1;
    }
    else if (word.symbol == "LPARENT")
    {
        output(word);
        getWord();
        expression();
        if (nowSymbol.name == "18373584CONSTCHAR")
        {
            nowSymbol.name = "18373584CONSTINT";
            nowSymbol.baseType = INT;
        }
        if (word.symbol != "RPARENT")
            outputError(word.line, 'l');
        else {
            output(word);
            getWord();
        }
    }
    else if (word.symbol == "IDENFR")
    {
        if (symbolTableManager.findBaseInAllFunc(word.content) == CHAR)
            baseType = CHAR;
        std::string idName = identifier(1);
        nowSymbol = symbolTableManager.findSymbol(idName);
        if (word.symbol == "LBRACK")
        {
            output(word);
            getWord();
            identifierBaseType expressionBaseType = expression();
            Symbol dim1Symbol = nowSymbol;
            Symbol dim2Symbol("18373584CONSTINT", 0);
            if (expressionBaseType == CHAR)
                outputError(word.line, 'i');
            if (word.symbol != "RBRACK")
                outputError(word.line, 'm');
            else {
                output(word);
                getWord();
            }
            if (word.symbol == "LBRACK")
            {
                output(word);
                getWord();
                expressionBaseType = expression();
                dim2Symbol = nowSymbol;
                if (expressionBaseType == CHAR)
                    outputError(word.line, 'i');
                if (word.symbol != "RBRACK")
                    outputError(word.line, 'm');
                else {
                    output(word);
                    getWord();
                }
            }
            Symbol newSymbol4(newSymbolName(), VAR, symbolTableManager.findSymbol(idName).baseType);
            if (symbolTableManager.findSymbol(idName).dim2 == 1)
            {
                middleCodeManager.addMiddleCode(OpArrayGet, symbolTableManager.findSymbol(idName), dim1Symbol, newSymbol4);
            }
            else {
                Symbol arrayDim2("18373584CONSTINT", symbolTableManager.findSymbol(idName).dim2);
                Symbol newSymbol2(newSymbolName(), VAR, INT);
                middleCodeManager.addMiddleCode(OpMulti, dim1Symbol, arrayDim2, newSymbol2);
                Symbol newSymbol3(newSymbolName(), VAR, INT);
                middleCodeManager.addMiddleCode(OpAdd, newSymbol2, dim2Symbol, newSymbol3);
                middleCodeManager.addMiddleCode(OpArrayGet, symbolTableManager.findSymbol(idName), newSymbol3, newSymbol4);
            }
            nowSymbol = newSymbol4;
        }
    }
    output("<因子>");
    return baseType;
}

//* ＜语句＞    ::= ＜循环语句＞｜＜条件语句＞| ＜有返回值函数调用语句＞;  |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜情况语句＞｜＜空＞;|＜返回语句＞; | '{'＜语句列＞'}'
void SyntaxAnalyzer::statement()
{
    if (word.symbol == "WHILETK" || word.symbol == "FORTK")
        loopStatement();
    else if (word.symbol == "IFTK")
        conditionStatement();
    else if (word.symbol == "IDENFR" && idMap.find(word.content) != idMap.end() && idMap.find(word.content)->second == "RETURNFUNC")
    {
        functionCallWithReturn();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
        else outputError(lastWord.line, 'k');
    }
    else if (word.symbol == "IDENFR" && idMap.find(word.content) != idMap.end() && idMap.find(word.content)->second == "NORETURNFUNC")
    {
        functionCallNoReturn();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
        else outputError(lastWord.line, 'k');
    }
    else if (word.symbol == "IDENFR")
    {
        Word idenfr = word;
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            outputError(word.line, 'c');
            functionCallNoDefine(idenfr);
        }
        else assignStatement(idenfr.content);
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
        else outputError(lastWord.line, 'k');
    }
    else if (word.symbol == "SCANFTK")
    {
        readStatement();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
        else outputError(lastWord.line, 'k');
    }
    else if (word.symbol == "PRINTFTK")
    {
        writeStatement();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
        else outputError(lastWord.line, 'k');
    }
    else if (word.symbol == "SWITCHTK")
        caseStatement();
    else if (word.symbol == "RETURNTK")
    {
        returnStatement();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
        else outputError(lastWord.line, 'k');
    }
    else if (word.symbol == "LBRACE")
    {
        output(word);
        getWord();
        statementColumn();
        if (word.symbol == "RBRACE")
        {
            output(word);
            getWord();
        }
    }
    else if (word.symbol == "SEMICN")
    {
        output(word);
        getWord();
    }
    else outputError(lastWord.line, 'k');
    output("<语句>");
}

//* ＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞|＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
void SyntaxAnalyzer::assignStatement(std::string idName)
{
    if (symbolTableManager.findInAllFunc(idName) == CONST)
        outputError(word.line, 'j');
    if (symbolTableManager.findInAllFunc(idName) == NOTEXISTS)
        outputError(word.line, 'c');
    if (word.symbol == "ASSIGN")
    {
        output(word);
        getWord();
        expression();
        Symbol symbol1 = symbolTableManager.findSymbol(idName);
        Symbol symbol2 = nowSymbol;
        middleCodeManager.addMiddleCode(OpAssign, symbol1, symbol2);
    }
    else if (word.symbol == "LBRACK")
    {
        Symbol expressionSymbol("18373584null", NOTEXISTS, NOEXISTS);
        output(word);
        getWord();
        identifierBaseType baseType = expression();
        Symbol dim1Symbol = nowSymbol;
        Symbol dim2Symbol("18373584CONSTINT", 0);
        if (baseType == CHAR)
            outputError(word.line, 'i');
        if (word.symbol != "RBRACK")
            outputError(word.line, 'm');
        else {
            output(word);
            getWord();
        }
        if (word.symbol == "ASSIGN")
        {
            output(word);
            getWord();
            expression();
            expressionSymbol = nowSymbol;
        }
        else if (word.symbol == "LBRACK")
        {
            output(word);
            getWord();
            identifierBaseType baseType = expression();
            dim2Symbol = nowSymbol;
            if (baseType == CHAR)
                outputError(word.line, 'i');
            if (word.symbol != "RBRACK")
                outputError(word.line, 'm');
            else {
                output(word);
                getWord();
            }
            if (word.symbol == "ASSIGN") {
                output(word);
                getWord();
                expression();
                expressionSymbol = nowSymbol;
            }
        }
        Symbol idSymbol = symbolTableManager.findSymbol(idName);
        Symbol arrayDim2(newSymbolName(), idSymbol.dim2);
        if (idSymbol.dim2 == 1)
        {
            middleCodeManager.addMiddleCode(OpArrayPush, idSymbol, dim1Symbol, expressionSymbol);
        }
        else {
            Symbol newSymbol2(newSymbolName(), VAR, INT);
            middleCodeManager.addMiddleCode(OpMulti, dim1Symbol, arrayDim2, newSymbol2);
            Symbol newSymbol3(newSymbolName(), VAR, INT);
            middleCodeManager.addMiddleCode(OpAdd, newSymbol2, dim2Symbol, newSymbol3);
            middleCodeManager.addMiddleCode(OpArrayPush, idSymbol, newSymbol3, expressionSymbol);
        }
    }
    output("<赋值语句>");
}

//* ＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
void SyntaxAnalyzer::conditionStatement()
{
    if (word.symbol == "IFTK")
    {
        ifId ++;
        int nowIfId = ifId;
        Symbol ifBeginLabel("if_" + std::to_string(nowIfId) + "_begin", 0);
        middleCodeManager.addMiddleCode(OpLabel, ifBeginLabel);
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            condition(nowIfId, "if");
            if (word.symbol != "RPARENT")
                outputError(word.line, 'l');
            else {
                output(word);
                getWord();
            }
            statement();
            Symbol ifEnd("if_" + std::to_string(nowIfId) + "_end", 0);
            middleCodeManager.addMiddleCode(OpJ, ifEnd);
            Symbol ifElse("if_" + std::to_string(nowIfId) + "_else", 0);
            middleCodeManager.addMiddleCode(OpLabel, ifElse);
            if (word.symbol == "ELSETK")
            {
                output(word);
                getWord();
                statement();
            }
            middleCodeManager.addMiddleCode(OpLabel, ifEnd);
        }
        output("<条件语句>");
    }
}

//* ＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
void SyntaxAnalyzer::condition(int id, std::string name)
{
    identifierBaseType expressionBaseType = expression();
    Symbol symbol1 = nowSymbol;
    if (expressionBaseType == CHAR)
        outputError(word.line, 'f');
    Operator op = relationalOperator();
    expressionBaseType = expression();
    Symbol symbol2 = nowSymbol;
    if (expressionBaseType == CHAR)
        outputError(word.line, 'f');
    if (name == "if")
    {
        Symbol ifElse("if_" + std::to_string(id) + "_else", 0);
        middleCodeManager.addMiddleCode(op, symbol1, symbol2, ifElse);
    }
    if (name == "while")
    {
        Symbol whileEnd("while_" + std::to_string(id) + "_end", 0);
        middleCodeManager.addMiddleCode(op, symbol1, symbol2, whileEnd);
    }
    if (name == "for")
    {
        Symbol forEnd("for_" + std::to_string(id) + "_end", 0);
        middleCodeManager.addMiddleCode(op, symbol1, symbol2, forEnd);
    }
    output("<条件>");
}

//* ＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞| for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
void SyntaxAnalyzer::loopStatement()
{
    if (word.symbol == "WHILETK")
    {
        whileId ++;
        int nowWhileId = whileId;
        Symbol whileBeginSymbol("while_" + std::to_string(nowWhileId) + "_begin", 0);
        middleCodeManager.addMiddleCode(OpLabel, whileBeginSymbol);
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            condition(whileId, "while");
            if (word.symbol != "RPARENT")
                outputError(word.line, 'l');
            else {
                output(word);
                getWord();
            }
            statement();
            middleCodeManager.addMiddleCode(OpJ, whileBeginSymbol);
        }
        Symbol whileEndSymbol("while_" + std::to_string(nowWhileId) + "_end", 0);
        middleCodeManager.addMiddleCode(OpLabel, whileEndSymbol);
        output("<循环语句>");
    }
    else if (word.symbol == "FORTK")
    {
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            if (symbolTableManager.findInAllFunc(word.content) == CONST)
                outputError(word.line, 'j');
            std::string idNameInit = identifier(1);
            Symbol initSymbol = symbolTableManager.findSymbol(idNameInit);
            if (word.symbol == "ASSIGN")
            {
                output(word);
                getWord();
                expression();
                Symbol assignSymbol = nowSymbol;
                middleCodeManager.addMiddleCode(OpAssign, initSymbol, assignSymbol);
                if (word.symbol != "SEMICN")
                    outputError(lastWord.line, 'k');
                else {
                    output(word);
                    getWord();
                }
                forId ++;
                int nowForId = forId;
                Symbol forBeginLabel("for_" + std::to_string(nowForId) + "_begin", 0);
                middleCodeManager.addMiddleCode(OpLabel, forBeginLabel);
                condition(forId, "for");
                if (word.symbol != "SEMICN")
                    outputError(lastWord.line, 'k');
                else {
                    output(word);
                    getWord();
                }
                std::string id1Name = identifier(1);
                if (word.symbol == "ASSIGN")
                {
                    output(word);
                    getWord();
                    std::string id2Name = identifier(1);
                    Operator op = additionOperator();
                    int step = stepSize();
                    if (word.symbol != "RPARENT")
                        outputError(word.line, 'l');
                    else {
                        output(word);
                        getWord();
                    }
                    statement();
                    Symbol symbol1 = symbolTableManager.findSymbol(id1Name);
                    Symbol symbol2 = symbolTableManager.findSymbol(id2Name);
                    Symbol constSymbol3("18373584CONSTINT", step);
                    middleCodeManager.addMiddleCode(op, symbol2, constSymbol3, symbol1);
                    middleCodeManager.addMiddleCode(OpJ, forBeginLabel);
                    Symbol forEndSymbol("for_" + std::to_string(nowForId) + "_end", 0);
                    middleCodeManager.addMiddleCode(OpLabel, forEndSymbol);
                }
            }
        }
    }
    output("<循环语句>");
}

//* ＜步长＞::= ＜无符号整数＞
int SyntaxAnalyzer::stepSize()
{
    int ret = unsignedInteger();
    output("<步长>");
    return ret;
}

//* ＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
void SyntaxAnalyzer::caseStatement()
{
    if (word.symbol == "SWITCHTK")
    {
        switchId ++;
        int nowSwitchId = switchId;
        Symbol switchBegin("switch_" + std::to_string(nowSwitchId) + "_begin", 0);
        middleCodeManager.addMiddleCode(OpLabel, switchBegin);
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            identifierBaseType expressionBaseType = expression();
            Symbol symbol1 = nowSymbol;
            if (word.symbol != "RPARENT")
                outputError(word.line, 'l');
            else {
                output(word);
                getWord();
            }
            if (word.symbol == "LBRACE")
            {
                output(word);
                getWord();
                int caseNumber = caseTable(expressionBaseType, symbol1, nowSwitchId);
                caseDefault(nowSwitchId, caseNumber);
                if (word.symbol == "RBRACE")
                {
                    output(word);
                    getWord();
                }
            }
        }
        Symbol switchEnd("switch_" + std::to_string(nowSwitchId) + "_end", 0);
        middleCodeManager.addMiddleCode(OpLabel, switchEnd);
        output("<情况语句>");
    }
}

//* ＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
int SyntaxAnalyzer::caseTable(identifierBaseType expressionBaseType, Symbol expressionSymbol, int id)
{
    int caseId = 0;
    while (word.symbol == "CASETK")
    {
        caseId ++;
        caseChildrenStatement(expressionBaseType, expressionSymbol, id, caseId);
    }
    output("<情况表>");
    return caseId+1;
}

//* ＜情况子语句＞  ::=  case＜常量＞：＜语句＞
void SyntaxAnalyzer::caseChildrenStatement(identifierBaseType expressionBaseType, Symbol expressionSymbol, int id, int id2)
{
    if (word.symbol == "CASETK")
    {
        Symbol caseBegin("switch_" + std::to_string(id) + "_case_" + std::to_string(id2), 0);
        middleCodeManager.addMiddleCode(OpLabel, caseBegin);
        output(word);
        getWord();
        if (word.symbol == "CHARCON" && expressionBaseType == INT)
            outputError(word.line, 'o');
        if ((word.symbol == "INTCON" || word.symbol == "PLUS" || word.symbol == "MINUS") && expressionBaseType == CHAR)
            outputError(word.line, 'o');
        std::string constBaseType = (word.symbol == "INTCON") ? "18373584CONSTINT" : "18373584CONSTCHAR";
        int constValue = constant();
        Symbol constSymbol(constBaseType, constValue);
        Symbol nextLabel("switch_" + std::to_string(id) + "_case_" + std::to_string(id2+1), 0);
        middleCodeManager.addMiddleCode(OpBne, expressionSymbol, constSymbol, nextLabel);
        if (word.symbol == "COLON")
        {
            output(word);
            getWord();
            statement();
        }
        Symbol switchEnd("switch_" + std::to_string(id) + "_end", 0);
        middleCodeManager.addMiddleCode(OpJ, switchEnd);
        output("<情况子语句>");
    }
}

//* ＜缺省＞   ::=  default :＜语句＞
void SyntaxAnalyzer::caseDefault(int id, int id2)
{
    if (word.symbol == "DEFAULTTK")
    {
        Symbol newLabel("switch_" + std::to_string(id) + "_case_" + std::to_string(id2), 0);
        middleCodeManager.addMiddleCode(OpLabel, newLabel);
        output(word);
        getWord();
        if (word.symbol == "COLON")
        {
            output(word);
            getWord();
            statement();
        }
    }
    else outputError(word.line, 'p');
    output("<缺省>");
}

//* ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
void SyntaxAnalyzer::functionCallWithReturn()
{
    std::string funcName = word.content;
    Symbol funcSymbol = symbolTableManager.findSymbol(funcName);
    identifier(1);
    if (word.symbol == "LPARENT")
    {
        output(word);
        getWord();
        valueParameterTable(funcName);
        if (word.symbol != "RPARENT")
            outputError(word.line, 'l');
        else {
            output(word);
            getWord();
        }
    }
    Symbol tmpSymbol(newSymbolName(), VAR, funcSymbol.baseType);
    middleCodeManager.addMiddleCode(OpFuncCall, funcSymbol, tmpSymbol);
    nowSymbol = tmpSymbol;
    output("<有返回值函数调用语句>");
}

//* ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
void SyntaxAnalyzer::functionCallNoReturn()
{
    std::string funcName = word.content;
    identifier(1);
    if (word.symbol == "LPARENT")
    {
        output(word);
        getWord();
        valueParameterTable(funcName);
        if (word.symbol != "RPARENT")
            outputError(word.line, 'l');
        else {
            output(word);
            getWord();
        }
    }
    middleCodeManager.addMiddleCode(OpFuncCall, symbolTableManager.findSymbol(funcName));
    output("<无返回值函数调用语句>");
}

void SyntaxAnalyzer::functionCallNoDefine(Word func)
{
    if (word.symbol == "LPARENT")
    {
        output(word);
        getWord();
        valueParameterTable(func.content);
        if (word.symbol != "RPARENT")
            outputError(word.line, 'l');
        else {
            output(word);
            getWord();
        }
    }
}

//* ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
void SyntaxAnalyzer::valueParameterTable(std::string funcName)
{
    std::vector<identifierBaseType> callVector;
    std::vector<Symbol> funcParams = symbolTableManager.getFuncParams(funcName);
    if (word.symbol == "PLUS" || word.symbol == "MINU" || word.symbol == "IDENFR" || word.symbol == "LPARENT" || word.symbol == "CHARCON" || word.symbol == "INTCON")
    {
        while (true)
        {
            identifierBaseType expressionType = expression();
            Symbol paramSymbol = nowSymbol;
            middleCodeManager.addMiddleCode(OpPushParam, paramSymbol);
            callVector.emplace_back(expressionType);
            if (word.symbol == "COMMA")
            {
                output(word);
                getWord();
            }
            else break;
        }
    }
    if (symbolTableManager.findInAllFunc(funcName) != NOTEXISTS)
    {
        if (callVector.size() != funcParams.size())
            outputError(word.line, 'd');
        std::vector<identifierBaseType>::iterator it;
        std::vector<Symbol>::iterator it2;
        for (it = callVector.begin(), it2 = funcParams.begin(); it != callVector.end() && it2 != funcParams.end(); it ++, it2 ++)
            if ((*it) != (*it2).baseType)
            {
                outputError(word.line, 'e');
                break;
            }
    }
    output("<值参数表>");
}

//* ＜语句列＞   ::= ｛＜语句＞｝
void SyntaxAnalyzer::statementColumn()
{
    while (word.symbol == "WHILETK" || word.symbol == "FORTK" || word.symbol == "IFTK" ||
    word.symbol == "IDENFR" || word.symbol == "SCANFTK" || word.symbol == "PRINTFTK" ||
    word.symbol == "SWITCHTK" || word.symbol == "RETURNTK" || word.symbol == "LBRACE" ||
    word.symbol == "SEMICN")
        statement();
    output("<语句列>");
}

//* ＜读语句＞    ::=  scanf '('＜标识符＞')'
void SyntaxAnalyzer::readStatement()
{
    if (word.symbol == "SCANFTK")
    {
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            if (symbolTableManager.findInAllFunc(word.content) == CONST)
                outputError(word.line, 'j');
            std::string idName = identifier(1);
            Symbol symbol = symbolTableManager.findSymbol(idName);
            middleCodeManager.addMiddleCode(OpScan, symbol);
            if (word.symbol != "RPARENT")
                outputError(word.line, 'l');
            else {
                output(word);
                getWord();
            }
        }
        output("<读语句>");
    }
}

//* ＜写语句＞    ::= printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')'
void SyntaxAnalyzer::writeStatement()
{
    if (word.symbol == "PRINTFTK")
    {
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            if (word.symbol == "STRCON")
            {
                std::string str = string();
                int index = middleCodeManager.addStr(str);
                middleCodeManager.addMiddleCode(OpPrint, Symbol("string", index), Symbol("string", index));
                if (word.symbol == "COMMA")
                {
                    output(word);
                    getWord();
                    expression();
                    middleCodeManager.addMiddleCode(OpPrint, Symbol("expression", 0), nowSymbol);
                    if (word.symbol != "RPARENT")
                        outputError(word.line, 'l');
                    else {
                        output(word);
                        getWord();
                    }
                    middleCodeManager.addMiddleCode(OpPrint, Symbol("string", 0), Symbol("string", 0));
                    return;
                }
                if (word.symbol != "RPARENT")
                    outputError(word.line, 'l');
                else {
                    output(word);
                    getWord();
                }
            }
            else
            {
                expression();
                middleCodeManager.addMiddleCode(OpPrint, Symbol("expression", 0), nowSymbol);
                if (word.symbol != "RPARENT")
                    outputError(word.line, 'l');
                else {
                    output(word);
                    getWord();
                }
            }
        }
        middleCodeManager.addMiddleCode(OpPrint, Symbol("string", 0), Symbol("string", 0));
        output("<写语句>");
    }
}

//* ＜返回语句＞   ::=  return['('＜表达式＞')']
void SyntaxAnalyzer::returnStatement()
{
    identifierBaseType returnBase = VOID;
    hasReturn = true;
    if (word.symbol == "RETURNTK")
    {
        Symbol retSymbol("18373584null", 0);
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            returnBase = expression();
            retSymbol = nowSymbol;
            if (returnBase != nowFuncReturn && nowFuncReturn == VOID)
                outputError(word.line, 'g');
            else if (returnBase != nowFuncReturn)
                outputError(word.line, 'h');
            if (word.symbol != "RPARENT")
                outputError(word.line, 'l');
            else {
                output(word);
                getWord();
            }
        }
        middleCodeManager.addMiddleCode(OpFuncReturn, retSymbol);
        output("<返回语句>");
    }
    if (returnBase == VOID && (nowFuncReturn == INT || nowFuncReturn == CHAR))
        outputError(word.line, 'h');
}

void SyntaxAnalyzer::output(std::string syntaxContent)
{
    //std::cout << syntaxContent << std::endl;
    outputFile << syntaxContent << std::endl;
}

void SyntaxAnalyzer::output(Word outputWord)
{
    //std::cout << outputWord.symbol << " " << outputWord.content << std::endl;
    outputFile << outputWord.symbol << " " << outputWord.content << " " << outputWord.line << std::endl;
}

void SyntaxAnalyzer::getWord()
{
    lastWord = word;
    word = lexicalAnalyzer.analyze();
}

void SyntaxAnalyzer::addToMap(std::string id, std::string type)
{
    idMap.insert(std::pair<std::string, std::string>(id, type));
}

void SyntaxAnalyzer::outputError(int line, char errorType)
{
    //errorFile << line << " " << errorType << std::endl;
    errorList.emplace_back(line, errorType);
}

std::string SyntaxAnalyzer::newSymbolName()
{
    std::string ret = "18373584tmp";
    ret.append(std::to_string(tmpIndex));
    tmpIndex ++;
    return ret;
}

void SyntaxAnalyzer::analyze()
{
    outputFile.open("syntaxOutput.txt");
    getWord();
    program();
    outputFile.close();
    if (errorList.empty())
    {
        middleCodeManager.beginGenerate();
    }
    else
    {
        errorFile.open("error.txt");
        std::vector<ErrorMessage>::iterator it;
        for (it = errorList.begin(); it != errorList.end(); it ++)
        {
            errorFile << (*it).line << " " << (*it).type << std::endl;
        }
        errorFile.close();
    }
}