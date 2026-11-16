# DealerDesk — Docker build and test

Trusted quality path: compile and run CTest inside Docker. The image pins a
Debian userland with `build-essential` (gcc) and `cmake`/`ctest`, matching the
challenge one-liner `gcc -o dealerdesk *.c -lm` plus our CMake entrypoint.

Multi-OS GitHub Release binaries (when present) are a CI/CD demo only. They are
not a second quality gate.

## Enter the container

From the repository root:

```bash
docker compose run --rm dealerdesk bash
```

The repo is mounted at `/workspace`. Always run build, test, and the app from
that directory so `data/` resolves.

## Configure, build, test, run

Inside the container:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/dealerdesk
```

Legacy host one-liner (still valid outside Docker):

```bash
gcc -o dealerdesk *.c -lm
./dealerdesk
```

## After mutating sample data

```bash
git checkout -- data
```
