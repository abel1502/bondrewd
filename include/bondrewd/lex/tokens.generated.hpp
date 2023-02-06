// Placeholder; will be replaced by the generated file.

#pragma once

#include <string_view>


namespace bondrewd::lex {


enum class Punct {
    //
};


enum class HardKeyword {
    //
};


const char *punct_to_string(Punct value);


const char *keyword_to_string(HardKeyword value);


HardKeyword string_to_keyword(std::string_view value);


class PunctAutomata {
public:
    PunctAutomata() {}

    PunctAutomata(const PunctAutomata &) = default;
    PunctAutomata(PunctAutomata &&) = default;
    PunctAutomata &operator=(const PunctAutomata &) = default;
    PunctAutomata &operator=(PunctAutomata &&) = default;

    bool feed(char c, Punct &value) {
        //
    }

    void reset() {
        state = 0;
    }

protected:
    int state = 0;
};


}  // namespace bondrewd::lex
