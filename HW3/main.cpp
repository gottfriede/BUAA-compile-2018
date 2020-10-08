#include <iostream>
#include "SyntaxAnalyzer.h"

int main()
{
    std::ifstream inputFile;
    inputFile.open("testfile.txt");
    LexicalAnalyzer lexicalAnalyzer((std::ifstream &) inputFile);
    SyntaxAnalyzer syntaxAnalyzer((LexicalAnalyzer &) lexicalAnalyzer);
    syntaxAnalyzer.analyze();
    inputFile.close();
    return 0;
}
