{{ _autogenerated_ }}
#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/parse/parser_base.hpp>

#include <map>
#include <optional>


#pragma region Subheader
{{ generator.gen_subheader() }}
#pragma endregion Subheader


namespace bondrewd::parse {


#pragma region Parser
class Parser : public ParserBase<Parser> {
public:
    #pragma region Constructors
    Parser(lex::Lexer lexer) : ParserBase(std::move(lexer)) {}
    #pragma endregion Constructors

    #pragma region Service constructors
    Parser(const Parser &) = delete;
    Parser(Parser &&) = default;
    Parser &operator=(const Parser &) = delete;
    Parser &operator=(Parser &&) = default;
    #pragma endregion Service constructors

    #pragma region API
    {{ generator.gen_api() }}
    #pragma endregion API

protected:
    #pragma region Constants and typedefs
    using state_t = lex::Lexer::state_t;
    #pragma endregion Constants and typedefs

    #pragma region Fields
    {%- for rulename, rule in generator.all_rules_sorted %}
    {%- if generator.should_cache(rule) %}
    std::map<state_t, {{ rule.type }}> cache_{{ rulename }}{};
    {%- endif %}
    {%- endfor %}
    #pragma endregion Fields

    #pragma region Rule types
    enum class RuleType: unsigned {
        {%- for rulename, rule in generator.all_rules_sorted %}
        {{ rulename }},{{ "  // Left-recursive" if rule.left_recursive else "" }}
        {%- endfor %}
    };

    static constexpr bool is_left_recursive(RuleType rule) {
        switch (rule) {
            {%- for rulename, rule in generator.all_rules_sorted if rule.left_recursive %}
            case RuleType::{{ rulename }}:  return true;
            {%- endfor %}
            default:  return false;
        }
    }

    template <RuleType R>
    using rule_result_t = std::tuple_element_t<
        (unsigned)R,
        std::tuple<
            {%- for rulename, rule in generator.all_rules_sorted %}
            {{ rule.type }},
            {%- endfor %}
        >
    >;
    #pragma endregion Rule types

    #pragma region Helpers
    bool lookahead(bool positive, auto rule_func) {
        auto guard = lexer.lookahead();

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

    #pragma region CacheHelper
    template <RuleType rule_type>
    class CacheHelper {
    public:
        using result_t = std::optional<rule_result_t<rule_type>>;

        CacheHelper(Parser &parser, state_t state, result_t &result) :
            parser{parser}, state{std::move(state)}, rule_type{rule_type}, result{result} {}
        
        CacheHelper(const CacheHelper &) = delete;
        CacheHelper(CacheHelper &&) = delete;
        CacheHelper &operator=(const CacheHelper &) = delete;
        CacheHelper &operator=(CacheHelper &&) = delete;

        void accept() {
            is_success = true;
        }

        ~CacheHelper() {
            if (is_success) {
                parser.store_cached<rule_type>(state, result);
            }
        }
        
    private:
        Parser &parser;
        state_t state;
        result_t &result;
        bool is_success = false;
    
    };
    #pragma endregion CacheHelper

    #pragma region Caching
    template <RuleType rule_type>
    constexpr std::map<state_t, rule_result_t<rule_type>> &_get_cache() {
        {%- for rulename, rule in generator.all_rules_sorted if generator.should_cache(rule) %}
        if constexpr (rule_type == RuleType::{{ rulename }}) {
            return cache_{{ rulename }};
        } else {% endfor %} {
            static_assert(false, "Uncachable rule type");
        }
    }

    template <RuleType rule_type>
    std::optional<rule_result_t<rule_type>> get_cached(state_t state) {
        auto &cache = _get_cache<rule_type>();

        auto it = cache.find(state);

        if (it == cache.end()) {
            return std::nullopt;
        }

        return it->second;
    }

    // Updates, if present
    template <RuleType rule_type>
    void store_cached(state_t state, std::optional<rule_result_t<rule_type>> result) {
        auto &cache = _get_cache<rule_type>();

        cache.emplace(state, std::move(result));
    }
    #pragma endregion Caching

    #pragma region Rule parsers
    {{ generator.gen_rule_parsers() }}
    #pragma endregion Rule parsers

};
#pragma endregion Parser


}  // namespace bondrewd::parse
