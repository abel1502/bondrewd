_ASDL_PATH := $(CURRENT_PATH)

_GENERATED := \
	$(PROJECT_ROOT)/include/bondrewd/ast/ast_nodes.gen.hpp

_DEPENDENCIES := \
	$(_ASDL_PATH)/templates/ast_nodes.tpl.hpp \
	$(_ASDL_PATH)/asdl.py \
	$(_ASDL_PATH)/asdl_cpp.py \
	$(TOOLS_ROOT)/jinja_codegen.py \
	$(CONFIG_ASDL)


GENERATED += $(_GENERATED)


$(_GENERATED) : $(_DEPENDENCIES)
	$(PYTHON) $(_ASDL_PATH)/asdl_cpp.py --asdl $(CONFIG_ASDL) --output $(PROJECT_ROOT)
