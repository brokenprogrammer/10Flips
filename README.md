# 10Flips

Online version of the greatest card game known to man

## Client

The client is made using Emscripten.

### Installing

```bash
# Get the emsdk repo
git clone https://github.com/emscripten-core/emsdk.git

# Enter that directory
cd emsdk

# Fetch the latest version of the emsdk (not needed the first time you clone)
git pull

# Download and install the latest SDK tools.
emsdk install latest

# Make the "latest" SDK "active" for the current user. (writes .emscripten file)
emsdk activate latest

# Activate PATH and other environment variables in the current terminal
emsdk_env.bat
```

### Building

```bash
build.bat
```

### Running

```bash
cd build

python -m http.server 8080
```
