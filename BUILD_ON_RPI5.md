# Building DA7281 HAL on Raspberry Pi 5

This guide shows how to build the DA7281 HAL on Raspberry Pi 5 for development.

## Why Build on RPi5?

- Save resources on your main PC/laptop
- Linux environment is better for embedded cross-compilation
- Faster iteration during development
- Can leave builds running without tying up your main machine

## Prerequisites

### 1. Install ARM GCC Toolchain

```bash
sudo apt-get update
sudo apt-get install -y gcc-arm-none-eabi cmake build-essential git
```

### 2. Clone Qorvo SDK

```bash
cd ~
git clone https://github.com/HapticFeedbackWearable/DW3_QM33_SDK.git
```

### 3. Clone DA7281 HAL Project

```bash
cd ~
git clone https://github.com/ansarirahim/da7281-dwm3001c-hal.git
cd da7281-dwm3001c-hal
```

## Building

### Quick Build

```bash
# Create build directory
mkdir -p build && cd build

# Configure
cmake ..

# Build
make

# Show sizes
make size
```

### Custom SDK Path

If your SDK is in a different location:

```bash
cmake .. -DQORVO_SDK_PATH=/path/to/your/SDK_17_1_0
make
```

### Build Output

After successful build, you'll see:

```
[100%] Building C object CMakeFiles/da7281_hal.dir/src/da7281_i2c.c.o
[100%] Built target da7281_hal
```

Object files created:
- `CMakeFiles/da7281_hal.dir/src/da7281.o` (~7.3 KB)
- `CMakeFiles/da7281_hal.dir/src/da7281_i2c.o` (~3.8 KB)

### Check Sizes

```bash
make size
```

Output:
```
   text    data     bss     dec     hex filename
   1562      32       8    1602     642 da7281_i2c.o
   4180       0       0    4180    1054 da7281.o
```

## Development Workflow

### 1. Edit Code on RPi5

```bash
# SSH to RPi5
ssh ar@192.168.1.42

# Edit files
cd ~/da7281-dwm3001c-hal
nano src/da7281.c
```

### 2. Build

```bash
cd build
make
```

### 3. Check for Errors

The build uses `-Wall -Werror` so any warnings will fail the build.

### 4. Commit and Push

```bash
git add -A
git commit -m "Your changes"
git push origin main
```

## Accessing from Windows PC

### Option 1: SSH and Edit Remotely

Use VS Code Remote SSH extension:
1. Install "Remote - SSH" extension in VS Code
2. Connect to `ar@192.168.1.42`
3. Open folder `~/da7281-dwm3001c-hal`
4. Edit and build directly on RPi5

### Option 2: Sync Files

Use `rsync` to sync between PC and RPi5:

```bash
# From Windows (using WSL or Git Bash)
rsync -avz da7281-dwm3001c-hal/ ar@192.168.1.42:~/da7281-dwm3001c-hal/
```

### Option 3: Git Workflow

1. Edit on Windows PC
2. Commit and push to GitHub
3. Pull on RPi5 and build

```bash
# On RPi5
cd ~/da7281-dwm3001c-hal
git pull origin main
cd build
make
```

## Troubleshooting

### Build Fails with Missing Headers

Make sure SDK path is correct:
```bash
ls ~/DW3_QM33_SDK/SDK_BSP/Nordic/SDK_17_1_0
```

### Compiler Not Found

Install ARM GCC:
```bash
sudo apt-get install gcc-arm-none-eabi
arm-none-eabi-gcc --version
```

### Clean Build

```bash
cd build
rm -rf *
cmake ..
make
```

## Integration with Christopher's Project

After building successfully on RPi5, provide Christopher with:

1. **Object files** (from `build/CMakeFiles/da7281_hal.dir/src/`)
2. **Header files** (from `include/`)
3. **Integration instructions** (from main README.md)

He can link the object files directly into his project without needing to compile them.

## Performance

RPi5 build times:
- Clean build: ~2-3 seconds
- Incremental build: <1 second

Much faster than Windows cross-compilation!

