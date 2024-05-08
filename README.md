# Clone repository with submodules

```bash
git clone --recurse-submodules git@github.com:FelisssTattoo/FelisssTattooCRM.git
```

If you forget to use `--recurse-submodules` while cloning, use next command in project root
```bash
git submodule update --init --recursive
```

# Prerequisites/Dependencies

```bash
sudo apt install libssl-dev libboost-dev
```

# Build
```bash
cmake -B build
cd build
make
```

## build with static analyzer
```bash
cmake -B build -DRUN_CLANG_TIDY=ON
cd build
make
```

## build with shared libraries(not static)
```bash
cmake -B build -DBUILD_SHARED_LIBS=ON
cd build
make
```

# Scripts/Tools

## clang-format
From root of project, run
```bash
./scripts/run_clang_format.sh
```
to format all project sources with project's code style

---
From root of project, run
```bash
./scripts/install_hooks.sh
```
to force git to apply project's code style on pre-commit

## clang-tidy
From root of project, run
```bash
./scripts/run_clang_tidy.sh
```
to run static analyze
