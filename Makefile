
DEST_BIN ?= ~/bin


SHELL_UTILS = \
	,runc \
	,runcpp \
	,profile \
	,tester

INSTALL_SHELL_UTILS = $(patsubst %,$(DEST_BIN)/%,$(SHELL_UTILS))


BIG = \
	desym \
	erat \
	getrand \
	gibpoopass \
	graphinator-lite \
	imgm \
	sudoku \
	tabcat \
	wallpie

INSTALL_BIG = $(patsubst %,install_%,$(BIG))


all: $(BIG)


desym:
	$(MAKE) -C $@ all

erat:
	$(MAKE) -C $@ all

getrand:
	$(MAKE) -C $@ all

gibpoopass:
	$(MAKE) -C $@ all

graphinator-lite:
	$(MAKE) -C $@ all

imgm:
	$(MAKE) -C $@ all

sudoku:
	$(MAKE) -C $@ all

tabcat:
	$(MAKE) -C $@ all

wallpie:
	$(MAKE) -C $@ all


install_desym: desym
	install -m 0755 -T desym/desym $(DEST_BIN)/desym

install_erat: erat
	install -m 0755 -T erat/gibprime $(DEST_BIN)/erat

install_graphinator-lite: graphinator-lite
	install -m 0755 -T graphinator-lite/gibgraph $(DEST_BIN)/gibgraph
	install -m 0755 -T graphinator-lite/monitor $(DEST_BIN)/monitor

install_getrand: getrand
	install -m 0755 -T getrand/getrand $(DEST_BIN)/getrand

install_gibpoopass: gibpoopass
	install -m 0755 -T gibpoopass/pass $(DEST_BIN)/gibpoopass

install_imgm: imgm
	install -m 0755 -T imgm/imgm $(DEST_BIN)/imgm

install_sudoku: sudoku
	install -m 0755 -T sudoku/sudoku $(DEST_BIN)/sudoku

install_tabcat: tabcat
	install -m 0755 -T tabcat/tabcat $(DEST_BIN)/tabcat

install_wallpie: wallpie
	install -m 0755 -T wallpie/wallpie $(DEST_BIN)/wallpie




$(DEST_BIN):
	-mkdir -p $(DEST_BIN)


$(DEST_BIN)/%: $(DEST_BIN) shell/%
	install -m 0755 -T shell/$* $@



install: $(INSTALL_SHELL_UTILS) $(INSTALL_BIG)
	@echo ""
	@echo "INSTALLED TO $(DEST_BIN)"
	@echo "Remember to add that directory to PATH"

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
