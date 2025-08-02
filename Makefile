TARGET = ds_engine
SRC    = commands.c  file_utils.c  main.c  path_utils.c
OBJ    = $(SRC:.c=.o)
CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O3 -o ds_engine

INSTALL_DIR = ${HOME}/.dsv2
SCRIPT = ds_script
ALIAS_LINE = alias ds='source ~/.dsv2/$(SCRIPT)'

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
	
# Install binary and script
install: $(TARGET)
	@mkdir -p $(INSTALL_DIR)
	cp $(TARGET) $(INSTALL_DIR)/
	cp $(SCRIPT) $(INSTALL_DIR)/

	@echo "Checking for alias in ~/.bashrc..."
	@if ! grep -Fxq "$(ALIAS_LINE)" ~/.bashrc; then \
		echo "Adding alias to ~/.bashrc"; \
		echo "$(ALIAS_LINE)" >> ~/.bashrc; \
	else \
		echo "Alias already exists in ~/.bashrc."; \
	fi

.PHONY: all clean install
