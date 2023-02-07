// AUTOGENERATED by tools/generate_tokens.py on {{ date }}.
#include <bondrewd/lex/tokens.generated.hpp>

#include <unordered_map>


namespace bondrewd::lex {


const char *punct_to_string(Punct value) {
    switch (value) {
        {% for punct in tokens_info.puncts -%}
        case Punct::{{ punct.name }}: return {{ punct.value }};
        {% endfor %}
    }
}

const char *keyword_to_string(HardKeyword value) {
    switch (value) {
        {% for keyword in tokens_info.keywords -%}
        case HardKeyword::{{ keyword.name }}: return {{ keyword.value }};
        {% endfor %}
    }
}

Punct string_to_punct(std::string_view value) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wexit-time-destructors"
    static const std::unordered_map<std::string_view, Punct> map = {
        {% for punct in tokens_info.puncts -%}
        { {{ punct.value }}, Punct::{{ punct.name }} },
        {% endfor %}
    };
    #pragma GCC diagnostic pop

    return map.at(value);
}

HardKeyword string_to_keyword(std::string_view value) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wexit-time-destructors"
    static const std::unordered_map<std::string_view, HardKeyword> map = {
        {% for keyword in tokens_info.keywords -%}
        { {{ keyword.value }}, HardKeyword::{{ keyword.name }} },
        {% endfor %}
    };
    #pragma GCC diagnostic pop

    return map.at(value);
}


DEFINE_DECLARED(class MiscTrie);
DEFINE_DECLARED(class StringTrie);
DEFINE_DECLARED(class BlockCommentTrie);


} // namespace bondrewd::lex

