#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ROW_SIZE 16
#define BUF_SIZE 4096
#define MAX_SEARCH 255

void show_help(const char *name)
{
    printf("ByteForge - a simple file inspector\n\n");
    printf("Usage:\n");
    printf("  %s <file>\n", name);
    printf("  %s <file> --offset N --length N\n", name);
    printf("  %s <file> --search TEXT\n", name);
    printf("  %s --help\n\n", name);
    printf("Options:\n");
    printf("  --offset N   Start showing bytes at offset N.\n");
    printf("  --length N   Show at most N bytes.\n");
    printf("  --search T   Find text in the file (up to 255 bytes).\n");
    printf("  --help       Show this help.\n");
}

int read_number(const char *text, long *number)
{
    char *end;
    long value;

    if (text == NULL || text[0] == '\0' || text[0] == '-') {
        return 0;
    }

    errno = 0;
    value = strtol(text, &end, 10);
    if (errno != 0 || *end != '\0' || end == text) {
        return 0;
    }

    *number = value;
    return 1;
}

int get_file_size(FILE *fp, long *size)
{
    if (fseek(fp, 0, SEEK_END) != 0) {
        return 0;
    }

    *size = ftell(fp);
    if (*size < 0) {
        return 0;
    }

    return fseek(fp, 0, SEEK_SET) == 0;
}

void print_info(const char *filename, long size)
{
    printf("\n========================================\n");
    printf("               BYTEFORGE\n");
    printf("          Low-Level File Inspector\n");
    printf("========================================\n\n");
    printf("File: %s\n", filename);
    printf("Size: %ld bytes\n", size);
}

int show_hex(FILE *fp, long offset, long length)
{
    unsigned char *buf;
    long shown = 0;

    buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        fprintf(stderr, "Could not allocate the read buffer.\n");
        return 0;
    }

    if (fseek(fp, offset, SEEK_SET) != 0) {
        fprintf(stderr, "Could not move to offset %ld.\n", offset);
        free(buf);
        return 0;
    }

    printf("\nOffset     Hex bytes                                        ASCII\n");
    printf("--------------------------------------------------------------------------\n");

    while (shown < length) {
        long left = length - shown;
        size_t want = left > BUF_SIZE ? BUF_SIZE : (size_t)left;
        size_t got = fread(buf, 1, want, fp);
        size_t row;

        if (got == 0) {
            if (ferror(fp)) {
                fprintf(stderr, "Error while reading the file.\n");
                free(buf);
                return 0;
            }
            break;
        }

        for (row = 0; row < got; row += ROW_SIZE) {
            size_t count = got - row;
            size_t i;

            if (count > ROW_SIZE) {
                count = ROW_SIZE;
            }

            printf("%08lX   ", (unsigned long)(offset + shown + (long)row));

            for (i = 0; i < ROW_SIZE; i++) {
                if (i < count) {
                    printf("%02X ", (unsigned int)buf[row + i]);
                } else {
                    printf("   ");
                }
            }

            printf(" | ");
            for (i = 0; i < count; i++) {
                unsigned char ch = buf[row + i];
                if (ch >= 32 && ch <= 126) {
                    putchar(ch);
                } else {
                    putchar('.');
                }
            }
            putchar('\n');
        }

        shown += (long)got;
    }

    free(buf);
    printf("\nDisplayed %ld byte(s).\n", shown);
    return 1;
}

int search_file(FILE *fp, const char *text)
{
    unsigned char buf[BUF_SIZE + MAX_SEARCH];
    size_t search_len = strlen(text);
    size_t keep = 0;
    long bytes_read = 0;
    unsigned long found = 0;

    if (search_len == 0 || search_len > MAX_SEARCH) {
        fprintf(stderr, "Search text must contain between 1 and %d bytes.\n", MAX_SEARCH);
        return 0;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Could not rewind the file before searching.\n");
        return 0;
    }

    printf("\nSearching for: \"%s\"\n", text);

    while (1) {
        size_t got = fread(buf + keep, 1, BUF_SIZE, fp);
        size_t total = keep + got;
        size_t i;
        long start_offset = bytes_read - (long)keep;

        for (i = 0; i + search_len <= total; i++) {
            if (memcmp(buf + i, text, search_len) == 0) {
                long match_at = start_offset + (long)i;
                printf("Match at offset 0x%08lX (%ld)\n",
                       (unsigned long)match_at, match_at);
                found++;
            }
        }

        bytes_read += (long)got;

        if (got < BUF_SIZE) {
            if (ferror(fp)) {
                fprintf(stderr, "Error while reading the file.\n");
                return 0;
            }
            break;
        }

        keep = search_len - 1;
        if (keep > total) {
            keep = total;
        }
        if (keep > 0) {
            memmove(buf, buf + total - keep, keep);
        }
    }

    if (found == 0) {
        printf("No matches found.\n");
    } else {
        printf("Found %lu match(es).\n", found);
    }

    return 1;
}

int main(int argc, char *argv[])
{
    const char *filename;
    const char *search = NULL;
    long offset = 0;
    long length = -1;
    long size;
    int have_offset = 0;
    int have_length = 0;
    int i;
    int ok;
    FILE *fp;

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 ||
                      strcmp(argv[1], "-h") == 0)) {
        show_help(argv[0]);
        return 0;
    }

    if (argc < 2) {
        show_help(argv[0]);
        return 1;
    }

    filename = argv[1];

    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--offset") == 0) {
            if (have_offset || i + 1 >= argc || !read_number(argv[i + 1], &offset)) {
                fprintf(stderr, "--offset needs one non-negative number.\n");
                return 1;
            }
            have_offset = 1;
            i++;
        } else if (strcmp(argv[i], "--length") == 0) {
            if (have_length || i + 1 >= argc || !read_number(argv[i + 1], &length)) {
                fprintf(stderr, "--length needs one non-negative number.\n");
                return 1;
            }
            have_length = 1;
            i++;
        } else if (strcmp(argv[i], "--search") == 0) {
            if (search != NULL || i + 1 >= argc || argv[i + 1][0] == '\0') {
                fprintf(stderr, "--search needs one non-empty text argument.\n");
                return 1;
            }
            search = argv[i + 1];
            i++;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    if (search != NULL && (have_offset || have_length)) {
        fprintf(stderr, "Use --search on its own, without --offset or --length.\n");
        return 1;
    }

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open '%s': %s\n", filename, strerror(errno));
        return 1;
    }

    if (!get_file_size(fp, &size)) {
        fprintf(stderr, "Could not get the file size.\n");
        fclose(fp);
        return 1;
    }

    print_info(filename, size);

    if (search != NULL) {
        ok = search_file(fp, search);
    } else {
        if (offset > size) {
            fprintf(stderr, "Offset %ld is past the end of the file.\n", offset);
            fclose(fp);
            return 1;
        }

        if (length < 0 || length > size - offset) {
            length = size - offset;
        }

        printf("Range: %ld to %ld (end not included)\n", offset, offset + length);
        ok = show_hex(fp, offset, length);
    }

    if (fclose(fp) != 0) {
        fprintf(stderr, "Could not close the file cleanly.\n");
        ok = 0;
    }

    return ok ? 0 : 1;
}
