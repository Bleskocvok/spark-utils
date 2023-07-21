
DEST_BIN ?= ~/bin


SHELL_UTILS = \
	,runc \
	,runcpp \
	,profile \
	,tester

INSTALL_SHELL_UTILS = $(patsubst %,$(DEST_BIN)/%,$(SHELL_UTILS))


BIG = \
	desym \
	getrand \
	imgm \
	wallpie

INSTALL_BIG = $(patsubst %,install_%,$(BIG))


all: $(BIG)


desym:
	$(MAKE) -C $@ all

getrand:
	$(MAKE) -C $@ all

imgm:
	$(MAKE) -C $@ all

wallpie:
	$(MAKE) -C $@ all


install_desym: desym
	install -m 0755 -T desym/desym $(DEST_BIN)/desym

install_getrand: getrand
	install -m 0755 -T getrand/getrand $(DEST_BIN)/getrand

install_imgm: imgm
	install -m 0755 -T imgm/imgm $(DEST_BIN)/imgm

install_wallpie: wallpie
	install -m 0755 -T wallpie/wallpie $(DEST_BIN)/wallpie




$(DEST_BIN):
	-mkdir -p $(DEST_BIN)


$(DEST_BIN)/%: $(DEST_BIN) shell/%
	install -m 0755 -T shell/$* $@



install: $(INSTALL_SHELL_UTILS) $(INSTALL_BIG)

uninstall:
	$(RM) -r $(DEST_BIN)


clean:
	for f in $(BIG); do	\
		$(MAKE) -C $$f $@; \
		echo; \
	done

distclean:
	for f in $(BIG); do	\
		$(MAKE) -C $$f $@; \
		echo; \
	done


.PHONY: all clean distclean \
	install uninstall \
	$(BIG) $(INSTALL_BIG) \


# COPY = cp


# BIN = $(shell ls bin/)
# DEST_BIN = $(patsubst %,~/bin/%,$(BIN))


# DIR = \
# 	desym \
# 	imgm

# DEST_DIR = $(patsubst %,~/bin/%,$(DIR))


# all: $(DEST_BIN) $(DEST_DIR)


# ~/bin:
# 	mkdir -p $@

# ~/bin/,%: bin/,% ~/bin
# 	$(COPY) $< $@

# ~/bin/%: % ~/bin
# 	$(MAKE) -C $* $*
# 	$(COPY) $*/$* $@

# uninstall:
# 	$(RM) $(DEST_BIN)


# clean:
# 	for f in $(DIR); do	\
# 		$(MAKE) -C $$f $@; \
# 		echo; \
# 	done

# distclean:
# 	for f in $(DIR); do	\
# 		$(MAKE) -C $$f $@; \
# 		echo; \
# 	done


# .PHONY: all clean distclean



