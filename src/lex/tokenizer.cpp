#include <bondrewd/lex/tokenizer.hpp>


namespace bondrewd::lex {


Token Tokenizer::get_token() {
    // Loop needed because of how comments are handled
    while (scanner) {
        scanner.skip_space();

        if (is_digit(scanner.cur()) || (scanner.cur() == '.' && is_digit(scanner.peek_next()))) {
            parse_number();
            return token;
        }

        if (is_name_char(scanner.cur())) {
            parse_name_or_keyword();
            return token;
        }

        if (parse_other()) {
            return token;
        }
    }

    token = Token::endmarker(scanner.tell(), "");
    return token;
}


int64_t Tokenizer::extract_integer(std::string_view digits, unsigned base, const SrcLocation &loc) {
    // Digits are assumed to be correct

    int64_t result = 0;
    bool allow_underscores = false;

    for (char digit : digits) {
        if (digit == '_') {
            if (!allow_underscores)
                error("Invalid underscore in integer literal", loc);
            
            allow_underscores = false;
            continue;
        }

        // TODO: Maybe make MSVC-compatible?
        if (__builtin_mul_overflow(result, base, &result))
            error("Integer literal too large", loc);
        if (__builtin_add_overflow(result, evaluate_digit(digit, base), &result))
            error("Integer literal too large", loc);
        
        allow_underscores = true;
    }

    if (!allow_underscores)
        error("Invalid underscore in integer literal", loc);

    return result;
}


double Tokenizer::extract_float(std::string_view integral_part,
                                std::string_view fractional_part,
                                std::string_view exponent_part,
                                int exponent_sign,
                                unsigned base,
                                const SrcLocation &loc) {
    // Digits are assumed to be correct

    double result = 0;

    for (auto digit : integral_part) {
        result *= base;
        result += evaluate_digit(digit, base);
    }

    double fraction = 0;
    for (auto digit : std::ranges::reverse_view(fractional_part)) {
        fraction += evaluate_digit(digit, base);
        fraction /= base;
    }

    result += fraction;

    if (exponent_sign != 0) {
        int64_t exponent = extract_integer(exponent_part, base, loc);

        result *= std::pow((double)base, exponent_sign * exponent);
    }

    return result;
}


void Tokenizer::parse_number() {
    auto start_pos = scanner.tell();

    unsigned base = 10;

    // Note: We don't parse sign here, because it will be treated as an unary operator instead
    // TODO: Maybe should do, in fact

    if (scanner.cur() == '0') {
        switch (scanner.peek_next()) {
        case 'B':
        case 'b':
            base = 2;
            scanner.advance(2);
            break;

        case 'O':
        case 'o':
            base = 8;
            scanner.advance(2);
            break;

        case 'X':
        case 'x':
            base = 16;
            scanner.advance(2);
            break;

        default:
            break;
        }
    }

    const auto pred_digit = [base](int c) { return c == '_' || evaluate_digit(c, base) >= 0; };

    std::string_view integral_part = scanner.read_while(pred_digit);

    bool is_integer = true;

    std::string_view fraction_part{};
    std::string_view exp_part{};
    int exp_sign = 0;

    if (scanner.cur() == '.' \
        && (!integral_part.empty() || evaluate_digit(scanner.peek_next(), base) >= 0)) {

        scanner.advance();
        is_integer = false;

        fraction_part = scanner.read_while(pred_digit);
    }

    if ((base <= 10 && (scanner.cur() == 'E' || scanner.cur() == 'e')) || \
        (base == 16 && (scanner.cur() == 'P' || scanner.cur() == 'p'))) {
        
        if (fraction_part.empty() && integral_part.empty()) {
            error("Invalid floating point literal", start_pos);
        }

        scanner.advance();
        is_integer = false;
        exp_sign = 1;

        switch (scanner.cur()) {
        case '+':
            scanner.advance();
            break;

        case '-':
            scanner.advance();
            exp_sign = -1;
            break;

        default:
            break;
        }

        exp_part = scanner.read_while(pred_digit);
    }

    if (is_integer && integral_part.empty()) {
        error("Invalid integer literal", start_pos);
    }

    if (is_name_char(scanner.cur())) {
        error(fmt::format("Garbage '{}' after number", (char)scanner.cur()), scanner.tell());
    }

    if (integral_part.size() >= MAX_NUMBER_LENGTH \
        || fraction_part.size() >= MAX_NUMBER_LENGTH \
        || exp_part.size() >= MAX_NUMBER_LENGTH) {

        error("Integer literal too large", start_pos);
    }

    if (is_integer) {
        token = Token::number(
            extract_integer(integral_part, base, start_pos),
            start_pos,
            scanner.view_since(start_pos)
        );
    } else {
        token = Token::number(
            extract_float(integral_part, fraction_part, exp_part, exp_sign, base, start_pos),
            start_pos,
            scanner.view_since(start_pos)
        );
    }

}


void Tokenizer::parse_name_or_keyword() {
    parse_name();
    assert(token.is_name());

    auto keyword = string_to_keyword.find(token.get_name().value);
    if (keyword != string_to_keyword.end()) {
        token = Token::keyword(keyword->second, token.get_location(), token.get_source());
    }
}


void Tokenizer::parse_name() {
    auto start_pos = scanner.tell();

    auto value = scanner.read_while<is_name_char>();
    assert(!value.empty());

    token = Token::name(value, start_pos, value);
}


bool Tokenizer::parse_other() {
    auto start_pos = scanner.tell();

    Punct punct{};
    std::string_view quote{};

    auto verdict = MiscTrie().feed(scanner, &punct, &quote);

    switch (verdict) {
    case MiscTrie::Verdict::none:
        error(fmt::format("Unexpected character '{}'", (char)scanner.cur()), start_pos);

    case MiscTrie::Verdict::string_quote:
        parse_string(quote);
        return true;

    case MiscTrie::Verdict::block_comment:
        parse_block_comment();
        return false;

    case MiscTrie::Verdict::line_comment:
        parse_line_comment();
        return false;

    case MiscTrie::Verdict::punct:
        token = Token::punct(punct, start_pos, scanner.view_since(start_pos));
        return true;

    NODEFAULT;
    }
}


void Tokenizer::parse_string(std::string_view start_quote) {
    auto start_pos = scanner.tell();
    std::string value = "";
    const bool is_multiline = start_quote.size() > 1;

    // Actually, this ended up less efficient than the implementation of this
    // I had in python (due to the fact that I'm using a separate switch-based trie here,
    // instead of walking an explicit one while iterating over the input).
    // The difference should be negligible, though.
    while (true) {
        auto seg_start_pos = scanner.tell();

        std::string_view end_quote{};
        int escape{};

        auto verdict = StringTrie().feed(scanner, &end_quote, &escape);

        switch (verdict) {
        case StringTrie::Verdict::none: {
            if (scanner.at_eof()) {
                error("Unterminated string", start_pos);
            }

            if (scanner.tell() == seg_start_pos) {
                // So that we don't spin forever in one place
                scanner.advance();
            }

            value += scanner.view_since(seg_start_pos);
        } break;

        case StringTrie::Verdict::end_quote: {
            if (end_quote == start_quote) {
                token = Token::string(value, start_quote, start_pos, value);
                return;
            }

            if (end_quote.starts_with(start_quote)) {
                // This is a bit of a hack for cases like `"abc"""`
                // We only want to consume the matching quote, not the extra ones
                scanner.seek(seg_start_pos);
                scanner.advance(end_quote.size());
            }

            value += scanner.view_since(seg_start_pos);
        } break;

        case StringTrie::Verdict::newline: {
            if (!is_multiline) {
                error("Unterminated string", start_pos);
            }

            value += '\n';
        } break;

        case StringTrie::Verdict::escape: {
            if (escape == '\n' || escape == Scanner::end_of_file) {
                break;
            }

            if (simple_escapes.contains((char)escape)) {
                value += simple_escapes.at((char)escape);
                break;
            }

            if (escape == 'x') {
                int digits[2] = {};
                digits[0] = scanner.cur();
                scanner.advance();
                digits[1] = scanner.cur();
                scanner.advance();

                // This automatically checks for invalid digits and EOF
                unsigned hex_value = evaluate_digit_checked(digits[0], 16, seg_start_pos) * 16 \
                                    + evaluate_digit_checked(digits[1], 16, seg_start_pos);
                assert(hex_value < 0x100);

                value += (char)hex_value;
                break;
            }

            error("Invalid escape sequence", seg_start_pos);
        } break;

        NODEFAULT;
        }
    }
}


void Tokenizer::parse_line_comment() {
    // TODO: Handle special comments, like pragmas?
    scanner.skip_line();
}


void Tokenizer::parse_block_comment() {
    // TODO: Allow for different types of block comments?
    auto start_pos = scanner.tell();

    unsigned balance = 1;

    while (balance) {
        auto verdict = BlockCommentTrie().feed(scanner);

        switch (verdict) {
        case BlockCommentTrie::Verdict::none: {
            if (scanner.at_eof()) {
                error("Unterminated block comment", start_pos);
            }

            scanner.advance();
        } break;

        case BlockCommentTrie::Verdict::start: {
            ++balance;
        } break;

        case BlockCommentTrie::Verdict::end: {
            --balance;
        } break;
        }
    }
}


const std::unordered_map<char, char> Tokenizer::simple_escapes = {
    // '\n', '' and 'x' are handled separately
    {'\'', '\''},
    {'\"', '\"'},
    {'\\', '\\'},
    {'a' , '\a'},
    {'b' , '\b'},
    {'f' , '\f'},
    {'n' , '\n'},
    {'r' , '\r'},
    {'t' , '\t'},
    {'v' , '\v'},
};


}  // namespace bondrewd::lex
