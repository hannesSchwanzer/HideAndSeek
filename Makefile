# Define Project and PlatformIO Variables
PLATFORMIO_CLI = platformio

# Default target
all: build

build:
	$(PLATFORMIO_CLI) run -e debug

buildRelease:
	$(PLATFORMIO_CLI) run -e release

upload:
	$(PLATFORMIO_CLI) run -e debug --target upload

uploadRelease:
	$(PLATFORMIO_CLI) run -e release --target upload

monitor:
	$(PLATFORMIO_CLI) device monitor

clean:
	$(PLATFORMIO_CLI) run --target clean

compiledb:
	$(PLATFORMIO_CLI) run -t compiledb

uploadAndMonitor: upload monitor

default: all
