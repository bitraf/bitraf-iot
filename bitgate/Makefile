TOOLCHAIN_Linux_x86_64 = https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
IDF_PATH = $(PWD)/esp-idf
PATH := $(PWD)/toolchain/xtensa-esp32-elf/bin:$(PWD)/venv/bin:$(PATH)

all: .toolchain.cookie .python.cookie
	cd bitgate; IDF_PATH=$(IDF_PATH) PATH=$(PATH) make

%: .toolchain.cookie .python.cookie
	cd bitgate; IDF_PATH=$(IDF_PATH) PATH=$(PATH) make $(patsubst make-,,$@)

####################################################
# Toolchain

toolchain:
	mkdir $@

toolchain/toolchain.tar.gz: | toolchain
	curl -o $@ $(TOOLCHAIN_Linux_x86_64)

toolchain/xtensa-esp32-elf: toolchain/toolchain.tar.gz
	tar xf $< -C toolchain
	touch $@

.toolchain.cookie: toolchain/xtensa-esp32-elf
	touch $@

####################################################
# Python

venv:
	virtualenv -p python venv

.python.cookie: venv
	venv/bin/pip install -r $(IDF_PATH)/requirements.txt
	touch $@
