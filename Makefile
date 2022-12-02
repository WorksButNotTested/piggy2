FRIDA_VERSION?=16.0.6
FRIDA_PLATFORM?=linux
FRIDA_ARCH?=x86_64
FRIDA_DEVKIT_FILENAME:=frida-core-devkit-$(FRIDA_VERSION)-$(FRIDA_PLATFORM)-$(FRIDA_ARCH).tar.xz
FRIDA_DEVKIT_URL:=https://github.com/frida/frida/releases/download/$(FRIDA_VERSION)/$(FRIDA_DEVKIT_FILENAME)
BUILD_DIR:=build/
FRIDA_DEVKIT_DOWNLOAD:=$(BUILD_DIR)$(FRIDA_DEVKIT_FILENAME)
FRIDA_DEVKIT_DIR:=build/devkit/
FRIDA_DEVKIT_INCLUDE:=$(FRIDA_DEVKIT_DIR)frida-core.h
FRIDA_DEVKIT_LIB:=$(FRIDA_DEVKIT_DIR)libfrida-core.a

INC_DIR:=inc/
SRC_DIR:=src/
OBJ_DIR:=$(BUILD_DIR)obj/
INCLUDES:=$(wildcard $(INC_DIR)*.h)
SOURCES:=$(wildcard $(SRC_DIR)*.c)
OBJS:=$(foreach src,$(SOURCES),$(OBJ_DIR)$(notdir $(patsubst %.c, %.o, $(src))))
PIGGY2_DEBUG:=$(BUILD_DIR)_piggy2
PIGGY2:=$(BUILD_DIR)piggy2
STRIP?=strip
CFLAGS?=\
	-g \
	-Wall \
	-pedantic \
	-Werror \
	-fPIC \
	-ffunction-sections \
	-fdata-sections
LDFLAGS?=\
	-fPIC \
	-Os \
	-Wl,--gc-sections


.PHONY: all clean run format js-src

all: $(PIGGY2)

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(FRIDA_DEVKIT_DIR): | $(BUILD_DIR)
	mkdir -p $@

$(FRIDA_DEVKIT_DOWNLOAD): | $(FRIDA_DEVKIT_DIR)
	wget -O $(FRIDA_DEVKIT_DOWNLOAD) $(FRIDA_DEVKIT_URL)

$(FRIDA_DEVKIT_INCLUDE): | $(FRIDA_DEVKIT_DOWNLOAD)
	tar Jxvf $(FRIDA_DEVKIT_DOWNLOAD) -C $(FRIDA_DEVKIT_DIR)

$(OBJ_DIR):
	mkdir -p $@

define BUILD_SOURCE
$(2): $(1) $(INCLUDES) | $(FRIDA_DEVKIT_INCLUDE) $(OBJ_DIR)
	$(CC) \
		$(CFLAGS) \
		-I $(INC_DIR) \
		-I $(FRIDA_DEVKIT_DIR) \
		-c $1 \
		-o $2
endef

$(foreach src,$(SOURCES),$(eval $(call BUILD_SOURCE,$(src),$(OBJ_DIR)$(notdir $(patsubst %.c, %.o, $(src))))))

$(PIGGY2_DEBUG): $(OBJS) | $(FRIDA_DEVKIT_LIB)
	$(CC) \
		$(LDFLAGS) \
		$(OBJS) \
		$(FRIDA_DEVKIT_LIB) \
		-lpthread \
		-ldl \
		-o $@

$(PIGGY2): $(PIGGY2_DEBUG)
	cp $< $@
	$(STRIP) -s $@

$(CHSELL): $(PIGGY2)
	cp $< $@

run: $(PIGGY2)
	$< /bin/ls piggy.js

format:
	echo $(INCLUDES) $(SOURCES) | xargs -L1 clang-format -i
