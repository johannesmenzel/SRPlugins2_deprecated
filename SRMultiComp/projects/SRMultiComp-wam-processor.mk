include ./config/SRMultiComp-web.mk

TARGET = ./build-web/scripts/SRMultiComp-wam.js

SRC += $(WAM_SRC)
CFLAGS += $(WAM_CFLAGS)
CFLAGS += $(EXTRA_CFLAGS)
LDFLAGS += $(WAM_LDFLAGS) \
-s EXPORTED_FUNCTIONS=$(WAM_EXPORTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC)
