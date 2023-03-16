{{ _autogenerated_ }}
#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/parse/parser_base.hpp>
#include <bondrewd/ast/ast_nodes.gen.hpp>
#include <bondrewd/lex/src_location.hpp>

#include <map>
#include <vector>
#include <optional>
#include <algorithm>
#include <fmt/format.h>


#pragma region Subheader
{{ generator.gen_subheader() }}
#pragma endregion Subheader


namespace bondrewd::parse {


#pragma region Parser
class Parser : public ParserBase<Parser> {
public:
    #pragma region Constructors
    Parser(lex::Lexer lexer_) : ParserBase(std::move(lexer_)) {}
    #pragma endregion Constructors

    #pragma region Service constructors
    Parser(const Parser &) = delete;
    Parser(Parser &&) = default;
    Parser &operator=(const Parser &) = delete;
    Parser &operator=(Parser &&) = default;
    #pragma endregion Service constructors

    #pragma region API
    auto parse() {
        auto result = parse_start_rule();

        if (!result) {
            auto &scanner = lexer.get_scanner();

            lex::SrcLocation err_loc = scanner.tell();

            throw SyntaxError(fmt::format("Syntax error at {} (`{}`)", err_loc.to_string(), scanner.view_context(err_loc, 5)));
        }

        return *result;
    }
    #pragma endregion API

protected:
    #pragma region Constants and typedefs
    using state_t = lex::Lexer::state_t;

    static constexpr unsigned MAX_RECURSION_LEVEL = 6000;
    #pragma endregion Constants and typedefs

    #pragma region Fields
    unsigned _level = 0;

    // Cache-related fields in a following region
    #pragma endregion Fields

    #pragma region Rule types
    enum class RuleType: unsigned {
        {%- for rulename, rule in generator.all_rules_sorted %}
        {{ rulename }},{{ "  // Left-recursive" if rule.left_recursive else "" }}
        {%- endfor %}
    };

    template <RuleType R>
    using rule_raw_result_t = std::tuple_element_t<
        (unsigned)R,
        std::tuple<
            {%- for rulename, rule in generator.all_rules_sorted %}
            {{ rule.type }}{{ "," if not loop.last else "" }}
            {%- endfor %}
        >
    >;

    template <RuleType R>
    using rule_result_t = std::optional<rule_raw_result_t<R>>;
    #pragma endregion Rule types

    #pragma region Helpers
    bool lookahead(bool positive, auto rule_func) {
        auto guard = lexer.lookahead(positive);

        // TODO: Pass this?
        return positive == (bool)rule_func();
    }

    template <typename U>
    U forced(U result) {
        if (!result) {
            // TODO: More info?
            fatal_error("Forced rule failed");
        }

        return std::move(result);
    }

    state_t tell() {
        return lexer.tell();
    }

    void seek(state_t state) {
        lexer.seek(state);
    }
    #pragma endregion Helpers

    #pragma region Extras
    {% filter indent(4) -%}
    {{ generator.gen_extras() }}
    {%- endfilter %}
    #pragma endregion Extras

    #pragma region Caching
    #pragma region CacheNode
    template <RuleType rule_type>
    class CacheNode {
    public:
        rule_result_t<rule_type> value;
        state_t end_state;

        CacheNode(rule_result_t<rule_type> value, state_t end_state)
            : value{std::move(value)}, end_state{std::move(end_state)} {}
        
        CacheNode(const CacheNode &) = delete;
        CacheNode(CacheNode &&) = default;
        CacheNode &operator=(const CacheNode &) = delete;
        CacheNode &operator=(CacheNode &&) = default;

    };
    #pragma endregion CacheNode

    #pragma region Fields
    {% for rulename, rule in generator.all_rules_sorted -%}
    {% if generator.should_cache(rule, include_left_recursive=True) -%}
    std::map<state_t, CacheNode<RuleType::{{ rulename }}>> cache_{{ rulename }}{};
    {% endif %}
    {%- endfor %}
    #pragma endregion Fields

    template <RuleType rule_type>
    constexpr std::map<state_t, CacheNode<rule_type>> &_get_cache() {
        {%- for rulename, rule in generator.all_rules_sorted if generator.should_cache(rule, include_left_recursive=True) %}
        if constexpr (rule_type == RuleType::{{ rulename }}) {
            return cache_{{ rulename }};
        } else {% endfor %} {
            // Hack to bypass msvc's static_assert in templates optimization
            // (it was triggered despite being within a non-instantiated template function...)
            static_assert(false && rule_type == rule_type, "Uncachable rule type");
        }
    }

    template <RuleType rule_type>
    std::optional<rule_result_t<rule_type>> get_cached(state_t state) {
        auto &cache = _get_cache<rule_type>();

        auto it = cache.find(state);

        if (it == cache.end()) {
            return std::nullopt;
        }

        seek(it->second.end_state);

        return it->second.value;
    }

    /// Updates, if present
    template <RuleType rule_type>
    void store_cached(state_t state, rule_result_t<rule_type> result) {
        auto &cache = _get_cache<rule_type>();

        cache.insert_or_assign(state, CacheNode<rule_type>(std::move(result), tell()));
    }
    #pragma endregion Caching

    #pragma region Rule parsers
    {% filter indent(4) -%}
    {{ generator.gen_rule_parsers(with_impls=False) }}
    {%- endfilter %}
    #pragma endregion Rule parsers

};
#pragma endregion Parser


}  // namespace bondrewd::parse
