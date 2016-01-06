# Quelldateien
SRCS             = imageLoader.c vector.c main.c scene.c io.c water.c terrain.c logic.c stringOutput.c 

# ausfuehrbares Ziel
TARGET           = water

# Ist libz verfügbar?
HAVE_LIBZ = yes


# Basis-Makefile einbinden
-include ./Makefile.common


# Precompiler flags
# Pfad der Schnittstelle des ImageLoaders hinzufuegen
CPPFLAGS_COMMON  += -I$(IMGLOADER_DIR)/include

# Linker flags
# Pfad der ImageLoader-Bibliothek hinzufuegen
LDFLAGS_COMMON  += -L$(IMGLOADER_DIR)/lib


# Wenn libz verfuegbar ist, muessen wir sie mit einlinken, um die
# Bibliothek auch zum Laden von PNG-Bildern verwenden zu koennen.
ifeq ($(HAVE_LIBZ),yes)
LDLIBS += -lz
endif


.PHONY: distcleanimageloader


# TARGETS

# Regel zur Erstellung der ImageLoader-Bibliothek
$(IMGLOADER_DIR)/lib/$(IMGLOADER_LIB):
	(cd $(IMGLOADER_DIR) && ./configure)
	$(MAKE) -C $(IMGLOADER_DIR)

# Vollstaendiges Aufraeumen beinhaltet auch Aufraeumen des ImageLoaders
distclean: distcleanimageloader

# Regel zum Aufraeumen des ImageLoaders
distcleanimageloader:
	if [ -f $(IMGLOADER_DIR)/Makefile ]; then $(MAKE) -C $(IMGLOADER_DIR) HAVE_LIBZ=$(HAVE_LIBZ) distclean; fi
