PYTHON ?= python

# TODO: absolute paths?
PROJECT_ROOT := ./../
TOOLS_ROOT := ./../tools

CONFIG_GRAMMAR := $(PROJECT_ROOT)/grammar/bondrewd.grammar
CONFIG_ASDL := $(PROJECT_ROOT)/grammar/bondrewd.grammar
CONFIG_KEYWORDS := $(PROJECT_ROOT)/grammar/keywords
CONFIG_PUNCTS := $(PROJECT_ROOT)/grammar/puncts

CURRENT_PATH = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Extended by generate.mk from each tool
GENERATED := 


all: _all


include $(TOOLS_ROOT)/*/generate.mk


_all: $(GENERATED)

clean:
	rm -f $(GENERATED)

re: clean all

.PHONY: all _all clean re
