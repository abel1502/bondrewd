// AUTOGENERATED by bondrewd/tools/tokens/generate_tokens.py on 2023-04-24 06:25:53
// DO NOT EDIT

#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/scanner.hpp>

#include <string_view>
#include <cstdio>
#include <unordered_map>


namespace bondrewd::lex {


enum class Punct {
    LPAR = 0,
    RPAR = 1,
    LSQB = 2,
    RSQB = 3,
    LBRACE = 4,
    RBRACE = 5,
    EXCLAMATION = 6,
    COLON = 7,
    DOT = 8,
    COMMA = 9,
    SEMI = 10,
    DOUBLECOLON = 11,
    PLUS = 12,
    MINUS = 13,
    STAR = 14,
    SLASH = 15,
    PERCENT = 16,
    POWER = 17,
    TILDE = 18,
    CIRCUMFLEX = 19,
    VBAR = 20,
    AMPER = 21,
    LEFTSHIFT = 22,
    RIGHTSHIFT = 23,
    EQUAL = 24,
    PLUSEQUAL = 25,
    MINEQUAL = 26,
    STAREQUAL = 27,
    SLASHEQUAL = 28,
    PERCENTEQUAL = 29,
    AMPEREQUAL = 30,
    VBAREQUAL = 31,
    CIRCUMFLEXEQUAL = 32,
    LEFTSHIFTEQUAL = 33,
    RIGHTSHIFTEQUAL = 34,
    DOUBLEEQUAL = 35,
    NOTEQUAL = 36,
    GREATER = 37,
    LESS = 38,
    GREATEREQUAL = 39,
    LESSEQUAL = 40,
    BIDIRCMP = 41,
    AT = 42,
    RARROW = 43,
    RARROW2 = 44,
    ELLIPSIS = 45,
};


enum class HardKeyword {
    AND = 0,
    OR = 1,
    NOT = 2,
    IN = 3,
    IS = 4,
    IF = 5,
    THEN = 6,
    ELSE = 7,
    FOR = 8,
    WHILE = 9,
    LOOP = 10,
    RETURN = 11,
    BREAK = 12,
    CONTINUE = 13,
    MATCH = 14,
    CTIME = 15,
    RTIME = 16,
    DYN = 17,
    REF = 18,
    MOVE = 19,
    COPY = 20,
    MUT = 21,
    VAR = 22,
    VAL = 23,
    FUNC = 24,
    CLASS = 25,
    STRUCT = 26,
    ENUM = 27,
    TRAIT = 28,
    NAMESPACE = 29,
    IMPL = 30,
    CARTRIDGE = 31,
    EXPAND = 32,
    UNWRAP = 33,
    IMPORT = 34,
    EXPORT = 35,
    PUBLIC = 36,
    PROTECTED = 37,
    PRIVATE = 38,
};


const char *punct_to_string(Punct value);

const char *keyword_to_string(HardKeyword value);

extern const std::unordered_map<std::string_view, Punct> string_to_punct;

extern const std::unordered_map<std::string_view, HardKeyword> string_to_keyword;



class MiscTrie {
public:
    #pragma region Verdicts
    enum class Verdict {
        none = 0,
        block_comment = 1,
        line_comment = 2,
        punct = 3,
        string_quote = 4,
    };
    #pragma endregion Verdicts 

    #pragma region Constructors
    MiscTrie() {}
    #pragma endregion Constructors

    #pragma region Service constructors
    MiscTrie(const MiscTrie &) = default;
    MiscTrie(MiscTrie &&) = default;
    MiscTrie &operator=(const MiscTrie &) = default;
    MiscTrie &operator=(MiscTrie &&) = default;
    #pragma endregion Service constructors

    #pragma region Interface
    Verdict feed(Scanner &scanner, Punct *punct, std::string_view *quote) {
        auto start_pos = scanner.tell();
    switch (scanner.cur()) {
    case '/': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '/': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "//"

                return Verdict::line_comment;
            } break;
            }
        } break;
        case '*': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "/*"

