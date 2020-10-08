#include "LexicalAnalyzer.h"
#include <iostream>
#include <fstream>
#include <utility>
#include <cctype>

LexicalAnalyzer::LexicalAnalyzer(std::ifstream &input)
{
    inputFile = dynamic_cast<std::basic_ifstream<char> &&>(input);
}

void LexicalAnalyzer::analyze()
{
    char ch;
    std::string lowerToken;
    std::string token;
    int num = 0;
    while (!inputFile.eof())
    {
        ch = inputFile.get();
        if (isalpha(ch) || ch == '_')
        {
            token.append(1, ch);
            lowerToken.append(1, tolower(ch));
            char next = inputFile.peek();
            while (isalpha(next) || isdigit(next) || next == '_')
            {
                ch = inputFile.get();
                token.append(1, ch);
                lowerToken.append(1, tolower(ch));
                next = inputFile.peek();
            }
            if (lowerToken == "const")
                addToList("CONSTTK",token);
            else if (lowerToken == "int")
                addToList("INTTK",token);
            else if (lowerToken == "char")
                addToList("CHARTK",token);
            else if (lowerToken == "void")
                addToList("VOIDTK",token);
            else if (lowerToken == "main")
                addToList("MAINTK",token);
            else if (lowerToken == "if")
                addToList("IFTK",token);
            else if (lowerToken == "else")
                addToList("ELSETK",token);
            else if (lowerToken == "switch")
                addToList("SWITCHTK",token);
            else if (lowerToken == "case")
                addToList("CASETK",token);
            else if (lowerToken == "default")
                addToList("DEFAULTTK",token);
            else if (lowerToken == "while")
                addToList("WHILETK",token);
            else if (lowerToken == "for")
                addToList("FORTK",token);
            else if (lowerToken == "scanf")
                addToList("SCANFTK",token);
            else if (lowerToken == "printf")
                addToList("PRINTFTK",token);
            else if (lowerToken == "return")
                addToList("RETURNTK",token);
            else addToList("IDENFR",token);
            token.clear();
            lowerToken.clear();
        }
        else if (isdigit(ch))
        {
            token.append(1, ch);
            char next = inputFile.peek();
            while (isdigit(next))
            {
                ch = inputFile.get();
                token.append(1, ch);
                next = inputFile.peek();
            }
            addToList("INTCON", token);
            token.clear();
        }
        else if (ch == '\'')
        {
            ch = inputFile.get();
            if (inputFile.get() == '\'')
                addToList("CHARCON",std::string(1, ch));
        }
        else if (ch == '\"')
        {
            ch = inputFile.get();
            while (ch != '\"')
            {
                token.append(1, ch);
                ch = inputFile.get();
            }
            addToList("STRCON", token);
            token.clear();
        }
        else if (ch == '+')
            addToList("PLUS","+");
        else if (ch == '-')
            addToList("MINU","-");
        else if (ch == '*')
            addToList("MULT","*");
        else if (ch == '/')
            addToList("DIV","/");
        else if (ch == '<')
        {
            char nextCh = inputFile.peek();
            if (nextCh == '=')
            {
                addToList("LEQ", "<=");
                ch = inputFile.get();
            }
            else addToList("LSS","<");
        }
        else if (ch == '>')
        {
            char nextCh = inputFile.peek();
            if (nextCh == '=')
            {
                addToList("GEQ",">=");
                ch = inputFile.get();
            }
            else addToList("GRE",">");
        }
        else if (ch == '=')
        {
            char nextCh = inputFile.peek();
            if (nextCh == '=')
            {
                addToList("EQL","==");
                ch = inputFile.get();
            }
            else addToList("ASSIGN","=");
        }
        else if (ch == '!')
        {
            char nextCh = inputFile.peek();
            if (nextCh == '=')
            {
                addToList("NEQ","!=");
                ch = inputFile.get();
            }
        }
        else if (ch == ':')
            addToList("COLON",":");
        else if (ch == ';')
            addToList("SEMICN",";");
        else if (ch == ',')
            addToList("COMMA",",");
        else if (ch == '(')
            addToList("LPARENT","(");
        else if (ch == ')')
            addToList("RPARENT",")");
        else if (ch == '[')
            addToList("LBRACK","[");
        else if (ch == ']')
            addToList("RBRACK","]");
        else if (ch == '{')
            addToList("LBRACE","{");
        else if (ch == '}')
            addToList("RBRACE","}");
    }

}

void LexicalAnalyzer::addToList(std::string symbol, std::string content)
{
    Word word;
    word.symbol = std::move(symbol);
    word.content = std::move(content);
    wordList.push_back(word);
}

void LexicalAnalyzer::outputWordList()
{
    std::ofstream outputFile;
    outputFile.open("output.txt");
    for (std::list<Word>::iterator it = wordList.begin(); it != wordList.end(); it ++)
    {
        std::cout << it->symbol << " " << it->content << std::endl;
        outputFile << it->symbol << " " << it->content << std::endl;
    }

}