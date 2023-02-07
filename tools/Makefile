PYTHON ?= python

ROOT := ./../

GENERATED_TOKENS_HPP := $(ROOT)/include/bondrewd/lex/tokens.generated.hpp 
GENERATED_TOKENS_CPP := $(ROOT)/src/lex/tokens.generated.cpp
GENERATED := $(GENERATED_TOKENS_HPP) $(GENERATED_TOKENS_CPP)

CONFIG_GRAMMAR := $(ROOT)/grammar/bondrewd.grammar
CONFIG_KEYWORDS := $(ROOT)/grammar/keywords
CONFIG_PUNCTS := $(ROOT)/grammar/puncts


all: $(GENERATED)

clean:
	rm -f $(GENERATED)

re: clean all

$(GENERATED_TOKENS_HPP): tokens
$(GENERATED_TOKENS_CPP): tokens

tokens: $(CONFIG_KEYWORDS) $(CONFIG_PUNCTS)
	$(PYTHON) ./tokens/generate_tokens.py --keywords $(CONFIG_KEYWORDS) --puncts $(CONFIG_PUNCTS)

grammar: $(CONFIG_GRAMMAR)
	# TODO: pegen++

.PHONY: all clean re tokens
