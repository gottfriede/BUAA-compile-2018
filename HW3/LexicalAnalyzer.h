#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#endif //LEXICALANALYZER_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>

struct Word
{
    std::string symbol;
    std::string content;
};

class LexicalAnalyzer
{
private:
    std::ifstream &inputFile;
    char ch;
    Word formWord(std::string symbol, std::string content);

public:
    LexicalAnalyzer(std::ifstream &inputFile);
    Word analyze();
};
