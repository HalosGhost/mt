PRJNM = mt
LIBNM = lib$(PRJNM)

SO_VER = 0.1

RTDIR = $(shell git rev-parse --show-toplevel)

SRCDIR = src
TSTDIR = tst
INCDIR = inc
DOCDIR = doc
BLDDIR = bld
OBJDIR = $(BLDDIR)/obj
DEPDIR = dep

CONFIGURED_DEPS = monocypher

CPPFLAGS = -I$(INCDIR)
CFLAGS = -Og -ggdb3 -Wall -Wextra -Wpedantic -std=gnu18
LINKFLAGS = -lm

ifneq ($(CONFIGURED_DEPS),)
CPPFLAGS += $(shell pkg-config --cflags-only-I $(CONFIGURED_DEPS))
CFLAGS += $(shell pkg-config --cflags-only-other $(CONFIGURED_DEPS))
LINKFLAGS += $(shell pkg-config --libs $(CONFIGURED_DEPS))
endif

DATE = $(shell date +'%Y-%b-%d')

MKDIR = @mkdir -p --
RM = rm -rf --
LN = ln -sf --
DOCC = scdoc

MAINSRC = $(shell grep -rl main $(SRCDIR))
MAINOBJ = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(patsubst %.c,%.o,$(MAINSRC)))
MAINDEP = $(patsubst $(SRCDIR)%,$(DEPDIR)%,$(patsubst %.c,%.d,$(MAINSRC)))
MAINS = $(patsubst $(SRCDIR)%,$(BLDDIR)%,$(patsubst %.c,%,$(MAINSRC)))

SOURCES = $(filter-out $(MAINSRC),$(wildcard $(SRCDIR)/*))
OBJECTS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(patsubst %.c,%.o,$(SOURCES)))
DEPENDS = $(patsubst $(SRCDIR)%,$(DEPDIR)%,$(patsubst %.c,%.d,$(SOURCES)))

TESTSRC = $(wildcard $(TSTDIR)/*.c)
TESTS   = $(patsubst $(TSTDIR)%,$(BLDDIR)/$(TSTDIR)%,$(patsubst %.c,%,$(TESTSRC)))

DOCSSRC = $(wildcard $(DOCDIR)/*.scd)
DOCS    = $(patsubst $(DOCDIR)%,$(BLDDIR)/$(DOCDIR)%,$(patsubst %.scd,%,$(DOCSSRC)))

.PHONY: all clean check docs $(LIBNM) mains

all: $(LIBNM) mains docs check

check: $(TESTS)
	(for i in $^; do \
		printf '%s: [ PEND ]' $$i; \
		if ./$$i; then \
			printf '\r%s: [ PASS ]\n' $$i; \
		else \
			printf '\r%s: [ FAIL ]\n' $$i; \
		fi; \
	done)

clean:
	$(RM) $(BLDDIR) $(DEPDIR) tags

-include $(DEPENDS)

$(BLDDIR)/$(TSTDIR)/%: $(TSTDIR)/%.c $(BLDDIR)/$(LIBNM).a
	$(MKDIR) $(@D) $(DEPDIR)
	$(CC) $(CPPFLAGS) -fPIC $(CFLAGS) $^ -MMD -MP -MF $(DEPDIR)/$(@F).d -o $@

docs: $(DOCS)

$(BLDDIR)/$(DOCDIR)/%: $(DOCDIR)/%.scd
	$(MKDIR) $(@D)
	$(DOCC) < $< > $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(MKDIR) $(@D) $(DEPDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -MMD -MP -MF $(DEPDIR)/$(@F:.o=.d) -o $@

$(LIBNM): $(BLDDIR)/$(LIBNM).so $(BLDDIR)/$(LIBNM).a

$(BLDDIR)/$(LIBNM).so.$(SO_VER): $(OBJECTS)
	$(MKDIR) $(@D)
	$(CC) $(CPPFLAGS) -fPIE $(CFLAGS) -shared -o $@ $^ $(LINKFLAGS) -Wl,-soname,$(@F)

$(BLDDIR)/$(LIBNM).so: $(BLDDIR)/$(LIBNM).so.$(SO_VER)
	$(MKDIR) $(@D)
	$(LN) $(shell basename $<) $@

$(BLDDIR)/$(LIBNM).a: $(OBJECTS)
	$(MKDIR) $(@D)
	$(AR) rcs $@ $^

mains: $(MAINS)

$(BLDDIR)/%: $(OBJECTS) $(OBJDIR)/%.o
	$(MKDIR) $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(LINKFLAGS) -L$(BLDDIR) $(BLDDIR)/$(LIBNM).a

tags:
	find . -type f -iregex '.*\.[ch]\(xx\|pp\)?$$' | xargs ctags -a -f $@

$(V).SILENT:
