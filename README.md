# PWDatConverter

Converting 3D point clouds in TXT format to a very fast binary format (PW).

The prebuilt `PWDatConverter.exe` was compiled using GCC with MinGW for Windows 64 Bit.  
If you do not have a compiler installed, you can follow the official [Visual Studio Code MinGW setup guide](https://code.visualstudio.com/docs/cpp/config-mingw#_prerequisites).

PWDatConverter does not require any additional libraries and can be easily built using the provided `Makefile`.

> **Note:** The generated PW file is compatible with [GIRAFFE](https://github.com/mel-ias/GIRAFFE), a high-performance point cloud renderer and analysis tool.

---

## How to build

1. **Requirements:**
   - [g++](https://gcc.gnu.org/) (or another C++17-compatible compiler)
   - [make](https://www.gnu.org/software/make/)

2. **Build:**

   Open a terminal in the project directory and run (on Windows!):

   ```
   mingw32-make
   ```

   This will produce the executable `PWDatConverter`.

3. **Clean build files:**

   ```
   mingw32-make clean
   ```

---

## Features

- **TXT → PW:** Converts point clouds from TXT (columns: X Y Z R G B, separated by space, tab, comma, or semicolon) to a compact binary format.
- **PW → TXT:** Converts binary PW files back to TXT with full precision.
- **Lossless:** No loss of coordinate or color precision.
- **Cross-platform:** Compiles with any modern C++17 compiler.
- **Compatible with [GIRAFFE](https://github.com/mel-ias/GIRAFFE)** for visualization and analysis.

---

## Usage

### Convert TXT to PW

```
PWDatConverter.exe txt2pw input.txt output.pw
```

- `input.txt`: Your point cloud in TXT format (columns: X Y Z R G B, separated by space, tab, comma, or semicolon).
- `output.pw`: The binary output file.

### Convert PW to TXT

```
PWDatConverter.exe pw2txt input.pw output.txt
```

- `input.pw`: Your binary point cloud file.
- `output.txt`: The TXT output file (tab-separated, columns: X Y Z R G B).

---

## Example

**TXT input (tab, space, comma, or semicolon separated):**
```
-0.698778	-89.861092	0.000000	204	204	204
0.743234,-89.278084,0.000000,204,204,204
0.754819; -90.797966; 0.000000; 204; 204; 204
```

**Convert to PW:**
```
PWDatConverter.exe txt2pw cloud.txt cloud.pw
```

**Convert back to TXT:**
```
PWDatConverter.exe pw2txt cloud.pw cloud_converted.txt
```

---

## Notes

- The program always expects 6 columns: X Y Z R G B.
- The PW format always stores coordinates as double and colors as unsigned char.
- TXT output uses adaptive precision for compactness and accuracy.
- The PW format is designed for fast loading and is directly compatible with [GIRAFFE](https://github.com/mel-ias/GIRAFFE).

---

## License

MIT License (or specify your own)