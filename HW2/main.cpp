#include <iostream>
#include <fstream>
#include "LexicalAnalyzer.h"

int main()
{
    std::ifstream inputFile;
    inputFile.open("testfile.txt");
    LexicalAnalyzer lexicalAnalyzer((std::ifstream &) inputFile);
    lexicalAnalyzer.analyze();
    lexicalAnalyzer.outputWordList();
    inputFile.close();
    return 0;
}
