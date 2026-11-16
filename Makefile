# Thin wrappers so build/test/run work from the repository root.
# Host needs cmake installed (see README). Docker path needs Docker only.
#
# Host and Docker must use different build dirs: CMakeCache embeds absolute
# paths (/Users/... vs /workspace/...) and cannot be shared.

BUILD_DIR ?= build
DOCKER_BUILD_DIR ?= build-docker

.PHONY: all configure build test run clean docker-shell docker-test help

all: build

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run: build
	./$(BUILD_DIR)/dealerdesk

clean:
	rm -rf $(BUILD_DIR) $(DOCKER_BUILD_DIR)

docker-shell:
	docker compose run --rm dealerdesk bash

docker-test:
	docker compose run --rm dealerdesk bash -lc 'cmake -S . -B $(DOCKER_BUILD_DIR) && cmake --build $(DOCKER_BUILD_DIR) && ctest --test-dir $(DOCKER_BUILD_DIR) --output-on-failure'

help:
	@echo "Host (needs cmake on PATH):"
	@echo "  make          configure + build  -> $(BUILD_DIR)/"
	@echo "  make test     build + ctest"
	@echo "  make run      build + ./$(BUILD_DIR)/dealerdesk"
	@echo "  make clean    remove $(BUILD_DIR)/ and $(DOCKER_BUILD_DIR)/"
	@echo ""
	@echo "Docker (pinned toolchain, trusted path; uses $(DOCKER_BUILD_DIR)/):"
	@echo "  make docker-shell"
	@echo "  make docker-test"
	@echo ""
	@echo "Legacy one-liner still works: gcc -o dealerdesk *.c -lm"
