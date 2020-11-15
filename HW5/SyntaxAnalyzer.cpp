#include "SyntaxAnalyzer.h"

ErrorMessage::ErrorMessage(int line, char type) : line(line), type(type)
{}

SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer &newLexicalAnalyzer, SymbolTableManager &newSymbolTableManager, MiddleCodeManager &newMiddleCodeManager)
    : lexicalAnalyzer(newLexicalAnalyzer), symbolTableManager(newSymbolTableManager), middleCodeManager(newMiddleCodeManager)
{
    nowFuncReturn = VOID;
    hasReturn = false;
    tmpIndex = 0;
}

// ＜加法运算符＞ ::= +｜-
void SyntaxAnalyzer::additionOperator()
{
    if (word.symbol == "PLUS" || word.symbol == "MINU")
    {
        output(word);
        //output("<加法运算符>");
        getWord();
    }
    else output("additionOperator Error");
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
void SyntaxAnalyzer::relationalOperator()
{
    if (word.symbol == "LSS" || word.symbol == "LEQ" || word.symbol == "GRE" || word.symbol == "GEQ" || word.symbol == "EQL" || word.symbol == "NEQ")
    {
        output(word);
        //output("<关系运算符>");
        getWord();
    }
    else output("relationalOperator Error");
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
                                constant();
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
                        constant();
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
    output("<参数表>");
}

//* ＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
void SyntaxAnalyzer::mainFunction()
{
        if (word.symbol == "MAINTK")
        {
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
                    middleCodeManager.addMiddleCode(OpLabel, Symbol("main",FUNC,VOID));
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
    if (word.symbol == "PLUS" || word.symbol == "MINU")
    {
        if (word.symbol == "MINU")
            negative = true;
        baseType = INT;
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
        baseType = INT;
        additionOperator();
        baseType = item();
        Symbol symbol2 = nowSymbol;
        Symbol symbol3(newSymbolName(), VAR, INT);
        middleCodeManager.addMiddleCode(op, symbol1, symbol2, symbol3);
        nowSymbol = symbol3;
        symbol1 = symbol3;
    }
    output("<表达式>");
    return baseType;
}

//* ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
identifierBaseType SyntaxAnalyzer::item()
{
    identifierBaseType baseType = INT;
    baseType = factor();
    Symbol symbol1 = nowSymbol;
    while (word.symbol == "MULT" || word.symbol == "DIV")
    {
        Operator op = (word.symbol == "MULT") ? OpMulti : OpDiv;
        baseType = INT;
        multiplicationOperator();
        baseType = factor();
        Symbol symbol2 = nowSymbol;
        Symbol symbol3(newSymbolName(), VAR, INT);
        middleCodeManager.addMiddleCode(op, symbol1, symbol2, symbol3);
        nowSymbol = symbol3;
        symbol1 = symbol3;
    }
    output("<项>");
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
                if (expressionBaseType == CHAR)
                    outputError(word.line, 'i');
                if (word.symbol != "RBRACK")
                    outputError(word.line, 'm');
                else {
                    output(word);
                    getWord();
                }
            }
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
        output(word);
        getWord();
        identifierBaseType baseType = expression();
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
        }
        else if (word.symbol == "LBRACK")
        {
            output(word);
            getWord();
            identifierBaseType baseType = expression();
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
            }
        }
    }
    output("<赋值语句>");
}

//* ＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
void SyntaxAnalyzer::conditionStatement()
{
    if (word.symbol == "IFTK")
    {
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            condition();
            if (word.symbol != "RPARENT")
                outputError(word.line, 'l');
            else {
                output(word);
                getWord();
            }
            statement();
            if (word.symbol == "ELSETK")
            {
                output(word);
                getWord();
                statement();
            }
        }
        output("<条件语句>");
    }
}

//* ＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
void SyntaxAnalyzer::condition()
{
    identifierBaseType expressionBaseType = expression();
    if (expressionBaseType == CHAR)
        outputError(word.line, 'f');
    relationalOperator();
    expressionBaseType = expression();
    if (expressionBaseType == CHAR)
        outputError(word.line, 'f');
    output("<条件>");
}

//* ＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞| for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
void SyntaxAnalyzer::loopStatement()
{
    if (word.symbol == "WHILETK")
    {
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            condition();
            if (word.symbol != "RPARENT")
                outputError(word.line, 'l');
            else {
                output(word);
                getWord();
            }
            statement();
        }
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
            identifier(1);
            if (word.symbol == "ASSIGN")
            {
                output(word);
                getWord();
                expression();
                if (word.symbol != "SEMICN")
                    outputError(lastWord.line, 'k');
                else {
                    output(word);
                    getWord();
                }
                condition();
                if (word.symbol != "SEMICN")
                    outputError(lastWord.line, 'k');
                else {
                    output(word);
                    getWord();
                }
                identifier(1);
                if (word.symbol == "ASSIGN")
                {
                    output(word);
                    getWord();
                    identifier(1);
                    additionOperator();
                    stepSize();
                    if (word.symbol != "RPARENT")
                        outputError(word.line, 'l');
                    else {
                        output(word);
                        getWord();
                    }
                    statement();
                }
            }
        }
    }
    output("<循环语句>");
}

//* ＜步长＞::= ＜无符号整数＞
void SyntaxAnalyzer::stepSize()
{
    unsignedInteger();
    output("<步长>");
}

//* ＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
void SyntaxAnalyzer::caseStatement()
{
    if (word.symbol == "SWITCHTK")
    {
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            identifierBaseType expressionBaseType = expression();
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
                caseTable(expressionBaseType);
                caseDefault();
                if (word.symbol == "RBRACE")
                {
                    output(word);
                    getWord();
                }
            }
        }
        output("<情况语句>");
    }
}

//* ＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
void SyntaxAnalyzer::caseTable(identifierBaseType expressionBaseType)
{
    while (word.symbol == "CASETK")
    {
        caseChildrenStatement(expressionBaseType);
    }
    output("<情况表>");
}

//* ＜情况子语句＞  ::=  case＜常量＞：＜语句＞
void SyntaxAnalyzer::caseChildrenStatement(identifierBaseType expressionBaseType)
{
    if (word.symbol == "CASETK")
    {
        output(word);
        getWord();
        if (word.symbol == "CHARCON" && expressionBaseType == INT)
            outputError(word.line, 'o');
        if ((word.symbol == "INTCON" || word.symbol == "PLUS" || word.symbol == "MINUS") && expressionBaseType == CHAR)
            outputError(word.line, 'o');
        constant();
        if (word.symbol == "COLON")
        {
            output(word);
            getWord();
            statement();
        }
        output("<情况子语句>");
    }
}

//* ＜缺省＞   ::=  default :＜语句＞
void SyntaxAnalyzer::caseDefault()
{
    if (word.symbol == "DEFAULTTK")
    {
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
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            returnBase = expression();
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