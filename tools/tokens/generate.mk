_TOKENS_PATH := $(CURRENT_PATH)

_GENERATED := \
	$(PROJECT_ROOT)/include/bondrewd/lex/tokens.generated.hpp \
	$(PROJECT_ROOT)/src/lex/tokens.generated.cpp

_DEPENDENCIES := \
	$(_TOKENS_PATH)/templates/tokens.tpl.hpp \
	$(_TOKENS_PATH)/templates/tokens.tpl.cpp \
	$(_TOKENS_PATH)/generate_tokens.py \
	$(_TOKENS_PATH)/trie_gen.py \
	$(TOOLS_ROOT)/templated_codegen/__init__.py \
	$(CONFIG_KEYWORDS) \
	$(CONFIG_PUNCTS)


GENERATED += $(_GENERATED)


$(_GENERATED): $(_DEPENDENCIES)
	$(PYTHON) $(_TOKENS_PATH)/generate_tokens.py --keywords $(CONFIG_KEYWORDS) --puncts $(CONFIG_PUNCTS) --output $(PROJECT_ROOT)
