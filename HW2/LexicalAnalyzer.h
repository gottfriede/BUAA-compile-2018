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
    std::ifstream inputFile;
    std::list<Word> wordList;
    void addToList(std::string symbol, std::string content);

public:
    LexicalAnalyzer(std::ifstream &inputFile);
    void analyze();
    void outputWordList();
};
