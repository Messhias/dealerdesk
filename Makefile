# Thin wrappers so build/test/run work from the repository root.
# Host needs cmake installed (see README). Docker path needs Docker only.
#
# Host and Docker must use different build dirs: CMakeCache embeds absolute
# paths (/Users/... vs /workspace/...) and cannot be shared.

BUILD_DIR ?= build
DOCKER_BUILD_DIR ?= build-docker

.PHONY: all configure build test run bdd clean docker-shell docker-test docker-run docker-bdd help bdd-host-check

all: build

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run: build
	./$(BUILD_DIR)/dealerdesk

# BDD terminal suite (C). Not part of the unit CTest gate.
# Built by CMake target bdd_runner into the build dir (same as dealerdesk).
# Host uses $(BUILD_DIR)/; Docker uses $(DOCKER_BUILD_DIR)/ so ELF vs Mach-O
# never share one binary path.
BDD_BIN := $(BUILD_DIR)/bdd_runner
BDD_BIN_DOCKER := $(DOCKER_BUILD_DIR)/bdd_runner
# Legacy paths from the pre-CMake cc recipe (removed on clean).
BDD_BIN_LEGACY := BDD/runner/bdd_runner
BDD_BIN_LEGACY_LINUX := BDD/runner/bdd_runner.linux

# Host prerequisites: warn clearly, never install packages from Make.
bdd-host-check:
	@command -v $(CC) >/dev/null 2>&1 || { \
	  echo "make bdd: C compiler '$(CC)' not found on PATH."; \
	  echo "Install a C toolchain (Xcode CLT / build-essential), or use: make docker-bdd"; \
	  exit 1; \
	}
	@echo '\#include <zlib.h>' | $(CC) -E -x c - >/dev/null 2>&1 || { \
	  echo "make bdd: missing zlib headers (<zlib.h>)."; \
	  echo "Host hint: macOS often needs zlib via Xcode CLT; Linux needs zlib1g-dev (or equivalent)."; \
	  echo "This Makefile will not install packages. Or use: make docker-bdd"; \
	  exit 1; \
	}
	@echo '\#include <pty.h>' | $(CC) -E -x c - >/dev/null 2>&1 \
	  || echo '\#include <util.h>' | $(CC) -E -x c - >/dev/null 2>&1 || { \
	  echo "make bdd: missing PTY headers (<pty.h> or <util.h>)."; \
	  echo "Host hint: install libc / Xcode CLT development headers."; \
	  echo "This Makefile will not install packages. Or use: make docker-bdd"; \
	  exit 1; \
	}

bdd: build bdd-host-check
	@test -x $(BDD_BIN) || { \
	  echo "make bdd: missing $(BDD_BIN) (CMake target bdd_runner)."; \
	  echo "Re-run: cmake -S . -B $(BUILD_DIR) && cmake --build $(BUILD_DIR)"; \
	  exit 1; \
	}
	DEALERDESK_BIN=./$(BUILD_DIR)/dealerdesk ./$(BDD_BIN)

clean:
	rm -rf $(BUILD_DIR) $(DOCKER_BUILD_DIR) $(BDD_BIN_LEGACY) $(BDD_BIN_LEGACY_LINUX)

# Override image ENTRYPOINT so shell/test do not go through the pre-run gate.
docker-shell:
	docker compose run --rm --entrypoint bash dealerdesk

docker-test:
	docker compose run --rm --entrypoint bash dealerdesk -lc 'cmake -S . -B $(DOCKER_BUILD_DIR) && cmake --build $(DOCKER_BUILD_DIR) && ctest --test-dir $(DOCKER_BUILD_DIR) --output-on-failure'

# Default image entrypoint: build if needed → ctest → exec dealerdesk (fail closed).
docker-run:
	./scripts/run-with-tests.sh

# Docker build dir keeps the Linux runner off the host Mach-O path.
docker-bdd:
	docker compose build dealerdesk
	docker compose run --rm --entrypoint bash dealerdesk -lc \
	  'cmake -S . -B $(DOCKER_BUILD_DIR) && cmake --build $(DOCKER_BUILD_DIR) && \
	   DEALERDESK_BDD_DOCKER=1 DEALERDESK_BIN=./$(DOCKER_BUILD_DIR)/dealerdesk \
	   ./$(BDD_BIN_DOCKER)'

help:
	@echo "Host (needs cmake on PATH):"
	@echo "  make          configure + build  -> $(BUILD_DIR)/"
	@echo "  make test     build + ctest"
	@echo "  make run      build + ./$(BUILD_DIR)/dealerdesk"
	@echo "  make bdd      build + BDD terminal suite (CMake target bdd_runner)"
	@echo "  make clean    remove $(BUILD_DIR)/, $(DOCKER_BUILD_DIR)/, legacy BDD bins"
	@echo ""
	@echo "Docker (pinned toolchain, trusted path; uses $(DOCKER_BUILD_DIR)/):"
	@echo "  make docker-shell   interactive bash (bypasses pre-run gate)"
	@echo "  make docker-test    cmake + ctest only (bypasses pre-run gate)"
	@echo "  make docker-run     pre-run gate then dealerdesk"
	@echo "  make docker-bdd     same BDD suite inside the image (build-docker/bdd_runner)"
	@echo "  ./scripts/run-with-tests.sh   same as make docker-run"
	@echo ""
	@echo "Legacy one-liner still works: gcc -o dealerdesk *.c -lm"
