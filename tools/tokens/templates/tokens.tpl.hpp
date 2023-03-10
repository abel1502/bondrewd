{{ _autogenerated_ }}
#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/scanner.hpp>

#include <string_view>
#include <cstdio>
#include <unordered_map>


namespace bondrewd::lex {


enum class Punct {
{%- for punct in tokens_info.puncts %}
    {{ punct.name }} = {{ loop.index0 }},
{%- endfor %}
};


enum class HardKeyword {
{%- for keyword in tokens_info.keywords %}
    {{ keyword.name }} = {{ loop.index0 }},
{%- endfor %}
};


const char *punct_to_string(Punct value);

const char *keyword_to_string(HardKeyword value);

extern const std::unordered_map<std::string_view, Punct> string_to_punct;

extern const std::unordered_map<std::string_view, HardKeyword> string_to_keyword;


{%- macro _walk_trie_node(node, depth=0) %}
    switch (scanner.cur()) {
    {%- for ch, child_node in node.children.items() %}
    case '{{ ch | cpp_escape }}': {
        scanner.advance();
        {% filter indent(width=4) %}
        {{- _walk_trie_node(child_node, caller=caller) }}
        {%- endfilter %}
    } break;
    {%- endfor %}
    default: {
        {%- filter indent(width=0) %}
        {{- caller(node) }}
        {%- endfilter %}
    } break;
    }
{%- endmacro %}


{%- macro gen_trie(name, trie_info, extra_args="") %}
class {{ name }} {
public:
    #pragma region Verdicts
    enum class Verdict {
        {%- for verdict in trie_info.verdicts %}
        {{ verdict }} = {{ loop.index0 }},
        {%- endfor %}
    };
    #pragma endregion Verdicts 

    #pragma region Constructors
    {{ name }}() {}
    #pragma endregion Constructors

    #pragma region Service constructors
    {{ name }}(const {{ name }} &) = default;
    {{ name }}({{ name }} &&) = default;
    {{ name }} &operator=(const {{ name }} &) = default;
    {{ name }} &operator=({{ name }} &&) = default;
    #pragma endregion Service constructors

    #pragma region Interface
    Verdict feed(Scanner &scanner{{ ', ' if extra_args }}{{ extra_args }}) {
        auto start_pos = scanner.tell();

        {%- set payload = caller %}
        {%- call(node) _walk_trie_node(trie_info.root) %}
        {%- set accepted_node = node.rollback %}
        // "{{ accepted_node.word | cpp_escape }}"
        {%- if node != accepted_node %}
        scanner.seek(start_pos);
        scanner.advance({{ accepted_node.word | length }});
        {%- endif %}
        {%- filter indent(width=4) %}
        {{- payload(accepted_node) }}
        {%- endfilter %}
        return Verdict::{{ accepted_node.verdict }};
        {%- endcall %}
    }
    #pragma endregion Interface

};

{%- endmacro %}


{% call(node) gen_trie("MiscTrie", tokens_info.misc_trie, extra_args="Punct *punct, std::string_view *quote") %}
    {%- if node.verdict == "string_quote" %}
    if (quote) {
        *quote = "{{ node.word | cpp_escape }}";
    }
    {%- elif node.verdict == "punct" %}
    if (punct) {
        *punct = Punct::{{ tokens_info.punct_lookup[node.word] }};  // TODO
    }
    {%- endif %}
{% endcall %}


{% call(node) gen_trie("StringTrie", tokens_info.string_trie, extra_args="std::string_view *quote, int *escape") %}
    {%- if node.verdict == "end_quote" %}
        if (quote) {
            *quote = "{{ node.word | cpp_escape }}";
        }
    {%- elif node.verdict == "escape" %}
        if (escape) {
            *escape = scanner.cur();
            scanner.advance();
        }
    {%- endif %}
{% endcall %}


{% call(node) gen_trie("BlockCommentTrie", tokens_info.block_comment_trie) %}
{% endcall %}


}  // namespace bondrewd::lex
