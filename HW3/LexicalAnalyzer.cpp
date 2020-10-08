#include "LexicalAnalyzer.h"
#include <iostream>
#include <utility>
#include <cctype>

LexicalAnalyzer::LexicalAnalyzer(std::ifstream &input) : inputFile(input)
{}

Word LexicalAnalyzer::analyze()
{
    std::string lowerToken;
    std::string token;
    while (!inputFile.eof())
    {
        ch = inputFile.get();
        if (isspace(ch))
            continue;
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
                return formWord("CONSTTK",token);
            else if (lowerToken == "int")
                return formWord("INTTK",token);
            else if (lowerToken == "char")
                return formWord("CHARTK",token);
            else if (lowerToken == "void")
                return formWord("VOIDTK",token);
            else if (lowerToken == "main")
                return formWord("MAINTK",token);
            else if (lowerToken == "if")
                return formWord("IFTK",token);
            else if (lowerToken == "else")
                return formWord("ELSETK",token);
            else if (lowerToken == "switch")
                return formWord("SWITCHTK",token);
            else if (lowerToken == "case")
                return formWord("CASETK",token);
            else if (lowerToken == "default")
                return formWord("DEFAULTTK",token);
            else if (lowerToken == "while")
                return formWord("WHILETK",token);
            else if (lowerToken == "for")
                return formWord("FORTK",token);
            else if (lowerToken == "scanf")
                return formWord("SCANFTK",token);
            else if (lowerToken == "printf")
                return formWord("PRINTFTK",token);
            else if (lowerToken == "return")
                return formWord("RETURNTK",token);
            else return formWord("IDENFR",token);
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
            return formWord("INTCON", token);
        }
        else if (ch == '\'')
        {
            ch = inputFile.get();
            if (inputFile.get() == '\'')
                return formWord("CHARCON",std::string(1, ch));
        }
        else if (ch == '\"')
        {
            ch = inputFile.get();
            while (ch != '\"')
            {
                token.append(1, ch);
                ch = inputFile.get();
            }
            return formWord("STRCON", token);
        }
        else if (ch == '+')
            return formWord("PLUS","+");
        else if (ch == '-')
            return formWord("MINU","-");
        else if (ch == '*')
            return formWord("MULT","*");
        else if (ch == '/')
            return formWord("DIV","/");
        else if (ch == '<')
        {
            char nextCh = inputFile.peek();
            if (nextCh == '=')
            {
                ch = inputFile.get();
                return formWord("LEQ", "<=");
            }
            else return formWord("LSS","<");
        }
        else if (ch == '>')
        {
            char nextCh = inputFile.peek();
            if (nextCh == '=')
            {
                ch = inputFile.get();
                return formWord("GEQ",">=");
            }
            else return formWord("GRE",">");
        }
        else if (ch == '=')
        {
            char nextCh = inputFile.peek();
            if (nextCh == '=')
            {
                ch = inputFile.get();
                return formWord("EQL","==");
            }
            else return formWord("ASSIGN","=");
        }
        else if (ch == '!')
        {
            char nextCh = inputFile.peek();
            if (nextCh == '=')
            {
                ch = inputFile.get();
                return formWord("NEQ","!=");
            }
        }
        else if (ch == ':')
            return formWord("COLON",":");
        else if (ch == ';')
            return formWord("SEMICN",";");
        else if (ch == ',')
            return formWord("COMMA",",");
        else if (ch == '(')
            return formWord("LPARENT","(");
        else if (ch == ')')
            return formWord("RPARENT",")");
        else if (ch == '[')
            return formWord("LBRACK","[");
        else if (ch == ']')
            return formWord("RBRACK","]");
        else if (ch == '{')
            return formWord("LBRACE","{");
        else if (ch == '}')
            return formWord("RBRACE","}");
    }
    return formWord("EOF","EOF");
}

Word LexicalAnalyzer::formWord(std::string symbol, std::string content)
{
    Word word;
    word.symbol = symbol;
    word.content = content;
    return word;
}