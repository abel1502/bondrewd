_PEGEN_PATH := $(CURRENT_PATH)

_GENERATED := \
	$(PROJECT_ROOT)/include/bondrewd/parse/parser.gen.hpp \
	$(PROJECT_ROOT)/src/parse/parser.gen.cpp

_DEPENDENCIES := \
	$(_PEGEN_PATH)/templates/parser.tpl.hpp \
	$(_PEGEN_PATH)/templates/parser.tpl.cpp \
	$(_PEGEN_PATH)/cxx_generator.py \
	$(_PEGEN_PATH)/pegenxx.py \
	$(TOOLS_ROOT)/jinja_codegen.py \
	$(CONFIG_GRAMMAR) \
	$(CONFIG_KEYWORDS) \
	$(CONFIG_PUNCTS)


GENERATED += $(_GENERATED)


$(_PEGEN_PATH)/custom_pegen_grammar_parser.py: $(_PEGEN_PATH)/custom_pegen_metagrammar.gram
	$(PYTHON) -m pegen -q $(_PEGEN_PATH)/custom_pegen_metagrammar.gram -o $(_PEGEN_PATH)/custom_pegen_grammar_parser.py


$(_GENERATED) : $(_DEPENDENCIES) $(_PEGEN_PATH)/custom_pegen_grammar_parser.py
	$(PYTHON) $(_PEGEN_PATH)/pegenxx.py --grammar $(CONFIG_GRAMMAR) --keywords $(CONFIG_KEYWORDS) --puncts $(CONFIG_PUNCTS) --output $(PROJECT_ROOT)
