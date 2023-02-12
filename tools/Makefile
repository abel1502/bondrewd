PYTHON ?= python

# TODO: absolute paths?
PROJECT_ROOT := $(abspath ../)
TOOLS_ROOT := $(PROJECT_ROOT)/tools
CURRENT_PATH = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

CONFIG_GRAMMAR := $(PROJECT_ROOT)/grammar/bondrewd.gram
CONFIG_ASDL := $(PROJECT_ROOT)/grammar/bondrewd.asdl
CONFIG_KEYWORDS := $(PROJECT_ROOT)/grammar/keywords
CONFIG_PUNCTS := $(PROJECT_ROOT)/grammar/puncts


# Extended by generate.mk from each tool
GENERATED := 


all: _all


include $(TOOLS_ROOT)/*/generate.mk


_all: $(GENERATED)

clean:
	rm -f $(GENERATED)

re: clean all

.PHONY: all _all clean re
