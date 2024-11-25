# Define Project and PlatformIO Variables
PLATFORMIO_CLI = platformio

# Default target
all: build

build:
	$(PLATFORMIO_CLI) run

upload:
	$(PLATFORMIO_CLI) run --target upload

monitor:
	$(PLATFORMIO_CLI) device monitor

clean:
	$(PLATFORMIO_CLI) run --target clean

compiledb:
	$(PLATFORMIO_CLI) run -t compiledb

default: all
