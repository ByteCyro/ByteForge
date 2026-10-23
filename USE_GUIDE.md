# ByteForge — Use Guide

ByteForge is a read-only command-line tool for inspecting the bytes stored in a file. It can print a hexadecimal view, show a selected range of bytes, and search a file for text.

## 1. Build ByteForge

You need a C compiler such as GCC or Clang.

### Windows (GCC / MinGW)

Open PowerShell or the VS Code terminal in the folder that contains `byteforge.c`, then run:

```powershell
gcc -std=c99 -O2 -Wall -Wextra -Wpedantic byteforge.c -o byteforge.exe
```

### Linux or macOS

Run this in the folder that contains `byteforge.c`:

```sh
cc -std=c99 -O2 -Wall -Wextra -Wpedantic byteforge.c -o byteforge
```

If the command finishes without an error, the executable should be created in the current folder.

## 2. Show the available options

Windows:

```powershell
.\byteforge.exe --help
```

Linux or macOS:

```sh
./byteforge --help
```

The general format is:

```text
byteforge <file> [options]
```

Put the filename first. If a path contains spaces, surround it with quotes.

## 3. Inspect a file

The project includes a sample file at `examples/sample.txt`.

Windows:

```powershell
.\byteforge.exe examples/sample.txt
```

Linux or macOS:

```sh
./byteforge examples/sample.txt
```

ByteForge prints the file name and size, then displays the contents in rows of up to 16 bytes. Each row includes:

- **Offset:** where that row starts in the file, shown in hexadecimal.
- **Hex bytes:** each byte written as two hexadecimal digits.
- **ASCII:** printable ASCII characters are shown as text; other byte values are shown as dots.

For example, the text `Hello` is stored as these five bytes:

```text
48 65 6C 6C 6F  | Hello |
```

ByteForge only inspects files. It does not edit or overwrite them.

## 4. Inspect part of a file

Use `--offset` to choose the starting byte and `--length` to choose how many bytes to display.

```powershell
.\byteforge.exe examples/sample.txt --offset 8 --length 32
```

On Linux or macOS:

```sh
./byteforge examples/sample.txt --offset 8 --length 32
```

This starts at byte offset `8` and displays up to `32` bytes. Offsets start at **zero**, so offset `0` is the first byte in the file. The length is a byte count, not a line or character count. If the requested length extends past the end of the file, ByteForge displays the bytes that remain. An offset beyond the end of the file is reported as an error.

You can use `--offset` without `--length` to display from that offset to the end of the file. You can also use `--length` by itself to display that many bytes from the start.

## 5. Search for text

Use `--search` followed by the text you want to find:

```powershell
.\byteforge.exe examples/sample.txt --search ByteForge
```

Linux or macOS:

```sh
./byteforge examples/sample.txt --search ByteForge
```

ByteForge prints the offset of each match and a total match count. Search is **case-sensitive**, so `ByteForge` and `byteforge` are different searches. Search text can be up to 255 bytes. Put quotes around a phrase that contains spaces, for example:

```powershell
.\byteforge.exe examples/sample.txt --search "small C file"
```

Search mode is separate from range viewing, so do not combine `--search` with `--offset` or `--length`.

## 6. A quick practice run

Try these commands in order:

```powershell
.\byteforge.exe --help
.\byteforge.exe examples/sample.txt
.\byteforge.exe examples/sample.txt --offset 8 --length 32
.\byteforge.exe examples/sample.txt --search ByteForge
.\byteforge.exe examples/sample.txt --search notfound
```

On Linux or macOS, replace `.\byteforge.exe` with `./byteforge` in the commands above.

The final search should print `No matches found.` if that text is not present in the sample file. This is a normal result, not a program error.

## 7. Common problems

- **Could not open the file:** check the spelling and path. Relative paths are resolved from the current terminal folder.
- **`gcc` or `cc` is not recognized:** install a C compiler or configure it in your system's `PATH`.
- **Unknown option:** check the spelling of the option and its arguments.
- **Invalid offset or length:** enter a non-negative whole number.
- **No matches found:** the search text was not found with the same capitalization.

## Limitations

- ByteForge is a viewer and search tool; it does not modify files.
- Search looks for literal text and is case-sensitive; it is not a regular-expression search.
- Search text is limited to 255 bytes.
- Very large files may be limited by the file-position range supported by the platform's C runtime.

For more information about the project and the C concepts it practises, see [README.md](README.md).
