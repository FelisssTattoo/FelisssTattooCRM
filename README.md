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
sudo apt install libboost-all-dev
```

# Build
```bash
cmake -B build
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
