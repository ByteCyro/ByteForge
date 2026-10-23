# ByteForge

ByteForge is a small C command-line tool for looking inside files. It shows file bytes in hexadecimal, displays printable characters beside them, and can search a file for text.

I made it as a way to practise C file handling, pointers, buffers, and dynamic memory allocation. It uses only the C standard library.

## What it does

- Shows the size of a file.
- Prints bytes in 16-byte rows with their offset and an ASCII column.
- Lets you inspect a range with `--offset` and `--length`.
- Searches for text and prints the offsets where it appears.
- Reads files in chunks instead of loading the whole file into memory.
- Opens files read-only; it does not change them.

## Build (Checkout the USE_GUIDE.md)

With GCC or Clang:

```sh
cc -std=c99 -Wall -Wextra -Wpedantic byteforge.c -o byteforge
```

On Windows with MinGW, you can use:

```powershell
gcc -std=c99 -Wall -Wextra -Wpedantic byteforge.c -o byteforge.exe
```

## Examples

Show a file:

```sh
./byteforge examples/sample.txt
```

Start at byte 8 and show up to 32 bytes:

```sh
./byteforge examples/sample.txt --offset 8 --length 32
```

Search for some text:

```sh
./byteforge examples/sample.txt --search ByteForge
```

Show help:

```sh
./byteforge --help
```

On Windows, use `byteforge.exe` instead of `./byteforge`.

Search is case-sensitive and supports search text up to 255 bytes. The file-position functions used here have a platform-dependent size limit, so very large files may not be supported on every system.

## What I practised

- Opening and closing files with `fopen` and `fclose`
- Reading binary data with `fread`
- Moving around a file with `fseek` and getting its size with `ftell`
- Allocating and freeing a read buffer
- Displaying numbers in hexadecimal
- Comparing byte sequences with `memcmp`
- Handling basic command-line options and errors

## License

MIT. See [LICENSE](LICENSE).
