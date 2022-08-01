
GHC = ghc
CABAL_UPDATE = cabal update
CABAL_INSTALL = cabal install --lib

GHCFLAGS += -O2

DEPS = \
	hdaemonize \
	fdo-notify \
	unix
DUMMY = $(patsubst %,.%.dep,$(DEPS))


DIST_ICON = ~/.user-utils/tea/icon.png


all: tea

deps: $(DUMMY)

.%.dep:
	$(CABAL_UPDATE)
	$(CABAL_INSTALL) $*
	@echo $* | tee $@

tea: tea.hs
	$(GHC) $(GHCFLAGS) -o $@ $^


~/.user-utils/tea:
	mkdir -p ~/.user-utils/tea

$(DIST_ICON): icon.png ~/.user-utils/tea
	cp $< $@


clean:
	$(RM) *.o *.hi

distclean: clean
	$(RM) tea


.PHONY: all clean distclean deps

