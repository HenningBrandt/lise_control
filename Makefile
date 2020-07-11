CC = gcc
CFLAGS = -Wall
LIBS = -lwiringPi -lyaml -lcyaml
INCLUDEDIRS = -Ilibcyaml/include
LIBDIRS = -Llibcyaml/build/debug

LIBCYAMLDIR = libcyaml
SRCDIR = src
BUILDDIR = build/release
CONFIGDIR = ~/.config/lise
CONFIGSRC = config
BINDIR = /usr/local/bin
SCRIPTDIR = scripts
SERVICEDIR = /etc/systemd/system

$(BUILDDIR)/lise_control : $(SRCDIR)/lise_control.c
	mkdir -p $(BUILDDIR)
	$(MAKE) -C $(LIBCYAMLDIR)
	$(CC) $< $(CFLAGS) $(INCLUDEDIRS) $(LIBDIRS) $(LIBS) -o $@

.PHONY: deps
deps:
	sudo apt-get install wiringpi
	sudo apt-get install libyaml-dev

.PHONY: install
install:
	mkdir -p $(CONFIGDIR)
	cp $(CONFIGSRC)/wiring.yaml $(CONFIGDIR)/wiring.yaml
	sudo cp $(BUILDDIR)/lise_control $(BINDIR)/lise_control
	sudo cp $(SCRIPTDIR)/lise.service $(SERVICEDIR)/lise.service

