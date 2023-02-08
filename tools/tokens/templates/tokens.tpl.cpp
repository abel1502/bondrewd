{{ _AUTOGENERATED_ }}
#include <bondrewd/lex/tokens.generated.hpp>


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


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
const std::unordered_map<std::string_view, Punct> string_to_punct = {
    {% for punct in tokens_info.puncts -%}
    { {{ punct.value }}, Punct::{{ punct.name }} },
    {% endfor %}
};

const std::unordered_map<std::string_view, HardKeyword> string_to_keyword = {
    {% for keyword in tokens_info.keywords -%}
    { {{ keyword.value }}, HardKeyword::{{ keyword.name }} },
    {% endfor %}
};
#pragma GCC diagnostic pop


} // namespace bondrewd::lex

