include mk/env.mk
include mk/functions.mk

# initialization
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# load modules (any subdirectory that contains a "module.mk" file)
$(call load-modules)


# rules and dependencies
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# generate all necessary rules
$(eval $(call build-rules,$(call get-all-modules)))


# recipes
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# necessary targets and phony targets
.PHONY: all clean distclean list-modules $(call get-all-modules)

all: $(call get-all-modules)

clean:
	$(if $(wildcard $(call get-all-targets) $(call get-all-objs) $(call get-all-deps)),\
		$(RM) $(strip $(call get-all-targets)) $(call get-all-objs) $(call get-all-deps))

distclean: clean
	$(if $(wildcard $(LIB_DIR)),\
		$(RM) $(LIB_DIR)/* && $(RMDIR) $(LIB_DIR))
	$(if $(wildcard $(BIN_DIR)),\
		$(RM) $(BIN_DIR)/* && $(RMDIR) $(BIN_DIR))

list-modules:
	$(list-modules)

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

$(LIB_DIR):
	$(MKDIR) $(LIB_DIR)
