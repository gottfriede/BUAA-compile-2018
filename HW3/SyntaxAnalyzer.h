#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#endif //SYNTAXANALYZER_H

#include <map>
#include "LexicalAnalyzer.h"

class SyntaxAnalyzer
{
private:
    LexicalAnalyzer lexicalAnalyzer;
    Word word;
    std::map<std::string, std::string> idMap;
    std::ofstream outputFile;
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
    void unsignedInteger();
    void integer();
    void identifier();
    void descriptionHeader();
    void constant();
    void variableDescription();
    void variableDefinition();
    void variableDefinitionNoInitial();
    void variableDefinitionWithInitial();
    void typeIdentifier();
    void functionDefinitionWithReturn(int mode);
    void functionDefinitionNoReturn(int mode);
    void compoundStatement();
    void parameterTable();
    void mainFunction();
    void expression();
    void item();
    void factor();
    void statement();
    void assignStatement();
    void conditionStatement();
    void condition();
    void loopStatement();
    void stepSize();
    void caseStatement();
    void caseTable();
    void caseChildrenStatement();
    void caseDefault();
    void functionCallWithReturn();
    void functionCallNoReturn();
    void valueParameterTable();
    void statementColumn();
    void readStatement();
    void writeStatement();
    void returnStatement();
    void output(std::string syntaxContent);
    void output(Word outputWord);
    void addToMap(std::string id, std::string type);
    void getWord();


public:
    SyntaxAnalyzer(LexicalAnalyzer &newLexicalAnalyzer);
    void analyze();
};