#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#endif //SYNTAXANALYZER_H

#include <map>
#include "LexicalAnalyzer.h"
#include "MiddleCodeManager.h"

struct ErrorMessage
{
    ErrorMessage(int line, char type);

    int line;
    char type;
};

class SyntaxAnalyzer
{
private:
    LexicalAnalyzer lexicalAnalyzer;
    SymbolTableManager symbolTableManager;
    MiddleCodeManager middleCodeManager;
    Word lastWord;
    Word word;
    std::map<std::string, std::string> idMap;
    std::ofstream outputFile;
    std::ofstream errorFile;
    identifierBaseType nowFuncReturn;
    std::vector<ErrorMessage> errorList;
    Symbol nowSymbol = Symbol("18373584null", NOTEXISTS, NOEXISTS);
    int tmpIndex;
    bool hasReturn;
    int ifId;
    int switchId;
    int forId;
    int whileId;
    Operator additionOperator();
    void multiplicationOperator();
    Operator relationalOperator();
    void letter();
    void digital();
    int character();
    std::string string();
    void program();
    void constantDescription();
    void constantDefinition();
    int unsignedInteger();
    int integer();
    std::string identifier(int mode);  // 0-declare, 1-refer
    void descriptionHeader();
    int constant();
    void variableDescription();
    void variableDefinition(Word type);
    void variableDefinitionNoInitial();
    void variableDefinitionWithInitial();
    void typeIdentifier();
    void functionDefinitionWithReturn(int mode);
    void functionDefinitionNoReturn(int mode);
    void compoundStatement();
    void parameterTable();
    void mainFunction();
    identifierBaseType expression();
    identifierBaseType item();
    identifierBaseType factor();
    void statement();
    void assignStatement(std::string idName);
    void conditionStatement();
    void condition(int id, std::string name);
    void loopStatement();
    int stepSize();
    void caseStatement();
    void caseTable(identifierBaseType expressionBaseType);
    void caseChildrenStatement(identifierBaseType expressionBaseType);
    void caseDefault();
    void functionCallWithReturn();
    void functionCallNoReturn();
    void functionCallNoDefine(Word func);
    void valueParameterTable(std::string funcName);
    void statementColumn();
    void readStatement();
    void writeStatement();
    void returnStatement();
    void output(std::string syntaxContent);
    void output(Word outputWord);
    void addToMap(std::string id, std::string type);
    void getWord();
    void outputError(int line, char errorType);
    std::string newSymbolName();

public:
    SyntaxAnalyzer(LexicalAnalyzer &newLexicalAnalyzer, SymbolTableManager &newSymbolTableManager, MiddleCodeManager &newMiddleCodeManager);
    void analyze();
};