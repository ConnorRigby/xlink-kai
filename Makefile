ifeq ($(ERL_EI_INCLUDE_DIR),)
$(warn ERL_EI_INCLUDE_DIR not set. Invoke via mix)
endif

# Set Erlang-specific compile and linker flags
ERL_CFLAGS ?= -I$(ERL_EI_INCLUDE_DIR)
ERL_LDFLAGS ?= -L$(ERL_EI_LIBDIR)

LDFLAGS ?= -fPIC
CFLAGS ?= -Wall -DUNIX -lpthread

LIBKAI_CFLAGS ?= -I$(PWD)/src/libkaiui
LIBKAI_LDFLAGS ?= -L$(PWD)/src/libkaiui -lkaiui

all: libkaiui priv/libkai_port

priv/libkai_port: src/libkai_port/main.c
	$(CC) $(ERL_CFLAGS) $(CFLAGS) $(LDFLAGS) $(ERL_LDFLAGS) $< -o $@ $(LIBKAI_CFLAGS) $(LIBKAI_LDFLAGS)

priv/ui_test: src/libkaiui/ui.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LIBKAI_CFLAGS) $(LIBKAI_LDFLAGS)

libkaiui:
	cd src/libkaiui/; make

libkaiui-clean:
	cd src/libkaiui/; make clean

clean: libkaiui-clean
	$(RM) -d priv/libkai
