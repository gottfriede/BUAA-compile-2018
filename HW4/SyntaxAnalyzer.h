#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#endif //SYNTAXANALYZER_H

#include <map>
#include "LexicalAnalyzer.h"
#include "SymbolTable.h"

class SyntaxAnalyzer
{
private:
    LexicalAnalyzer lexicalAnalyzer;
    SymbolTableManager symbolTableManager;
    Word lastWord;
    Word word;
    std::map<std::string, std::string> idMap;
    std::ofstream outputFile;
    std::ofstream errorFile;
    identifierBaseType nowFuncReturn;
    bool hasReturn;
    void additionOperator();
    void multiplicationOperator();
    void relationalOperator();
    void letter();
    void digital();
    void character();
    void string();
    void program();
    void constantDescription();
    void constantDefinition();
    int unsignedInteger();
    void integer();
    void identifier(int mode);  // 0-declare, 1-refer
    void descriptionHeader();
    void constant();
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
    void condition();
    void loopStatement();
    void stepSize();
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


public:
    SyntaxAnalyzer(LexicalAnalyzer &newLexicalAnalyzer, SymbolTableManager &newSymbolTableManager);
    void analyze();
};