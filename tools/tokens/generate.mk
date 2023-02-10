GENERATED_TOKENS_HPP := $(PROJECT_ROOT)/include/bondrewd/lex/tokens.generated.hpp 
GENERATED_TOKENS_CPP := $(PROJECT_ROOT)/src/lex/tokens.generated.cpp
GENERATED += $(GENERATED_TOKENS_HPP) $(GENERATED_TOKENS_CPP)


_DEPENDENCIES := \
	$(CURRENT_PATH)/templates/tokens.tpl.hpp \
	$(CURRENT_PATH)/templates/tokens.tpl.cpp \
	$(CURRENT_PATH)/generate_tokens.py \
	$(CURRENT_PATH)/trie_gen.py \
	$(TOOLS_ROOT)/templated_codegen/__init__.py \
	$(CONFIG_KEYWORDS) \
	$(CONFIG_PUNCTS)


$(GENERATED_TOKENS_HPP) $(GENERATED_TOKENS_CPP): $(_DEPENDENCIES)
	$(PYTHON) $(CURRENT_PATH)/generate_tokens.py --keywords $(CONFIG_KEYWORDS) --puncts $(CONFIG_PUNCTS) --output $(PROJECT_ROOT)