                return Verdict::block_comment;
            } break;
            }
        } break;
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "/="
                if (punct) {
                    *punct = Punct::SLASHEQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "/"
            if (punct) {
                *punct = Punct::SLASH;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '#': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "#"

            return Verdict::line_comment;
        } break;
        }
    } break;
    case '(': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "("
            if (punct) {
                *punct = Punct::LPAR;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case ')': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // ")"
            if (punct) {
                *punct = Punct::RPAR;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '[': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "["
            if (punct) {
                *punct = Punct::LSQB;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case ']': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "]"
            if (punct) {
                *punct = Punct::RSQB;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '{': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "{"
            if (punct) {
                *punct = Punct::LBRACE;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '}': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "}"
            if (punct) {
                *punct = Punct::RBRACE;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '!': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "!="
                if (punct) {
                    *punct = Punct::NOTEQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "!"
            if (punct) {
                *punct = Punct::EXCLAMATION;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case ':': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case ':': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "::"
                if (punct) {
                    *punct = Punct::DOUBLECOLON;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // ":"
            if (punct) {
                *punct = Punct::COLON;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '.': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '.': {
            scanner.advance();
            
            switch (scanner.cur()) {
            case '.': {
                scanner.advance();
                
                switch (scanner.cur()) {
                default: {
                    // "..."
                    if (punct) {
                        *punct = Punct::ELLIPSIS;  // TODO
                    }

                    return Verdict::punct;
                } break;
                }
            } break;
            default: {
                // "."
                scanner.seek(start_pos);
                scanner.advance(1);
                if (punct) {
                    *punct = Punct::DOT;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "."
            if (punct) {
                *punct = Punct::DOT;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case ',': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // ","
            if (punct) {
                *punct = Punct::COMMA;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case ';': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // ";"
            if (punct) {
                *punct = Punct::SEMI;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '+': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "+="
                if (punct) {
                    *punct = Punct::PLUSEQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "+"
            if (punct) {
                *punct = Punct::PLUS;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '-': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "-="
                if (punct) {
                    *punct = Punct::MINEQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        case '>': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "->"
                if (punct) {
                    *punct = Punct::RARROW;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "-"
            if (punct) {
                *punct = Punct::MINUS;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '*': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '*': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "**"
                if (punct) {
                    *punct = Punct::POWER;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "*="
                if (punct) {
                    *punct = Punct::STAREQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "*"
            if (punct) {
                *punct = Punct::STAR;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '%': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "%="
                if (punct) {
                    *punct = Punct::PERCENTEQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "%"
            if (punct) {
                *punct = Punct::PERCENT;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '~': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "~"
            if (punct) {
                *punct = Punct::TILDE;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '^': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "^="
                if (punct) {
                    *punct = Punct::CIRCUMFLEXEQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "^"
            if (punct) {
                *punct = Punct::CIRCUMFLEX;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '|': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "|="
                if (punct) {
                    *punct = Punct::VBAREQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "|"
            if (punct) {
                *punct = Punct::VBAR;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '&': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "&="
                if (punct) {
                    *punct = Punct::AMPEREQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "&"
            if (punct) {
                *punct = Punct::AMPER;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '<': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '<': {
            scanner.advance();
            
            switch (scanner.cur()) {
            case '=': {
                scanner.advance();
                
                switch (scanner.cur()) {
                default: {
                    // "<<="
                    if (punct) {
                        *punct = Punct::LEFTSHIFTEQUAL;  // TODO
                    }

                    return Verdict::punct;
                } break;
                }
            } break;
            default: {
                // "<<"
                if (punct) {
                    *punct = Punct::LEFTSHIFT;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            case '>': {
                scanner.advance();
                
                switch (scanner.cur()) {
                default: {
                    // "<=>"
                    if (punct) {
                        *punct = Punct::BIDIRCMP;  // TODO
                    }

                    return Verdict::punct;
                } break;
                }
            } break;
            default: {
                // "<="
                if (punct) {
                    *punct = Punct::LESSEQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "<"
            if (punct) {
                *punct = Punct::LESS;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '>': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '>': {
            scanner.advance();
            
            switch (scanner.cur()) {
            case '=': {
                scanner.advance();
                
                switch (scanner.cur()) {
                default: {
                    // ">>="
                    if (punct) {
                        *punct = Punct::RIGHTSHIFTEQUAL;  // TODO
                    }

                    return Verdict::punct;
                } break;
                }
            } break;
            default: {
                // ">>"
                if (punct) {
                    *punct = Punct::RIGHTSHIFT;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // ">="
                if (punct) {
                    *punct = Punct::GREATEREQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // ">"
            if (punct) {
                *punct = Punct::GREATER;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '=': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '=': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "=="
                if (punct) {
                    *punct = Punct::DOUBLEEQUAL;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        case '>': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "=>"
                if (punct) {
                    *punct = Punct::RARROW2;  // TODO
                }

                return Verdict::punct;
            } break;
            }
        } break;
        default: {
            // "="
            if (punct) {
                *punct = Punct::EQUAL;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '@': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "@"
            if (punct) {
                *punct = Punct::AT;  // TODO
            }

            return Verdict::punct;
        } break;
        }
    } break;
    case '\'': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '\'': {
            scanner.advance();
            
            switch (scanner.cur()) {
            case '\'': {
                scanner.advance();
                
                switch (scanner.cur()) {
                default: {
                    // "\'\'\'"
                    if (quote) {
                        *quote = "\'\'\'";
                    }

                    return Verdict::string_quote;
                } break;
                }
            } break;
            default: {
                // "\'"
                scanner.seek(start_pos);
                scanner.advance(1);
                if (quote) {
                    *quote = "\'";
                }

                return Verdict::string_quote;
            } break;
            }
        } break;
        default: {
            // "\'"
            if (quote) {
                *quote = "\'";
            }

            return Verdict::string_quote;
        } break;
        }
    } break;
    case '\"': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '\"': {
            scanner.advance();
            
            switch (scanner.cur()) {
            case '\"': {
                scanner.advance();
                
                switch (scanner.cur()) {
                default: {
                    // "\"\"\""
                    if (quote) {
                        *quote = "\"\"\"";
                    }

                    return Verdict::string_quote;
                } break;
                }
            } break;
            default: {
                // "\""
                scanner.seek(start_pos);
                scanner.advance(1);
                if (quote) {
                    *quote = "\"";
                }

                return Verdict::string_quote;
            } break;
            }
        } break;
        default: {
            // "\""
            if (quote) {
                *quote = "\"";
            }

            return Verdict::string_quote;
        } break;
        }
    } break;
    default: {
        // ""
        scanner.seek(start_pos);
        scanner.advance(0);

        return Verdict::none;
    } break;
    }
    }
    #pragma endregion Interface

};



class StringTrie {
public:
    #pragma region Verdicts
    enum class Verdict {
        none = 0,
        end_quote = 1,
        escape = 2,
        newline = 3,
    };
    #pragma endregion Verdicts 

    #pragma region Constructors
    StringTrie() {}
    #pragma endregion Constructors

    #pragma region Service constructors
    StringTrie(const StringTrie &) = default;
    StringTrie(StringTrie &&) = default;
    StringTrie &operator=(const StringTrie &) = default;
    StringTrie &operator=(StringTrie &&) = default;
    #pragma endregion Service constructors

    #pragma region Interface
    Verdict feed(Scanner &scanner, std::string_view *quote, int *escape) {
        auto start_pos = scanner.tell();
    switch (scanner.cur()) {
    case '\\': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "\\"
                if (escape) {
                    *escape = scanner.cur();
                    scanner.advance();
                }

            return Verdict::escape;
        } break;
        }
    } break;
    case '\n': {
        scanner.advance();
        
        switch (scanner.cur()) {
        default: {
            // "\n"

            return Verdict::newline;
        } break;
        }
    } break;
    case '\'': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '\'': {
            scanner.advance();
            
            switch (scanner.cur()) {
            case '\'': {
                scanner.advance();
                
                switch (scanner.cur()) {
                default: {
                    // "\'\'\'"
                        if (quote) {
                            *quote = "\'\'\'";
                        }

                    return Verdict::end_quote;
                } break;
                }
            } break;
            default: {
                // "\'"
                scanner.seek(start_pos);
                scanner.advance(1);
                    if (quote) {
                        *quote = "\'";
                    }

                return Verdict::end_quote;
            } break;
            }
        } break;
        default: {
            // "\'"
                if (quote) {
                    *quote = "\'";
                }

            return Verdict::end_quote;
        } break;
        }
    } break;
    case '\"': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '\"': {
            scanner.advance();
            
            switch (scanner.cur()) {
            case '\"': {
                scanner.advance();
                
                switch (scanner.cur()) {
                default: {
                    // "\"\"\""
                        if (quote) {
                            *quote = "\"\"\"";
                        }

                    return Verdict::end_quote;
                } break;
                }
            } break;
            default: {
                // "\""
                scanner.seek(start_pos);
                scanner.advance(1);
                    if (quote) {
                        *quote = "\"";
                    }

                return Verdict::end_quote;
            } break;
            }
        } break;
        default: {
            // "\""
                if (quote) {
                    *quote = "\"";
                }

            return Verdict::end_quote;
        } break;
        }
    } break;
    default: {
        // ""
        scanner.seek(start_pos);
        scanner.advance(0);

        return Verdict::none;
    } break;
    }
    }
    #pragma endregion Interface

};



class BlockCommentTrie {
public:
    #pragma region Verdicts
    enum class Verdict {
        none = 0,
        end = 1,
        start = 2,
    };
    #pragma endregion Verdicts 

    #pragma region Constructors
    BlockCommentTrie() {}
    #pragma endregion Constructors

    #pragma region Service constructors
    BlockCommentTrie(const BlockCommentTrie &) = default;
    BlockCommentTrie(BlockCommentTrie &&) = default;
    BlockCommentTrie &operator=(const BlockCommentTrie &) = default;
    BlockCommentTrie &operator=(BlockCommentTrie &&) = default;
    #pragma endregion Service constructors

    #pragma region Interface
    Verdict feed(Scanner &scanner) {
        auto start_pos = scanner.tell();
    switch (scanner.cur()) {
    case '/': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '*': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "/*"

                return Verdict::start;
            } break;
            }
        } break;
        default: {
            // ""
            scanner.seek(start_pos);
            scanner.advance(0);

            return Verdict::none;
        } break;
        }
    } break;
    case '*': {
        scanner.advance();
        
        switch (scanner.cur()) {
        case '/': {
            scanner.advance();
            
            switch (scanner.cur()) {
            default: {
                // "*/"

                return Verdict::end;
            } break;
            }
        } break;
        default: {
            // ""
            scanner.seek(start_pos);
            scanner.advance(0);

            return Verdict::none;
        } break;
        }
    } break;
    default: {
        // ""
        scanner.seek(start_pos);
        scanner.advance(0);

        return Verdict::none;
    } break;
    }
    }
    #pragma endregion Interface

};


}  // namespace bondrewd::lex
