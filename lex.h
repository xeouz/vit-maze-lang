#include <string>
#include <memory>
#include <unordered_map>

namespace lang
{

enum
{
    T_EOF,

    T_IDENTIFIER,
    T_NUMBER,
    T_STRING,

    T_LBRACE,
    T_RBRACE,
    T_LBRACK,
    T_RBRACK,
    T_LPAREN,
    T_RPAREN,

    T_EQUALS,
    T_DEQUAL,
    T_LARROW,
    T_RARROW,
    T_EXCLAM,
    T_LESSEQ,
    T_MOREEQ,
    T_NOTEQ,

    T_AND,
    T_OR,

    T_ADD,
    T_SUB,
    T_MUL,
    T_DIV,
    T_MOD,
    T_ADDEQ,
    T_SUBEQ,
    T_MULEQ,
    T_DIVEQ,
    T_MODEQ,

    T_COMMA,
    T_DOT,

    T_LET,
    T_DO,
    T_FOR,
    T_THROUGH,
};

const std::unordered_map<std::string, int> KeywordMap = {
    {"let", T_LET},
    {"do", T_DO},
    {"for", T_FOR},
    {"through", T_THROUGH},
    {"and", T_AND},
    {"or", T_OR}
};

class Token
{
    int toknum;
    std::string value;
    std::size_t line, charpos;
public:
    Token();
    Token(int toknum, std::string value, std::size_t line=0, std::size_t charpos=0);

    std::string const& getValue() const;
    int const getTokenNumber() const;

    inline friend std::ostream& operator<<(std::ostream& os, const Token& token);
    inline friend bool operator==(int num, const Token& token);
    inline friend bool operator==(const Token& lhs, const Token& rhs);

    static int const getKeywordTokenType(std::string const& string);

    std::string const toString() const;
};

inline std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << token.toString();
    return os;
}
inline bool operator==(int num, const Token& token)
{
    return token.toknum == num;
}
inline bool operator==(const Token& lhs, const Token& rhs)
{
    return lhs.toknum == rhs.toknum;
}

class Lexer
{
    char cur;
    std::string input_code;
    std::size_t index, line, charpos, length;

    std::unique_ptr<Token> constructCurrentToken(int type, std::string value);
    std::unique_ptr<Token> constructCurrentTokenAndAdvance(int type, std::string value);
    char const peekNext(int peek_amt=0) const;
public:
    Lexer(std::string const& in_text);

    int const getIndex();
    void advance(int move_amt=0);
    std::string const gatherIdentifier();
    std::unique_ptr<Token> gatherNumber();
    std::unique_ptr<Token> gatherString();

    std::unique_ptr<Token> getToken();
    
    std::unique_ptr<Token> constructToken();
};

};