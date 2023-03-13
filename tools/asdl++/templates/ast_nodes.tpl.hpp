{{ _autogenerated_ }}
#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ast/ast.hpp>


namespace bondrewd::ast::nodes {


{%- macro gen_class(name, asdl_type, master_name="") -%}
{%- if asdl_type is instanceof asdl.Product or asdl_type is instanceof asdl.Constructor %}
{#- Product = concrete struct #}
{#- Constructor = Product without attributes #}
class {{ name }}{% if asdl_type is instanceof asdl.Constructor %} : _ConcreteASTNode{% endif %} {
public:
    #pragma region Fields
    {%- filter indent(width=4) %}
    {{- gen_fields(helpers.fields_and_attrs(asdl_type)) }}
    {%- endfilter %}
    #pragma endregion Fields

    #pragma region Constructors
    {%- if helpers.fields_and_attrs(asdl_type) %}
    {{ name }}({% for field in helpers.fields_and_attrs(asdl_type) %}{{ helpers.field_type(field) }} {{ field.name }}{%- if not loop.last %}, {% endif %}{% endfor %}) :
        {% for field in helpers.fields_and_attrs(asdl_type) %}{{ field.name }}{std::move({{ field.name }})}{%- if not loop.last %}, {% endif %}{% endfor %} {}
    {%- else %}
    {{ name }}() {};
    {%- endif %}
    #pragma endregion Constructors
    {% filter indent(width=4) %}
    {#- helpers.copyable(asdl_type) -#}
    {{- gen_service_ctors(name, copy=False) }}
    {%- endfilter %}

    #pragma region Uniform fields access
    /**
     * Returns a tuple of references to all fields.
     * Note that the tuple may include sequences (std::vector) and optional fields (null unique_ptr's).
     */
    auto get_fields_tuple() {
        return std::tie(
            {%- for field in asdl_type.fields %}
            {{- field.name }}
            {%- if not loop.last %}, {% endif %}
            {%- endfor -%}
        );
    }
    #pragma endregion Uniform fields access

    #pragma region Casts
    {%- if master_name %}
    // TODO: Explicit?
    operator ast::field<{{ master_name }}>() const {
        return ast::make_field<{{ master_name }}>(*this);
    }
    {%- endif %}
    #pragma endregion Casts

    #pragma region Extras
    {% filter indent(width=4) %}
    {{- asdl_type.extras }}
    {%- endfilter %}
    #pragma endregion Extras
};
{%- elif asdl_type is instanceof asdl.Sum %}
{#- Sum = abstract class #}
{%- for alt in asdl_type.types %}
{{- gen_class(alt.name, alt, master_name=name) }}

{% endfor %}
class {{ name }} : public _AST<{{ helpers.names_of_alts(asdl_type) | join(", ") }}> {
public:
    #pragma region Constructors
    {%- filter indent(width=4) %}
    {{- gen_ctor(name, asdl_type.attributes, value_arg_types=helpers.names_of_alts(asdl_type)) }}
    {%- endfilter %}
    #pragma endregion Constructors
    {% filter indent(width=4) %}
    {#- helpers.copyable(asdl_type) -#}
    {{- gen_service_ctors(name, copy=False) }}
    {%- endfilter %}

    #pragma region Destructor
    ~{{ name }}() = default;
    #pragma endregion Destructor

    #pragma region Attributes
    {% filter indent(width=4) %}
    {{- gen_fields(asdl_type.attributes) }}
    {%- endfilter %}
    #pragma endregion Attributes

    #pragma region Extras
    {% filter indent(width=4) %}
    {{- asdl_type.extras }}
    {%- endfilter %}
    #pragma endregion Extras
};
{%- endif %}
{%- endmacro %}


{%- macro gen_ctor(name, fields, value_arg_types=None) %}
{%- if value_arg_types is none %}
{%- set value_arg = "" %}
{%- set value_set = "" %}
{%- else %}
{%- set value_arg = "auto &&value_" %}
{%- set value_set = "std::forward<decltype(value_)>(value_)" %}
{%- endif %}
{%- set has_args = (fields | length) > 0 %}
{{ name }}({{ value_arg }}{% for arg in helpers.field_decls(fields) %}, {{ arg }}{% endfor %})
    : _AST({{ value_set }}){% for field in fields %}, std::move({{ field.name }}){% endfor %} {}
{%- endmacro %}


{%- macro gen_fields(fields) %}
{%- for field in helpers.field_decls(fields) %}
{{ field }};
{%- endfor %}
{%- endmacro %}


{%- macro gen_service_ctors(name, copy=True, move=True) %}
#pragma region Service constructors
{{ name }}(const {{ name }} &) = {{ "default" if copy else "delete" }};
{{ name }}({{ name }} &&) = {{ "default" if move else "delete" }};
{{ name }} &operator=(const {{ name }} &) = {{ "default" if copy else "delete" }};
{{ name }} &operator=({{ name }} &&) = {{ "default" if move else "delete" }};
#pragma endregion Service constructors
{%- endmacro %}


#pragma region Forward declarations
{%- for asdl_type in asdl_module.dfns %}
class {{ asdl_type.name }};
{%- if asdl_type.value is instanceof asdl.Sum %}
{%- for alt in asdl_type.value.types %}
class {{ alt.name }};
{%- endfor %}
{%- endif %}
{%- endfor %}
#pragma endregion Forward declarations


#pragma region Implementations
{%- for asdl_type in asdl_module.dfns %}
{{- gen_class(asdl_type.name, asdl_type.value) }}
{{- "\n\n" if not loop.last else "" }}
{%- endfor %}
#pragma endregion Implementations


}  // namespace bondrewd::ast::nodes


namespace bondrewd::ast {


// Import all nodes into the bondrewd::ast namespace,
// so that they can be used without the nodes:: prefix.
using namespace nodes;


}  // namespace bondrewd::ast
