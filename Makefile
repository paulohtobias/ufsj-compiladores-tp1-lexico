# Call Makefile-build to build for the specified target
all:
	$(MAKE) -f Makefile-build $(RULE)

run: RULE = run
run: all

%:
	$(MAKE) -f Makefile-build TARGET=$* $*

.PHONY: all run
