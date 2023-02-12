// AUTOGENERATED by tools\tokens\generate_tokens.py on 2023-02-12 17:21:41
// DO NOT EDIT

#include <bondrewd/lex/tokens.generated.hpp>


namespace bondrewd::lex {


const char *punct_to_string(Punct value) {
    switch (value) {
        case Punct::LPAR: return "(";
        case Punct::RPAR: return ")";
        case Punct::LSQB: return "[";
        case Punct::RSQB: return "]";
        case Punct::LBRACE: return "{";
        case Punct::RBRACE: return "}";
        case Punct::COLON: return ":";
        case Punct::DOT: return ".";
        case Punct::COMMA: return ",";
        case Punct::SEMI: return ";";
        case Punct::PLUS: return "+";
        case Punct::MINUS: return "-";
        case Punct::STAR: return "*";
        case Punct::SLASH: return "/";
        case Punct::PERCENT: return "%";
        case Punct::TILDE: return "~";
        case Punct::CIRCUMFLEX: return "^";
        case Punct::VBAR: return "|";
        case Punct::AMPER: return "&";
        case Punct::LEFTSHIFT: return "<<";
        case Punct::RIGHTSHIFT: return ">>";
        case Punct::EQUAL: return "=";
        case Punct::PLUSEQUAL: return "+=";
        case Punct::MINEQUAL: return "-=";
        case Punct::STAREQUAL: return "*=";
        case Punct::SLASHEQUAL: return "/=";
        case Punct::PERCENTEQUAL: return "%=";
        case Punct::AMPEREQUAL: return "&=";
        case Punct::VBAREQUAL: return "|=";
        case Punct::CIRCUMFLEXEQUAL: return "^=";
        case Punct::LEFTSHIFTEQUAL: return "<<=";
        case Punct::RIGHTSHIFTEQUAL: return ">>=";
        case Punct::EQEQUAL: return "==";
        case Punct::NOTEQUAL: return "!=";
        case Punct::GREATER: return ">";
        case Punct::LESS: return "<";
        case Punct::LESSEQUAL: return "<=";
        case Punct::GREATEREQUAL: return ">=";
        case Punct::BIDIRCMP: return "<=>";
        case Punct::AT: return "@";
        case Punct::RARROW: return "->";
        case Punct::RARROW2: return "=>";
        
    }
}

const char *keyword_to_string(HardKeyword value) {
    switch (value) {
        case HardKeyword::IF: return "if";
        case HardKeyword::ELSE: return "else";
        case HardKeyword::FOR: return "for";
        case HardKeyword::WHILE: return "while";
        case HardKeyword::MATCH: return "match";
        case HardKeyword::VAR: return "var";
        case HardKeyword::FUNC: return "func";
        case HardKeyword::TRUE: return "true";
        case HardKeyword::False: return "false";
        case HardKeyword::CLASS: return "class";
        case HardKeyword::TRAIT: return "trait";
        case HardKeyword::RETURN: return "return";
        case HardKeyword::IMPORT: return "import";
        case HardKeyword::EXPORT: return "export";
        case HardKeyword::PUBLIC: return "public";
        case HardKeyword::PRIVATE: return "private";
        case HardKeyword::PROTECTED: return "protected";
        case HardKeyword::CARTRIDGE: return "cartridge";
        
    }
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
const std::unordered_map<std::string_view, Punct> string_to_punct = {
    { "(", Punct::LPAR },
    { ")", Punct::RPAR },
    { "[", Punct::LSQB },
    { "]", Punct::RSQB },
    { "{", Punct::LBRACE },
    { "}", Punct::RBRACE },
    { ":", Punct::COLON },
    { ".", Punct::DOT },
    { ",", Punct::COMMA },
    { ";", Punct::SEMI },
    { "+", Punct::PLUS },
    { "-", Punct::MINUS },
    { "*", Punct::STAR },
    { "/", Punct::SLASH },
    { "%", Punct::PERCENT },
    { "~", Punct::TILDE },
    { "^", Punct::CIRCUMFLEX },
    { "|", Punct::VBAR },
    { "&", Punct::AMPER },
    { "<<", Punct::LEFTSHIFT },
    { ">>", Punct::RIGHTSHIFT },
    { "=", Punct::EQUAL },
    { "+=", Punct::PLUSEQUAL },
    { "-=", Punct::MINEQUAL },
    { "*=", Punct::STAREQUAL },
    { "/=", Punct::SLASHEQUAL },
    { "%=", Punct::PERCENTEQUAL },
    { "&=", Punct::AMPEREQUAL },
    { "|=", Punct::VBAREQUAL },
    { "^=", Punct::CIRCUMFLEXEQUAL },
    { "<<=", Punct::LEFTSHIFTEQUAL },
    { ">>=", Punct::RIGHTSHIFTEQUAL },
    { "==", Punct::EQEQUAL },
    { "!=", Punct::NOTEQUAL },
    { ">", Punct::GREATER },
    { "<", Punct::LESS },
    { "<=", Punct::LESSEQUAL },
    { ">=", Punct::GREATEREQUAL },
    { "<=>", Punct::BIDIRCMP },
    { "@", Punct::AT },
    { "->", Punct::RARROW },
    { "=>", Punct::RARROW2 },
    
};

const std::unordered_map<std::string_view, HardKeyword> string_to_keyword = {
    { "if", HardKeyword::IF },
    { "else", HardKeyword::ELSE },
    { "for", HardKeyword::FOR },
    { "while", HardKeyword::WHILE },
    { "match", HardKeyword::MATCH },
    { "var", HardKeyword::VAR },
    { "func", HardKeyword::FUNC },
    { "true", HardKeyword::TRUE },
    { "false", HardKeyword::False },
    { "class", HardKeyword::CLASS },
    { "trait", HardKeyword::TRAIT },
    { "return", HardKeyword::RETURN },
    { "import", HardKeyword::IMPORT },
    { "export", HardKeyword::EXPORT },
    { "public", HardKeyword::PUBLIC },
    { "private", HardKeyword::PRIVATE },
    { "protected", HardKeyword::PROTECTED },
    { "cartridge", HardKeyword::CARTRIDGE },
    
};
#pragma GCC diagnostic pop


} // namespace bondrewd::lex
