#include <iostream>
#include "SyntaxAnalyzer.h"

int main()
{
    std::ifstream inputFile;
    inputFile.open("testfile.txt");
    LexicalAnalyzer lexicalAnalyzer((std::ifstream &) inputFile);
    SymbolTableManager symbolTableManager;
    MiddleCodeManager middleCodeManager;
    SyntaxAnalyzer syntaxAnalyzer((LexicalAnalyzer &) lexicalAnalyzer, (SymbolTableManager &) symbolTableManager, (MiddleCodeManager &) middleCodeManager);
    syntaxAnalyzer.analyze();
    inputFile.close();
    return 0;
}
