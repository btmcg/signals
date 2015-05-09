#!/usr/bin/env make

BUILD_ROOT ?= $(CURDIR)

# Error if BUILD_ROOT not defined.
ifeq ($(BUILD_ROOT),)
    $(error "BUILD_ROOT not defined")
endif

include mk/env.mk


# Define all projects first
SUBDIRS = signalfd \
          sigtimedwait


# Sets of directories to do various things in
BUILD_DIRS     := $(SUBDIRS:%=build-%)
CLEAN_DIRS     := $(SUBDIRS:%=clean-%)
DISTCLEAN_DIRS := $(SUBDIRS:%=distclean-%)
INSTALL_DIRS   := $(SUBDIRS:%=install-%)
TEST_DIRS      := $(SUBDIRS:%=test-%)


all: $(BUILD_DIRS)
$(SUBDIRS): $(BUILD_DIRS)
$(BUILD_DIRS):
	$(MAKE) -C src/$(@:build-%=%)

clean: $(CLEAN_DIRS)
$(CLEAN_DIRS):
	$(MAKE) -C src/$(@:clean-%=%) $(MAKECMDGOALS)

distclean: $(DISTCLEAN_DIRS)
$(DISTCLEAN_DIRS):
	$(MAKE) -C src/$(@:distclean-%=%) $(MAKECMDGOALS)
	$(RM) doc

doc:
	doxygen $(DOXYFILE)

install: $(INSTALL_DIRS) all
$(INSTALL_DIRS):
	$(MAKE) -C src/$(@:install-%=%) $(MAKECMDGOALS)

test: $(TEST_DIRS) all
$(TEST_DIRS):
	$(MAKE) -C src/$(@:test-%=%) $(MAKECMDGOALS)


.PHONY: $(BUILD_DIRS)
.PHONY: $(CLEAN_DIRS)
.PHONY: $(DISTCLEAN_DIRS)
.PHONY: $(INSTALL_DIRS)
.PHONY: $(SUBDIRS)
.PHONY: $(TEST_DIRS)
.PHONY: all clean distclean doc install test
