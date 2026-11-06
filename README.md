*This project has been created as part of the 42 curriculum by tchern-h.*

# get_next_line

## Description

`get_next_line` is a C function that returns one line at a time from a file descriptor. Each call returns the next line, including its trailing `\n` (if there is one), and returns `NULL` when there is nothing left to read or an error occurs.

```c
char *get_next_line(int fd);
```

The challenge is that `read()` returns fixed-size chunks (`BUFFER_SIZE` bytes), not lines. A chunk can contain half a line or several lines, so the function must remember leftover data between calls. This is done with a `static` variable.

### How it works

A `static char *storage` keeps everything that has been read but not yet returned. Each call does three steps:

1. **`read_and_store`** reads chunks into a temporary `buffer` and appends them to `storage` until `storage` contains a `\n` or the end of the file is reached.
2. **`extract_line`** copies everything from the start of `storage` up to and including the first `\n`. This is the returned line.
3. **`update_storage`** removes that line from `storage`, keeping only what comes after the `\n` for the next call.

Example with the file `"ab\ncd"` and `BUFFER_SIZE=4`:

| Call | Read | `storage` after read | Returned | `storage` after update |
|------|------|----------------------|----------|------------------------|
| 1 | `"ab\nc"` | `"ab\nc"` | `"ab\n"` | `"c"` |
| 2 | `"d"`, then EOF | `"cd"` | `"cd"` | `NULL` |
| 3 | EOF | `NULL` | `NULL` | - |

### Performance

Appending to `storage` with a plain "allocate a new string and copy everything" would copy the whole stored text on every `read()`, which becomes quadratic on very long lines with a small `BUFFER_SIZE`. Instead, `read_and_store` uses a small `t_str` struct (`data`, `len`, `cap`) whose capacity **doubles** when full, so the total copying is linear in the line length. The `\n` search only looks at the newly read chunk, not at the whole `storage` again.

## Project structure

| File | Role |
|------|------|
| `get_next_line.h` | Includes, default `BUFFER_SIZE` (42), the `t_str` struct, and all prototypes |
| `get_next_line.c` | `get_next_line` and the three steps it is built from, plus the `fill` helper |
| `get_next_line_utils.c` | String helpers and the growable-buffer append |

## Function reference

### `get_next_line.c`

| Function | Explanation |
|----------|-------------|
| `get_next_line(fd)` | Public function. Returns `NULL` if `fd < 0` or `BUFFER_SIZE <= 0`. Otherwise it calls `read_and_store`, `extract_line` and `update_storage` in order, and returns the line. If anything fails, it frees `storage`, resets it to `NULL` and returns `NULL`. |
| `read_and_store(fd, storage)` | Allocates the temporary `buffer` (`BUFFER_SIZE + 1`), wraps the existing `storage` in a `t_str`, and calls `fill`. Frees `buffer` in every case. On a read or allocation error it frees `storage` and returns `NULL`; otherwise it returns the updated `storage`. |
| `fill(fd, s, buffer)` *(static)* | The read loop. First checks if `storage` already holds a `\n`. If not, it calls `read()` repeatedly, null-terminates each chunk, appends it with `str_append`, and stops as soon as the new chunk contains a `\n` or `read()` returns 0 (EOF). Returns the last `read()` result, or `-1` on a read or allocation error. |
| `extract_line(storage)` | Counts characters up to the first `\n` (included if present, otherwise up to the end of the string) and returns that part as a new string with `ft_substr`. |
| `update_storage(storage)` | Finds the first `\n`. If there is none, or nothing follows it, frees `storage` and returns `NULL`. Otherwise it returns a new string with only the text after the `\n` and frees the old `storage`. |

### `get_next_line_utils.c`

| Function | Explanation |
|----------|-------------|
| `ft_strlen(str)` | Length of a string. Returns `0` for a `NULL` pointer so callers can pass an empty `storage` safely. |
| `ft_strchr(s, c)` | Returns a pointer to the first occurrence of `c` in `s`, or `NULL`. Also returns `NULL` for a `NULL` string and finds the terminating `'\0'` when `c` is `'\0'`. |
| `ft_substr(s, start, len)` | Returns a new string of at most `len` characters starting at index `start`. Returns an empty string if `start` is past the end, and `NULL` if `s` is `NULL` or `malloc` fails. |
| `str_grow(s, need)` *(static)* | Makes sure the `t_str` can hold `need` bytes. If it is already large enough it does nothing; otherwise it allocates `max(cap * 2, need)` bytes, copies the old data, frees the old block, and updates `cap`. On `malloc` failure it returns `0` and leaves the old data untouched. |
| `str_append(s, buf, n)` | Appends `n` bytes of `buf` to the `t_str`, growing it with `str_grow` when needed, keeps `len` up to date and keeps the data null-terminated. Returns `1` on success and `0` on allocation failure. |

### `t_str` (defined in `get_next_line.h`)

| Field | Meaning |
|-------|---------|
| `data` | Pointer to the heap-allocated, null-terminated text |
| `len` | Number of characters currently stored (without the `'\0'`) |
| `cap` | Number of bytes allocated for `data` |

It only exists as a local variable inside `read_and_store`; the only `static` variable is still `storage`.

### Memory handling

- The temporary `buffer` is allocated and freed within every call to `read_and_store`.
- If `read()` fails (returns `-1`) or an allocation fails, `storage` is freed and set to `NULL`.
- When the last line is returned, `storage` is freed by `update_storage`.

## Instructions

### Compilation

Compile the three source files together with your own `main.c`. `BUFFER_SIZE` can be set at compile time; if it is not set, the header defaults it to 42.

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl
```

### Usage

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "get_next_line.h"

int main(void)
{
	int		fd;
	char	*line;

	fd = open("file.txt", O_RDONLY);
	if (fd < 0)
		return (1);
	while ((line = get_next_line(fd)))
	{
		printf("%s", line);
		free(line);
	}
	close(fd);
	return (0);
}
```

## Notes

- Only the mandatory part is implemented. A single `static` pointer is shared by all file descriptors, so reading from several fds in an interleaved way is not supported.
- If the caller stops reading before the end of the file, the remaining `storage` stays allocated, which is expected with a `static` variable.

## Resources

- `man 2 read`, `man 3 malloc`, `man 3 free`
- Static variables and storage duration in C
- AI usage: *(edit this section to describe how you used AI, for example to understand the algorithm, and which parts you wrote and tested yourself.)*
