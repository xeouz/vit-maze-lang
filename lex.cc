#include "lex.h"
#include <string>
#include <iostream>

namespace lang
{

///---  TOKEN  ---///
Token::Token(): line(0), charpos(0)
{
    toknum = 0;
    value = "";
}
Token::Token(int _toknum, std::string _value, std::size_t _line, std::size_t _charpos)
: toknum(_toknum), value(_value), line(_line), charpos(_charpos)
{

}

std::string const Token::GetStringFromType(int token_type)
{
    switch(token_type)
    {
    case T_EOF: return "EOF";

    case T_IDENTIFIER: return "IDENTIFIER";
    case T_NUMBER: return "NUMBER";
    case T_STRING: return "STRING";

    case T_LBRACE: return "LBRACE";
    case T_RBRACE: return "RBRACE";
    case T_LBRACK: return "LBRACK";
    case T_RBRACK: return "RBRACK";
    case T_LPAREN: return "LPAREN";
    case T_RPAREN: return "RPAREN";

    case T_EQUALS: return "EQUALS";
    case T_DEQUAL: return "DEQUAL";
    case T_LARROW: return "LARROW";
    case T_RARROW: return "RARROW";
    case T_EXCLAM: return "EXCLAM";
    case T_LESSEQ: return "LESSEQ";
    case T_MOREEQ: return "MOREEQ";
    case T_NOTEQ: return "NOTEQ";

    case T_AND: return "AND";
    case T_OR: return "OR";

    case T_ADD: return "ADD";
    case T_SUB: return "SUB";
    case T_MUL: return "MUL";
    case T_DIV: return "DIV";
    case T_MOD: return "MOD";
    case T_ADDEQ: return "ADDEQ";
    case T_SUBEQ: return "SUBEQ";
    case T_MULEQ: return "MULEQ";
    case T_DIVEQ: return "DIVEQ";
    case T_MODEQ: return "MODEQ";

    case T_COMMA: return "COMMA";
    case T_DOT: return "DOT";
    case T_ATSIGN: return "ATSIGN";

    case T_IF: return "IF";
    case T_ELSE: return "ELSE";
    case T_LET: return "LET";
    case T_DO: return "DO";
    case T_FOR: return "FOR";
    case T_THROUGH: return "THROUGH";
    case T_EXTERN: return "EXTERN";

    case T_NEXTLINE: return "NEXTLINE";
    
    default: return "<INVALID>";
    };
}
std::string const Token::toString() const
{
    return GetStringFromType(toknum);
}
std::string const& Token::getValue() const
{
    return value;
};
int const Token::getTokenType() const
{
    return toknum;
};

int const Token::getKeywordTokenType(std::string const& str)
{
    if(KeywordMap.count(str))
        return KeywordMap.at(str);
    return -1;
}
///---  TOKEN  ---///

///---  LEXER  ---///
Lexer::Lexer(std::string const& in_text, char _new_line_char)
: new_line_char(_new_line_char)
{
    input_code = in_text;
    length = in_text.length();
    index = line = charpos = 0;

    if(input_code.length() > 0)
        cur = input_code.at(0);
    else
        cur = 0;
}

std::unique_ptr<Token> Lexer::constructCurrentToken(int type, std::string value)
{
    if(type == -1)
        type = T_IDENTIFIER;
    auto token = std::make_unique<Token>(type, value, line, charpos);
    return std::move(token);
}
std::unique_ptr<Token> Lexer::constructCurrentTokenAndAdvance(int type, std::string value)
{
    advance(value.length()-1);
    return constructCurrentToken(type, value);
}
char const Lexer::peekNext(int peek_amt) const
{
    int findex = index + peek_amt + 1;
    if(findex > length-1)
        return T_EOF;

    return input_code.at(findex);
}

int const Lexer::getIndex()
{
    return index;
}
void Lexer::advance(int move_amt)
{
    int findex = index + move_amt + 1;
    if(findex >= length)
    {
        cur = EOF;
        return;
    }
    
    index += move_amt + 1;
    charpos += move_amt + 1;
    cur = input_code.at(index);
}
std::string const Lexer::gatherIdentifier()
{
    std::string id = "";

    while(isalnum(cur) || cur=='_')
    {
        if(cur==EOF)
            break;
        
        id += cur;
        advance();
    }

    return id;
}
std::unique_ptr<Token> Lexer::gatherNumber()
{
    std::string numstring;

    while(isdigit(cur))
    {
        numstring += cur;
        advance();
    }

    if(cur == '.')
    {
        numstring += '.';
        advance();
    }

    while(isdigit(cur))
    {
        numstring += cur;
        advance();
    }

    return constructCurrentToken(T_NUMBER, numstring);
}
std::unique_ptr<Token> Lexer::gatherString()
{
    std::string str = "";
    char start_quote = cur;
    advance();

    while(cur != start_quote)
    {
        str += cur;
        advance();

        if(cur == EOF)
            return nullptr;
    }

    advance();
    return constructCurrentToken(T_STRING, str);
}

std::unique_ptr<Token> Lexer::getToken()
{
    while(isspace(cur))
    {
        if(cur == '\n' || cur == '\r')
        {
            line++;
            charpos = 0;
        }
        else
        {
            charpos++;
        }

        advance();
    }

    if(isalpha(cur) || cur == '_')
    {
        std::string id_str = gatherIdentifier();
        return constructCurrentToken(Token::getKeywordTokenType(id_str), id_str);
    }
    
    if(isdigit(cur))
    {
        return gatherNumber();
    }

    if(cur == new_line_char)
    {
        return constructCurrentTokenAndAdvance(T_NEXTLINE, std::string("")+new_line_char);
    }

    char peek = peekNext(0);
    switch(cur)
    {
        case '{': return constructCurrentTokenAndAdvance(T_LBRACE, "{");
        case '}': return constructCurrentTokenAndAdvance(T_RBRACE, "}");
        case '[': return constructCurrentTokenAndAdvance(T_LBRACK, "[");
        case ']': return constructCurrentTokenAndAdvance(T_RBRACK, "]");
        case '(': return constructCurrentTokenAndAdvance(T_LPAREN, "(");
        case ')': return constructCurrentTokenAndAdvance(T_RPAREN, ")");
        case ',': return constructCurrentTokenAndAdvance(T_COMMA, ",");
        case '@': return constructCurrentTokenAndAdvance(T_ATSIGN, "@");

        case '=': {
            if(peek == '=')
                return constructCurrentTokenAndAdvance(T_DEQUAL, "==");
            return constructCurrentTokenAndAdvance(T_EQUALS, "=");
        }
        case '!': {
            if(peek == '=')
                return constructCurrentTokenAndAdvance(T_NOTEQ, "!=");
            return constructCurrentTokenAndAdvance(T_EXCLAM, "!");
        }
        case '>': {
            if(peek == '=')
                return constructCurrentTokenAndAdvance(T_MOREEQ, ">=");
            return constructCurrentTokenAndAdvance(T_RARROW, ">");
        }
        case '<': {
            if(peek == '=')
                return constructCurrentTokenAndAdvance(T_LESSEQ, "<=");
            return constructCurrentTokenAndAdvance(T_LARROW, "<");
        }

        case '+': {
            if(peek == '=')
                return constructCurrentTokenAndAdvance(T_ADDEQ, "+=");
            return constructCurrentTokenAndAdvance(T_ADD, "+");
        }
        case '-': {
            if(peek == '=')
                return constructCurrentTokenAndAdvance(T_SUBEQ, "-=");
            return constructCurrentTokenAndAdvance(T_SUB, "-");
        }
        case '*': {
            if(peek == '=')
                return constructCurrentTokenAndAdvance(T_MULEQ, "*=");
            return constructCurrentTokenAndAdvance(T_MUL, "*");
        }
        case '/': {
            if(peek == '=')
                return constructCurrentTokenAndAdvance(T_DIVEQ, "/=");
            return constructCurrentTokenAndAdvance(T_DIV, "/");
        }
        case '%': {
            if(peek == '=')
                return constructCurrentTokenAndAdvance(T_MODEQ, "%=");
            return constructCurrentTokenAndAdvance(T_MOD, "%");
        }

        case '.': return constructCurrentTokenAndAdvance(T_DOT, ".");

        case '"':  return gatherString();
        case '\'': return gatherString();

        case EOF: return constructCurrentTokenAndAdvance(T_EOF, "");

        default: {
            std::cout << "LEXER: getToken(): Default case ran. Current Character ASCII: " << (int)cur << std::endl;
            advance();
            return nullptr;
        }
    }
}

std::unique_ptr<Lexer> Lexer::create(std::string const& in_text, char new_line_char)
{
    return std::make_unique<Lexer>(in_text, new_line_char);
}
///---  LEXER  ---///
}