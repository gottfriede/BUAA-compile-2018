#include "LexicalAnalyzer.h"
#include <iostream>
#include <utility>
#include <cctype>

LexicalAnalyzer::LexicalAnalyzer(std::ifstream &input) : inputFile(input)
{
    line = 1;
}

Word LexicalAnalyzer::analyze()
{
    std::string lowerToken;
    std::string token;
    while (!inputFile.eof())
    {
        ch = inputFile.get();
        if (ch == '\n')
            line ++;
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
                return formWord("CONSTTK",lowerToken);
            else if (lowerToken == "int")
                return formWord("INTTK",lowerToken);
            else if (lowerToken == "char")
                return formWord("CHARTK",lowerToken);
            else if (lowerToken == "void")
                return formWord("VOIDTK",lowerToken);
            else if (lowerToken == "main")
                return formWord("MAINTK",lowerToken);
            else if (lowerToken == "if")
                return formWord("IFTK",lowerToken);
            else if (lowerToken == "else")
                return formWord("ELSETK",lowerToken);
            else if (lowerToken == "switch")
                return formWord("SWITCHTK",lowerToken);
            else if (lowerToken == "case")
                return formWord("CASETK",lowerToken);
            else if (lowerToken == "default")
                return formWord("DEFAULTTK",lowerToken);
            else if (lowerToken == "while")
                return formWord("WHILETK",lowerToken);
            else if (lowerToken == "for")
                return formWord("FORTK",lowerToken);
            else if (lowerToken == "scanf")
                return formWord("SCANFTK",lowerToken);
            else if (lowerToken == "printf")
                return formWord("PRINTFTK",lowerToken);
            else if (lowerToken == "return")
                return formWord("RETURNTK",lowerToken);
            else return formWord("IDENFR",lowerToken);
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
            while (ch != '\'')
            {
                token.append(1, ch);
                ch = inputFile.get();
            }
            return formWord("CHARCON",token);
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
    word.line = line;
    return word;
}