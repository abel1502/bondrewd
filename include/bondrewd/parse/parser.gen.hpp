// AUTOGENERATED by bondrewd/tools/pegen++/pegenxx.py on 2023-06-07 19:23:02
// DO NOT EDIT

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
#include <string>


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
        _gather_29,
        _gather_31,
        _gather_5,
        _gather_8,
        _loop0_1,
        _loop0_24,
        _loop0_25,
        _loop0_26,
        _loop0_3,
        _loop0_30,
        _loop0_32,
        _loop0_33,
        _loop0_6,
        _loop0_7,
        _loop0_9,
        _loop1_14,
        _loop1_15,
        _loop1_16,
        _loop1_27,
        _loop1_28,
        _tmp_10,
        _tmp_11,
        _tmp_12,
        _tmp_13,
        _tmp_17,
        _tmp_18,
        _tmp_19,
        _tmp_2,
        _tmp_20,
        _tmp_21,
        _tmp_22,
        _tmp_23,
        _tmp_34,
        _tmp_35,
        _tmp_36,
        _tmp_37,
        _tmp_4,
        and_expr,
        any_paren,
        any_token,
        arg_spec,
        args_spec,
        args_spec_nonempty,
        arithm_expr,
        array_expr,
        assign_op,
        assign_stmt,
        attr_name,
        bidir_cmp_expr,
        bitand_expr,  // Left-recursive
        bitor_expr,  // Left-recursive
        bitwise_expr,
        bitxor_expr,  // Left-recursive
        block_expr,
        break_expr,
        call_args,
        call_expr,  // Left-recursive
        cartridge_header_stmt,
        colon_attr_expr,  // Left-recursive
        comparison_expr,
        comparison_followup_pair,
        comparison_op,
        continue_expr,
        ctime_block_expr,
        defn,
        defn_block,
        defn_expr,
        dot_attr_expr,  // Left-recursive
        expand_expr,
        expr,
        expr_0,
        expr_1,
        expr_2,
        expr_3,
        expr_4,
        expr_5,  // Left-recursive
        expr_6,
        expr_or_unit,
        expr_stmt,
        file,
        flow,
        flow_block,
        flow_control_expr,
        flow_expr,
        for_flow,
        func_body,
        func_def,
        group_expr,
        if_flow,
        impl_def,
        infix_call_expr,
        loop_flow,
        macro_call_expr,  // Left-recursive
        modulo_expr,
        name,
        not_expr,
        ns_def,
        ns_spec,
        ns_spec_raw,
        or_expr,
        pass_spec_expr,
        pass_stmt,
        power_expr,
        primary_expr,
        product_bin_op,
        product_expr,  // Left-recursive
        raw_defn,
        raw_flow,
        return_expr,
        shift_bin_op,
        shift_expr,  // Left-recursive
        start,
        stmt,
        strings,
        struct_def,
        subscript_expr,  // Left-recursive
        sum_bin_op,
        sum_expr,  // Left-recursive
        token_stream,
        token_stream_delim,
        token_stream_no_parens,
        tuple_expr,
        type_annotation,
        unary_expr,
        unary_op,
        var_def,
        var_ref_expr,
        weak_expr,
        while_flow,
        xtime_flag,
    };

    template <RuleType R>
    using rule_raw_result_t = std::tuple_element_t<
        (unsigned)R,
        std::tuple<
            ast::sequence<ast::expr>,
            ast::sequence<ast::expr>,
            std::vector<std::string>,
            ast::sequence<ast::arg_spec>,
            ast::sequence<ast::stmt>,
            ast::sequence<ast::expr>,
            ast::sequence<ast::expr>,
            ast::sequence<ast::expr>,
            ast::sequence<ast::stmt>,
            ast::sequence<ast::expr>,
            ast::sequence<ast::expr>,
            ast::sequence<ast::stmt>,
            std::vector<std::string>,
            ast::sequence<ast::arg_spec>,
            ast::sequence<ast::arg_spec>,
            ast::sequence<ast::expr>,
            ast::sequence<ast::expr>,
            std::vector<std::pair < ast::field < ast::cmp_op > , ast::field < ast::expr >>>,
            std::vector<lex::Token>,
            std::vector<lex::Token>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::arg_spec>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            lex::Token,
            std::monostate,
            ast::field<ast::expr>,
            std::monostate,
            lex::Token,
            ast::field<ast::arg_spec>,
            ast::field<ast::args_spec>,
            ast::field<ast::args_spec>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::assign_op>,
            ast::field<ast::stmt>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::call_args>,
            ast::field<ast::expr>,
            ast::field<ast::stmt>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            std::pair < ast::field < ast::cmp_op > , ast::field < ast::expr >>,
            ast::field<ast::cmp_op>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::defn>,
            ast::sequence < ast::stmt >,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::stmt>,
            ast::field<ast::file>,
            ast::field<ast::flow>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::flow>,
            ast::field<ast::expr>,
            ast::field<ast::defn>,
            ast::field<ast::expr>,
            ast::field<ast::flow>,
            ast::field<ast::defn>,
            ast::field<ast::expr>,
            ast::field<ast::flow>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            std::string,
            ast::field<ast::expr>,
            ast::field<ast::defn>,
            ast::field<ast::defn>,
            ast::sequence < ast::identifier >,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::stmt>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::binary_op>,
            ast::field<ast::expr>,
            ast::field<ast::defn>,
            ast::field<ast::flow>,
            ast::field<ast::expr>,
            ast::field<ast::binary_op>,
            ast::field<ast::expr>,
            ast::field<ast::file>,
            ast::field<ast::stmt>,
            std::string,
            ast::field<ast::defn>,
            ast::field<ast::expr>,
            ast::field<ast::binary_op>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::unary_op>,
            ast::field<ast::defn>,
            ast::field<ast::expr>,
            ast::field<ast::expr>,
            ast::field<ast::flow>,
            ast::field<ast::xtime_flag>
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
    std::string _concat_strings(const std::vector<lex::Token> &strings) const {
        if (strings.empty()) {
            return "";
        }

        std::string result{};
        std::string_view quotes = "";
        bool first = true;

        for (auto &s: strings) {
            if (first) {
                quotes = s.get_string().quotes;
                first = false;
            }

            result += s.get_string().value;
            if (s.get_string().quotes != quotes) {
                // TODO: Custom error type!
                throw std::runtime_error("String literals must have the same quotes");
            }
        }

        return result;
    }

    template <typename T>
    ast::maybe<T> _opt2maybe(std::optional<ast::field<T>> opt) {
        if (opt) {
            return std::move(*opt);
        } else {
            return nullptr;
        }
    }

    template <typename T>
    ast::sequence<T> _prepend1(ast::field<T> item, ast::sequence<T> seq) {
        assert(item);
        seq.insert(seq.begin(), std::move(*item));
        return seq;
    }



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
    std::map<state_t, CacheNode<RuleType::bitand_expr>> cache_bitand_expr{};
    std::map<state_t, CacheNode<RuleType::bitor_expr>> cache_bitor_expr{};
    std::map<state_t, CacheNode<RuleType::bitxor_expr>> cache_bitxor_expr{};
    std::map<state_t, CacheNode<RuleType::block_expr>> cache_block_expr{};
    std::map<state_t, CacheNode<RuleType::call_args>> cache_call_args{};
    std::map<state_t, CacheNode<RuleType::defn>> cache_defn{};
    std::map<state_t, CacheNode<RuleType::expr>> cache_expr{};
    std::map<state_t, CacheNode<RuleType::expr_0>> cache_expr_0{};
    std::map<state_t, CacheNode<RuleType::expr_4>> cache_expr_4{};
    std::map<state_t, CacheNode<RuleType::expr_5>> cache_expr_5{};
    std::map<state_t, CacheNode<RuleType::flow>> cache_flow{};
    std::map<state_t, CacheNode<RuleType::product_expr>> cache_product_expr{};
    std::map<state_t, CacheNode<RuleType::shift_expr>> cache_shift_expr{};
    std::map<state_t, CacheNode<RuleType::stmt>> cache_stmt{};
    std::map<state_t, CacheNode<RuleType::strings>> cache_strings{};
    std::map<state_t, CacheNode<RuleType::sum_expr>> cache_sum_expr{};
    
    #pragma endregion Fields

    template <RuleType rule_type>
    constexpr std::map<state_t, CacheNode<rule_type>> &_get_cache() {
        if constexpr (rule_type == RuleType::bitand_expr) {
            return cache_bitand_expr;
        } else 
        if constexpr (rule_type == RuleType::bitor_expr) {
            return cache_bitor_expr;
        } else 
        if constexpr (rule_type == RuleType::bitxor_expr) {
            return cache_bitxor_expr;
        } else 
        if constexpr (rule_type == RuleType::block_expr) {
            return cache_block_expr;
        } else 
        if constexpr (rule_type == RuleType::call_args) {
            return cache_call_args;
        } else 
        if constexpr (rule_type == RuleType::defn) {
            return cache_defn;
        } else 
        if constexpr (rule_type == RuleType::expr) {
            return cache_expr;
        } else 
        if constexpr (rule_type == RuleType::expr_0) {
            return cache_expr_0;
        } else 
        if constexpr (rule_type == RuleType::expr_4) {
            return cache_expr_4;
        } else 
        if constexpr (rule_type == RuleType::expr_5) {
            return cache_expr_5;
        } else 
        if constexpr (rule_type == RuleType::flow) {
            return cache_flow;
        } else 
        if constexpr (rule_type == RuleType::product_expr) {
            return cache_product_expr;
        } else 
        if constexpr (rule_type == RuleType::shift_expr) {
            return cache_shift_expr;
        } else 
        if constexpr (rule_type == RuleType::stmt) {
            return cache_stmt;
        } else 
        if constexpr (rule_type == RuleType::strings) {
            return cache_strings;
        } else 
        if constexpr (rule_type == RuleType::sum_expr) {
            return cache_sum_expr;
        } else  {
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
    
    // start: file
    std::optional<ast::field<ast::file>> parse_start_rule();

    // file: stmt* $
    std::optional<ast::field<ast::file>> parse_file_rule();

    // stmt: cartridge_header_stmt | assign_stmt | expr_stmt | pass_stmt
    std::optional<ast::field<ast::stmt>> parse_stmt_rule();

    // cartridge_header_stmt: 'cartridge' name ';'
    std::optional<ast::field<ast::stmt>> parse_cartridge_header_stmt_rule();

    // assign_stmt: expr assign_op expr ';'
    std::optional<ast::field<ast::stmt>> parse_assign_stmt_rule();

    // assign_op: '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '<<=' | '>>=' | '&=' | '|=' | '^='
    std::optional<ast::field<ast::assign_op>> parse_assign_op_rule();

    // expr_stmt: expr ';'
    std::optional<ast::field<ast::stmt>> parse_expr_stmt_rule();

    // pass_stmt: ';'
    std::optional<ast::field<ast::stmt>> parse_pass_stmt_rule();

    // defn: xtime_flag raw_defn
    std::optional<ast::field<ast::defn>> parse_defn_rule();

    // raw_defn: var_def | func_def | struct_def | impl_def | ns_def
    std::optional<ast::field<ast::defn>> parse_raw_defn_rule();

    // var_def: 'var' name type_annotation? ['=' expr] ';'
    std::optional<ast::field<ast::defn>> parse_var_def_rule();

    // func_def: 'func' name? '(' args_spec ')' type_annotation? func_body
    std::optional<ast::field<ast::defn>> parse_func_def_rule();

    // func_body: '=>' expr | block_expr
    std::optional<ast::field<ast::expr>> parse_func_body_rule();

    // impl_def: 'impl' expr defn_block | 'impl' expr 'for' expr defn_block
    std::optional<ast::field<ast::defn>> parse_impl_def_rule();

    // defn_block: '{' stmt* '}'
    std::optional<ast::sequence < ast::stmt >> parse_defn_block_rule();

    // struct_def: ('class' | 'struct') name? args_spec
    std::optional<ast::field<ast::defn>> parse_struct_def_rule();

    // ns_def: 'ns' ns_spec
    std::optional<ast::field<ast::defn>> parse_ns_def_rule();

    // ns_spec: 'cartridge' '::' ns_spec_raw | ns_spec_raw
    std::optional<ast::field<ast::defn>> parse_ns_spec_rule();

    // ns_spec_raw: '::'.name+
    std::optional<ast::sequence < ast::identifier >> parse_ns_spec_raw_rule();

    // args_spec: args_spec_nonempty ','? | 
    std::optional<ast::field<ast::args_spec>> parse_args_spec_rule();

    // args_spec_nonempty: "self" ((',' arg_spec))* | ','.arg_spec+
    std::optional<ast::field<ast::args_spec>> parse_args_spec_nonempty_rule();

    // arg_spec: name type_annotation [('=' expr)]
    std::optional<ast::field<ast::arg_spec>> parse_arg_spec_rule();

    // flow: 'unwrap' raw_flow | raw_flow
    std::optional<ast::field<ast::flow>> parse_flow_rule();

    // raw_flow: if_flow | for_flow | while_flow | loop_flow
    std::optional<ast::field<ast::flow>> parse_raw_flow_rule();

    // if_flow: 'if' expr flow_block [('else' flow_block)]
    std::optional<ast::field<ast::flow>> parse_if_flow_rule();

    // for_flow: 'for' name 'in' expr flow_block [('else' flow_block)]
    std::optional<ast::field<ast::flow>> parse_for_flow_rule();

    // while_flow: 'while' expr flow_block [('else' flow_block)]
    std::optional<ast::field<ast::flow>> parse_while_flow_rule();

    // loop_flow: 'loop' flow_block
    std::optional<ast::field<ast::flow>> parse_loop_flow_rule();

    // flow_block: block_expr | flow_expr | flow_control_expr
    std::optional<ast::field<ast::expr>> parse_flow_block_rule();

    // expr_or_unit: expr | 
    std::optional<ast::field<ast::expr>> parse_expr_or_unit_rule();

    // expr: defn_expr | flow_expr | expr_0
    std::optional<ast::field<ast::expr>> parse_expr_rule();

    // defn_expr: defn
    std::optional<ast::field<ast::expr>> parse_defn_expr_rule();

    // flow_expr: flow
    std::optional<ast::field<ast::expr>> parse_flow_expr_rule();

    // expr_0: and_expr | or_expr | expr_1
    std::optional<ast::field<ast::expr>> parse_expr_0_rule();

    // and_expr: expr_2 (('and' expr_1))+
    std::optional<ast::field<ast::expr>> parse_and_expr_rule();

    // or_expr: expr_2 (('or' expr_1))+
    std::optional<ast::field<ast::expr>> parse_or_expr_rule();

    // expr_1: not_expr | expand_expr | pass_spec_expr | flow_control_expr | expr_2
    std::optional<ast::field<ast::expr>> parse_expr_1_rule();

    // not_expr: 'not' expr_1
    std::optional<ast::field<ast::expr>> parse_not_expr_rule();

    // expand_expr: 'expand' expr_1
    std::optional<ast::field<ast::expr>> parse_expand_expr_rule();

    // pass_spec_expr: 'ref' expr_1 | 'move' expr_1 | 'copy' expr_1
    std::optional<ast::field<ast::expr>> parse_pass_spec_expr_rule();

    // flow_control_expr: return_expr | break_expr | continue_expr
    std::optional<ast::field<ast::expr>> parse_flow_control_expr_rule();

    // return_expr: 'return' expr_or_unit
    std::optional<ast::field<ast::expr>> parse_return_expr_rule();

    // break_expr: 'break' expr_or_unit
    std::optional<ast::field<ast::expr>> parse_break_expr_rule();

    // continue_expr: 'continue'
    std::optional<ast::field<ast::expr>> parse_continue_expr_rule();

    // expr_2: comparison_expr | bidir_cmp_expr | expr_3
    std::optional<ast::field<ast::expr>> parse_expr_2_rule();

    // comparison_expr: expr_3 comparison_followup_pair+
    std::optional<ast::field<ast::expr>> parse_comparison_expr_rule();

    // comparison_followup_pair: comparison_op expr_3
    std::optional<std::pair < ast::field < ast::cmp_op > , ast::field < ast::expr >>> parse_comparison_followup_pair_rule();

    // comparison_op: '==' | '!=' | '<' | '<=' | '>' | '>=' | 'in' | 'not' 'in'
    std::optional<ast::field<ast::cmp_op>> parse_comparison_op_rule();

    // bidir_cmp_expr: expr_3 '<=>' expr_3
    std::optional<ast::field<ast::expr>> parse_bidir_cmp_expr_rule();

    // expr_3: arithm_expr | bitwise_expr | expr_4
    std::optional<ast::field<ast::expr>> parse_expr_3_rule();

    // arithm_expr: sum_expr | product_expr | modulo_expr
    std::optional<ast::field<ast::expr>> parse_arithm_expr_rule();

    // Left-recursive
    // sum_expr: (sum_expr | product_expr) sum_bin_op product_expr
    std::optional<ast::field<ast::expr>> parse_sum_expr_rule();
    std::optional<ast::field<ast::expr>> parse_raw_sum_expr();

    // sum_bin_op: '+' | '-'
    std::optional<ast::field<ast::binary_op>> parse_sum_bin_op_rule();

    // Left-recursive
    // product_expr: (product_expr | expr_4) product_bin_op expr_4
    std::optional<ast::field<ast::expr>> parse_product_expr_rule();
    std::optional<ast::field<ast::expr>> parse_raw_product_expr();

    // product_bin_op: '*' | '/'
    std::optional<ast::field<ast::binary_op>> parse_product_bin_op_rule();

    // modulo_expr: expr_4 '%' expr_4
    std::optional<ast::field<ast::expr>> parse_modulo_expr_rule();

    // bitwise_expr: bitor_expr | bitand_expr | bitxor_expr | shift_expr
    std::optional<ast::field<ast::expr>> parse_bitwise_expr_rule();

    // Left-recursive
    // bitor_expr: (bitor_expr | expr_4) '|' expr_4
    std::optional<ast::field<ast::expr>> parse_bitor_expr_rule();
    std::optional<ast::field<ast::expr>> parse_raw_bitor_expr();

    // Left-recursive
    // bitand_expr: (bitand_expr | expr_4) '&' expr_4
    std::optional<ast::field<ast::expr>> parse_bitand_expr_rule();
    std::optional<ast::field<ast::expr>> parse_raw_bitand_expr();

    // Left-recursive
    // bitxor_expr: (bitxor_expr | expr_4) '^' expr_4
    std::optional<ast::field<ast::expr>> parse_bitxor_expr_rule();
    std::optional<ast::field<ast::expr>> parse_raw_bitxor_expr();

    // Left-recursive
    // shift_expr: (shift_expr | expr_4) shift_bin_op expr_4
    std::optional<ast::field<ast::expr>> parse_shift_expr_rule();
    std::optional<ast::field<ast::expr>> parse_raw_shift_expr();

    // shift_bin_op: '<<' | '>>'
    std::optional<ast::field<ast::binary_op>> parse_shift_bin_op_rule();

    // expr_4: unary_expr | power_expr | expr_5
    std::optional<ast::field<ast::expr>> parse_expr_4_rule();

    // unary_expr: unary_op (unary_expr | expr_5)
    std::optional<ast::field<ast::expr>> parse_unary_expr_rule();

    // unary_op: '+' | '-' | '~' | '&' | '*'
    std::optional<ast::field<ast::unary_op>> parse_unary_op_rule();

    // power_expr: expr_5 '**' expr_5
    std::optional<ast::field<ast::expr>> parse_power_expr_rule();

    // Left-recursive
    // expr_5:
    //     | dot_attr_expr
    //     | colon_attr_expr
    //     | call_expr
    //     | macro_call_expr
    //     | subscript_expr
    //     | expr_6
    std::optional<ast::field<ast::expr>> parse_expr_5_rule();
    std::optional<ast::field<ast::expr>> parse_raw_expr_5();

    // Left-recursive
    // dot_attr_expr: expr_5 '.' name
    std::optional<ast::field<ast::expr>> parse_dot_attr_expr_rule();

    // Left-recursive
    // colon_attr_expr: expr_5 '::' name
    std::optional<ast::field<ast::expr>> parse_colon_attr_expr_rule();

    // Left-recursive
    // call_expr: expr_5 '(' call_args ')'
    std::optional<ast::field<ast::expr>> parse_call_expr_rule();

    // Left-recursive
    // macro_call_expr: expr_5 '!' token_stream_delim
    std::optional<ast::field<ast::expr>> parse_macro_call_expr_rule();

    // Left-recursive
    // subscript_expr: expr_5 '[' call_args ']'
    std::optional<ast::field<ast::expr>> parse_subscript_expr_rule();

    // call_args: 
    std::optional<ast::field<ast::call_args>> parse_call_args_rule();

    // token_stream: token_stream_delim | token_stream_no_parens
    std::optional<ast::field<ast::expr>> parse_token_stream_rule();

    // token_stream_delim:
    //     | '(' ~ token_stream* ')'
    //     | '[' ~ token_stream* ']'
    //     | '{' ~ token_stream* '}'
    std::optional<ast::field<ast::expr>> parse_token_stream_delim_rule();

    // token_stream_no_parens: ((!any_paren any_token))+
    std::optional<ast::field<ast::expr>> parse_token_stream_no_parens_rule();

    // any_paren: '(' | ')' | '[' | ']' | '{' | '}'
    std::optional<std::monostate> parse_any_paren_rule();

    // any_token: NAME | NUMBER | STRING | KEYWORD | PUNCT
    std::optional<lex::Token> parse_any_token_rule();

    // expr_6: primary_expr
    std::optional<ast::field<ast::expr>> parse_expr_6_rule();

    // primary_expr:
    //     | NUMBER
    //     | &STRING strings
    //     | '...'
    //     | var_ref_expr
    //     | group_expr
    //     | tuple_expr
    //     | array_expr
    //     | ctime_block_expr
    //     | block_expr
    std::optional<ast::field<ast::expr>> parse_primary_expr_rule();

    // var_ref_expr: name
    std::optional<ast::field<ast::expr>> parse_var_ref_expr_rule();

    // strings: STRING+
    std::optional<std::string> parse_strings_rule();

    // group_expr: '(' weak_expr ')'
    std::optional<ast::field<ast::expr>> parse_group_expr_rule();

    // tuple_expr: '(' ')' | '(' ','.expr+ ','? ')'
    std::optional<ast::field<ast::expr>> parse_tuple_expr_rule();

    // array_expr: '[' ']' | '[' ','.expr+ ','? ']'
    std::optional<ast::field<ast::expr>> parse_array_expr_rule();

    // ctime_block_expr: 'ctime' block_expr
    std::optional<ast::field<ast::expr>> parse_ctime_block_expr_rule();

    // block_expr: '{' stmt* expr_or_unit '}'
    std::optional<ast::field<ast::expr>> parse_block_expr_rule();

    // attr_name: name | group_expr
    std::optional<ast::field<ast::expr>> parse_attr_name_rule();

    // weak_expr: infix_call_expr | expr
    std::optional<ast::field<ast::expr>> parse_weak_expr_rule();

    // infix_call_expr: expr_4 name expr_4
    std::optional<ast::field<ast::expr>> parse_infix_call_expr_rule();

    // name: NAME
    std::optional<std::string> parse_name_rule();

    // xtime_flag: 'ctime' | 'rtime' | 
    std::optional<ast::field<ast::xtime_flag>> parse_xtime_flag_rule();

    // type_annotation: ':' expr
    std::optional<ast::field<ast::expr>> parse_type_annotation_rule();

    // _loop0_1: stmt
    std::optional<ast::sequence<ast::stmt>> parse__loop0_1_rule();

    // _tmp_2: '=' expr
    std::optional<ast::field<ast::expr>> parse__tmp_2_rule();

    // _loop0_3: stmt
    std::optional<ast::sequence<ast::stmt>> parse__loop0_3_rule();

    // _tmp_4: 'class' | 'struct'
    std::optional<std::monostate> parse__tmp_4_rule();

    // _loop0_6: '::' name
    std::optional<std::vector<std::string>> parse__loop0_6_rule();

    // _gather_5: name _loop0_6
    std::optional<std::vector<std::string>> parse__gather_5_rule();

    // _loop0_7: (',' arg_spec)
    std::optional<ast::sequence<ast::arg_spec>> parse__loop0_7_rule();

    // _loop0_9: ',' arg_spec
    std::optional<ast::sequence<ast::arg_spec>> parse__loop0_9_rule();

    // _gather_8: arg_spec _loop0_9
    std::optional<ast::sequence<ast::arg_spec>> parse__gather_8_rule();

    // _tmp_10: '=' expr
    std::optional<ast::field<ast::expr>> parse__tmp_10_rule();

    // _tmp_11: 'else' flow_block
    std::optional<ast::field<ast::expr>> parse__tmp_11_rule();

    // _tmp_12: 'else' flow_block
    std::optional<ast::field<ast::expr>> parse__tmp_12_rule();

    // _tmp_13: 'else' flow_block
    std::optional<ast::field<ast::expr>> parse__tmp_13_rule();

    // _loop1_14: ('and' expr_1)
    std::optional<ast::sequence<ast::expr>> parse__loop1_14_rule();

    // _loop1_15: ('or' expr_1)
    std::optional<ast::sequence<ast::expr>> parse__loop1_15_rule();

    // _loop1_16: comparison_followup_pair
    std::optional<std::vector<std::pair < ast::field < ast::cmp_op > , ast::field < ast::expr >>>> parse__loop1_16_rule();

    // _tmp_17: sum_expr | product_expr
    std::optional<ast::field<ast::expr>> parse__tmp_17_rule();

    // _tmp_18: product_expr | expr_4
    std::optional<ast::field<ast::expr>> parse__tmp_18_rule();

    // _tmp_19: bitor_expr | expr_4
    std::optional<ast::field<ast::expr>> parse__tmp_19_rule();

    // _tmp_20: bitand_expr | expr_4
    std::optional<ast::field<ast::expr>> parse__tmp_20_rule();

    // _tmp_21: bitxor_expr | expr_4
    std::optional<ast::field<ast::expr>> parse__tmp_21_rule();

    // _tmp_22: shift_expr | expr_4
    std::optional<ast::field<ast::expr>> parse__tmp_22_rule();

    // _tmp_23: unary_expr | expr_5
    std::optional<ast::field<ast::expr>> parse__tmp_23_rule();

    // _loop0_24: token_stream
    std::optional<ast::sequence<ast::expr>> parse__loop0_24_rule();

    // _loop0_25: token_stream
    std::optional<ast::sequence<ast::expr>> parse__loop0_25_rule();

    // _loop0_26: token_stream
    std::optional<ast::sequence<ast::expr>> parse__loop0_26_rule();

    // _loop1_27: (!any_paren any_token)
    std::optional<std::vector<lex::Token>> parse__loop1_27_rule();

    // _loop1_28: STRING
    std::optional<std::vector<lex::Token>> parse__loop1_28_rule();

    // _loop0_30: ',' expr
    std::optional<ast::sequence<ast::expr>> parse__loop0_30_rule();

    // _gather_29: expr _loop0_30
    std::optional<ast::sequence<ast::expr>> parse__gather_29_rule();

    // _loop0_32: ',' expr
    std::optional<ast::sequence<ast::expr>> parse__loop0_32_rule();

    // _gather_31: expr _loop0_32
    std::optional<ast::sequence<ast::expr>> parse__gather_31_rule();

    // _loop0_33: stmt
    std::optional<ast::sequence<ast::stmt>> parse__loop0_33_rule();

    // _tmp_34: ',' arg_spec
    std::optional<ast::field<ast::arg_spec>> parse__tmp_34_rule();

    // _tmp_35: 'and' expr_1
    std::optional<ast::field<ast::expr>> parse__tmp_35_rule();

    // _tmp_36: 'or' expr_1
    std::optional<ast::field<ast::expr>> parse__tmp_36_rule();

    // _tmp_37: !any_paren any_token
    std::optional<lex::Token> parse__tmp_37_rule();

    #pragma endregion Rule parsers

};
#pragma endregion Parser


}  // namespace bondrewd::parse
