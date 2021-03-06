KERNEL := $(shell sh -c 'uname -s 2>/dev/null || echo not')
OS := $(shell sh -c 'uname -o 2>/dev/null || echo not')
DISTRO = $(shell . /etc/os-release && echo "$$NAME $$VERSION_ID")
ARCH = $(shell uname -m 2>/dev/null)
NPROC = $(shell sh mk/nproc.sh)
TPUT = $(shell sh -c 'command -v tput')
TPUT-V = $(if $(TPUT), $(shell $(TPUT) -V 2>/dev/null))
CC-VERSION = $(shell $(CC) --version 2>/dev/null | head -n1)
STREQ = $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))
PRINTVAR = printf '\033[1m%13s\033[0m = %s$(2)\n' '$(1)' '$(strip $($(1)))' $(3)
PRINTVARX = $(call PRINTVAR,$(1), \033[32m(%s)\033[0m, '$(origin $(1))')
USERVARS = CC CFLAGS LDFLAGS LDLIBS DEBUG

AUTOVARS = \
    VERSION KERNEL \
    $(if $(call STREQ,$(KERNEL),Linux), DISTRO) \
    ARCH NPROC TERM TPUT TPUT-V MAKE_VERSION SHELL CC-VERSION

vars:
	@echo
	@$(foreach VAR, $(AUTOVARS), $(call PRINTVAR,$(VAR));)
	@$(foreach VAR, $(USERVARS), $(call PRINTVARX,$(VAR));)
	@echo


.PHONY: vars

ifneq "$(findstring s,$(firstword -$(MAKEFLAGS)))$(filter -s,$(MAKEFLAGS))" ""
  # Make "-s" flag was used (silent build)
  Q = @
  E = @:
else ifeq "$(V)" "1"
  # "V=1" variable was set (verbose build)
  Q =
  E = @:
else
  # Normal build
  Q = @
  E = @printf ' %7s  %s\n'
endif
