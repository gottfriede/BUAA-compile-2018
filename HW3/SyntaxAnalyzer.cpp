#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer &newLexicalAnalyzer) : lexicalAnalyzer(newLexicalAnalyzer)
{}

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
void SyntaxAnalyzer::character()
{
    if (word.symbol == "CHARCON")
    {
        output(word);
        //output("<字符>");
        getWord();
    }
    else output("character Error");
}

//* ＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
void SyntaxAnalyzer::string()
{
    if (word.symbol == "STRCON")
    {
        output(word);
        output("<字符串>");
        getWord();
    }
    else output("string Error");
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
        else output("constantDescription Error1");
    }
    if (right)
        output("<常量说明>");
    else
        output("constantDescription Error2");
}

//* ＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}   |   char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
void SyntaxAnalyzer::constantDefinition()
{
    if (word.symbol == "INTTK")
    {
        while (true)
        {
            output(word);
            getWord();
            addToMap(word.content, "INT");
            identifier();
            if (word.symbol == "ASSIGN")
            {
                output(word);
                getWord();
                integer();
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
            addToMap(word.content, "CHAR");
            identifier();
            if (word.symbol == "ASSIGN")
            {
                output(word);
                getWord();
                character();
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
void SyntaxAnalyzer::unsignedInteger()
{
    if (word.symbol == "INTCON")
    {
        output(word);
        output("<无符号整数>");
        getWord();
    }
    else output("unsignedInteger Error");
}

//* ＜整数＞        ::= ［＋｜－］＜无符号整数＞
void SyntaxAnalyzer::integer()
{
    if (word.symbol == "PLUS" || word.symbol == "MINU")
    {
        output(word);
        getWord();
    }
    unsignedInteger();
    output("<整数>");
}

// ＜标识符＞    ::=  ＜字母＞｛＜字母＞｜＜数字＞｝
void SyntaxAnalyzer::identifier()
{
    if (word.symbol == "IDENFR")
    {
        output(word);
        //output("<标识符>");
        getWord();
    }
    else output("identifier Error");
}

//* ＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞
void SyntaxAnalyzer::descriptionHeader()
{
    if (word.symbol == "INTTK" || word.symbol == "CHARTK")
    {
        output(word);
        getWord();
        addToMap(word.content, "RETURNFUNC");
        identifier();
        output("<声明头部>");
    }
    else output("descriptionHeader Error");
}

//* ＜常量＞   ::=  ＜整数＞|＜字符＞
void SyntaxAnalyzer::constant()
{
    if (word.symbol == "CHARCON")
        character();
    else integer();
    output("<常量>");
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
            output("<声明头部>");
            functionDefinitionWithReturn(1);
            return;
        }
        varDefine = true;
        output(type);
        output(id);
        variableDefinition();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
        else output("variableDescription Error");
    }
    output("<变量说明>");
}

//* ＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
void SyntaxAnalyzer::variableDefinition()
{
    bool assign = false;
    while (true)
    {
        if (word.symbol == "LBRACK")
        {
            output(word);
            getWord();
            unsignedInteger();
            if (word.symbol == "RBRACK")
            {
                output(word);
                getWord();
                if (word.symbol == "LBRACK")
                {
                    output(word);
                    getWord();
                    unsignedInteger();
                    if (word.symbol == "RBRACK")
                    {
                        output(word);
                        getWord();
                    }
                }
            }
        }
        if (word.symbol == "ASSIGN")
        {
            assign = true;
            output(word);
            getWord();
            if (word.symbol == "LBRACE")
            {
                output(word);
                getWord();
                if (word.symbol == "LBRACE")
                {
                    while (true)
                    {
                        if (word.symbol == "LBRACE")
                        {
                            output(word);
                            getWord();
                            while (true)
                            {
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
                    while (true)
                    {
                        constant();
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
                constant();
        }
        if (word.symbol == "COMMA")
        {
            output(word);
            getWord();
            identifier();
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
        if (word.symbol == "RPARENT")
        {
            output(word);
            getWord();
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
    }
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
        identifier();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            parameterTable();
            if (word.symbol == "RPARENT")
            {
                output(word);
                getWord();
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
        }
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
        typeIdentifier();
        identifier();
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
            output(word);
            getWord();
            if (word.symbol == "LPARENT")
            {
                output(word);
                getWord();
                if (word.symbol == "RPARENT")
                {
                    output(word);
                    getWord();
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
            }
        }
        output("<主函数>");
}

//* ＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
void SyntaxAnalyzer::expression()
{
    if (word.symbol == "PLUS" || word.symbol == "MINU")
    {
        output(word);
        getWord();
    }
    item();
    while (word.symbol == "PLUS" || word.symbol == "MINU")
    {
        additionOperator();
        item();
    }
    output("<表达式>");
}

//* ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
void SyntaxAnalyzer::item()
{
    factor();
    while (word.symbol == "MULT" || word.symbol == "DIV")
    {
        multiplicationOperator();
        factor();
    }
    output("<项>");
}

//* ＜因子＞    ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'|＜标识符＞'['＜表达式＞']''['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
void SyntaxAnalyzer::factor()
{
    if (word.symbol == "IDENFR" && idMap.find(word.content)->second == "RETURNFUNC")
        functionCallWithReturn();
    else if (word.symbol == "CHARCON")
        character();
    else if (word.symbol == "INTCON" || word.symbol == "PLUS" || word.symbol == "MINU")
        integer();
    else if (word.symbol == "LPARENT")
    {
        output(word);
        getWord();
        expression();
        if (word.symbol == "RPARENT")
        {
            output(word);
            getWord();
        }
    }
    else if (word.symbol == "IDENFR")
    {
        output(word);
        getWord();
        if (word.symbol == "LBRACK")
        {
            output(word);
            getWord();
            expression();
            if (word.symbol == "RBRACK")
            {
                output(word);
                getWord();
                if (word.symbol == "LBRACK")
                {
                    output(word);
                    getWord();
                    expression();
                    if (word.symbol == "RBRACK")
                    {
                        output(word);
                        getWord();
                    }
                }
            }
        }
    }
    output("<因子>");
}

//* ＜语句＞    ::= ＜循环语句＞｜＜条件语句＞| ＜有返回值函数调用语句＞;  |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜情况语句＞｜＜空＞;|＜返回语句＞; | '{'＜语句列＞'}'
void SyntaxAnalyzer::statement()
{
    if (word.symbol == "WHILETK" || word.symbol == "FORTK")
        loopStatement();
    else if (word.symbol == "IFTK")
        conditionStatement();
    else if (word.symbol == "IDENFR" && idMap.find(word.content)->second == "RETURNFUNC")
    {
        functionCallWithReturn();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
    }
    else if (word.symbol == "IDENFR" && idMap.find(word.content)->second == "NORETURNFUNC")
    {
        functionCallNoReturn();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
    }
    else if (word.symbol == "IDENFR")
    {
        assignStatement();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
    }
    else if (word.symbol == "SCANFTK")
    {
        readStatement();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
    }
    else if (word.symbol == "PRINTFTK")
    {
        writeStatement();
        if (word.symbol == "SEMICN")
        {
            output(word);
            getWord();
        }
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
    output("<语句>");
}

//* ＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞|＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
void SyntaxAnalyzer::assignStatement()
{
    identifier();
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
        expression();
        if (word.symbol == "RBRACK")
        {
            output(word);
            getWord();
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
                expression();
                if (word.symbol == "RBRACK")
                {
                    output(word);
                    getWord();
                    if (word.symbol == "ASSIGN") {
                        output(word);
                        getWord();
                        expression();
                    }
                }
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
            if (word.symbol == "RPARENT")
            {
                output(word);
                getWord();
                statement();
                if (word.symbol == "ELSETK")
                {
                    output(word);
                    getWord();
                    statement();
                }
            }
        }
        output("<条件语句>");
    }
}

//* ＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
void SyntaxAnalyzer::condition()
{
    expression();
    relationalOperator();
    expression();
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
            if (word.symbol == "RPARENT")
            {
                output(word);
                getWord();
                statement();
            }
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
            identifier();
            if (word.symbol == "ASSIGN")
            {
                output(word);
                getWord();
                expression();
                if (word.symbol == "SEMICN")
                {
                    output(word);
                    getWord();
                    condition();
                    if (word.symbol == "SEMICN")
                    {
                        output(word);
                        getWord();
                        identifier();
                        if (word.symbol == "ASSIGN")
                        {
                            output(word);
                            getWord();
                            identifier();
                            additionOperator();
                            stepSize();
                            if (word.symbol == "RPARENT")
                            {
                                output(word);
                                getWord();
                                statement();
                            }
                        }
                    }
                }
            }
        }
        output("<循环语句>");
    }
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
            expression();
            if (word.symbol == "RPARENT")
            {
                output(word);
                getWord();
                if (word.symbol == "LBRACE")
                {
                    output(word);
                    getWord();
                    caseTable();
                    caseDefault();
                    if (word.symbol == "RBRACE")
                    {
                        output(word);
                        getWord();
                    }
                }
            }
        }
        output("<情况语句>");
    }
}

//* ＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
void SyntaxAnalyzer::caseTable()
{
    while (word.symbol == "CASETK")
    {
        caseChildrenStatement();
    }
    output("<情况表>");
}

//* ＜情况子语句＞  ::=  case＜常量＞：＜语句＞
void SyntaxAnalyzer::caseChildrenStatement()
{
    if (word.symbol == "CASETK")
    {
        output(word);
        getWord();
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
    output("<缺省>");
}

//* ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
void SyntaxAnalyzer::functionCallWithReturn()
{
    identifier();
    if (word.symbol == "LPARENT")
    {
        output(word);
        getWord();
        valueParameterTable();
        if (word.symbol == "RPARENT")
        {
            output(word);
            getWord();
        }
    }
    output("<有返回值函数调用语句>");
}

//* ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
void SyntaxAnalyzer::functionCallNoReturn()
{
    identifier();
    if (word.symbol == "LPARENT")
    {
        output(word);
        getWord();
        valueParameterTable();
        if (word.symbol == "RPARENT")
        {
            output(word);
            getWord();
        }
    }
    output("<无返回值函数调用语句>");
}

//* ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
void SyntaxAnalyzer::valueParameterTable()
{
    if (word.symbol != "RPARENT")
    {
        while (true)
        {
            expression();
            if (word.symbol == "COMMA")
            {
                output(word);
                getWord();
            }
            else break;
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
            identifier();
            if (word.symbol == "RPARENT")
            {
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
                string();
                if (word.symbol == "COMMA")
                {
                    output(word);
                    getWord();
                    expression();
                    if (word.symbol == "RPARENT")
                    {
                        output(word);
                        getWord();
                    }
                }
                if (word.symbol == "RPARENT")
                {
                    output(word);
                    getWord();
                }
            }
            else
            {
                expression();
                if (word.symbol == "RPARENT")
                {
                    output(word);
                    getWord();
                }
            }
        }
        output("<写语句>");
    }
}

//* ＜返回语句＞   ::=  return['('＜表达式＞')']
void SyntaxAnalyzer::returnStatement()
{
    if (word.symbol == "RETURNTK")
    {
        output(word);
        getWord();
        if (word.symbol == "LPARENT")
        {
            output(word);
            getWord();
            expression();
            if (word.symbol == "RPARENT")
            {
                output(word);
                getWord();
            }
        }
        output("<返回语句>");
    }
}

void SyntaxAnalyzer::output(std::string syntaxContent)
{
    std::cout << syntaxContent << std::endl;
    outputFile << syntaxContent << std::endl;
}

void SyntaxAnalyzer::output(Word outputWord)
{
    std::cout << outputWord.symbol << " " << outputWord.content << std::endl;
    outputFile << outputWord.symbol << " " << outputWord.content << std::endl;
}

void SyntaxAnalyzer::getWord()
{
    word = lexicalAnalyzer.analyze();
}

void SyntaxAnalyzer::addToMap(std::string id, std::string type)
{
    idMap.insert(std::pair<std::string, std::string>(id, type));
}

void SyntaxAnalyzer::analyze()
{
    outputFile.open("output.txt");
    getWord();
    program();
    outputFile.close();
}