all: build

build: tools
	cd build && \
	cmake .. && \
	make

tools:
	@$(MAKE) -C tools

docs:
	@$(MAKE) -C docs html

.PHONY: all build tools docs
