
COPY = cp


BIN = $(shell ls bin/)
DEST_BIN = $(patsubst %,~/bin/%,$(BIN))


DIR = \
	desym \
	tea \
	imgm

DEST_DIR = $(patsubst %,~/bin/%,$(DIR))


all: $(DEST_BIN) $(DEST_DIR)


~/bin:
	mkdir -p $@

~/bin/,%: bin/,% ~/bin
	$(COPY) $< $@

~/bin/%: % ~/bin
	$(MAKE) -C $* $*
	$(COPY) $*/$* $@


tea: tea/icon.png
	$(COPY) $^ ~/.user-utils/$@/


uninstall:
	$(RM) $(DEST_BIN)


clean:
	for f in $(DIR); do	\
		$(MAKE) -C $$f $@; \
		echo; \
	done

distclean:
	for f in $(DIR); do	\
		$(MAKE) -C $$f $@; \
		echo; \
	done


.PHONY: all clean distclean
