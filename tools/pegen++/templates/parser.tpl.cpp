{{ _autogenerated_ }}
#include <bondrewd/parse/parser.gen.hpp>


#if {{ 1 if generator.debug else 0 }}
#define PARSER_DBG_(...)  fprintf(stderr, "[Parser] " __VA_ARGS__)
#else
#define PARSER_DBG_(...)
#endif


namespace bondrewd::parse {


#pragma region Rule parsers
{{ generator.gen_rule_parsers(with_impls=True) }}
#pragma endregion Rule parsers


}  // namespace bondrewd::parse
