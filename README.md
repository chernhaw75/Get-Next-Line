*This project has been created as part of the 42 curriculum by tchern-h.*

# get_next_line

## Description

`get_next_line` is a C function that returns one line at a time from a file descriptor. Each call returns the next line, including its trailing `\n` (if there is one), and returns `NULL` when there is nothing left to read or an error occurs.

```c
char *get_next_line(int fd);
```

The challenge is that `read()` returns fixed-size chunks (`BUFFER_SIZE` bytes), not lines. A chunk can contain half a line or several lines, so the function must remember leftover data between calls. This is done with a `static` variable.

## Algorithm explained

### The core idea

Think of the file as a long scroll that can only be viewed through a small window of `BUFFER_SIZE` characters. A line almost never ends exactly where the window ends, so three things are needed:

1. A **buffer**: the small, temporary window that `read()` fills.
2. A **storage**: a longer-lived string that collects everything that has been read but not yet returned.
3. A way to **cut one line** out of `storage` and keep the rest for the next call.

```
file ──read()──> buffer ──str_append──> storage ──extract_line──> returned line
                (temporary)            (kept between calls)            │
                                            ▲                          │
                                            └──── update_storage ──────┘
                                              (keeps only the leftover)
```

### Why `storage` is `static`

A normal local variable disappears when the function returns. `static char *storage` lives for the whole program, so the leftover text survives from one call to the next. It starts as `NULL` automatically.

### What happens in one call

1. **Check the input.** If `fd < 0` or `BUFFER_SIZE <= 0`, return `NULL`.
2. **Fill the storage** (`read_and_store` and `fill`). If `storage` has no `\n` yet, call `read()` into the buffer, append the chunk to `storage`, and repeat. Stop when the new chunk contains a `\n` or when `read()` returns `0` (end of file). If `read()` returns `-1` or an allocation fails, free everything and return `NULL`.
3. **Extract the line** (`extract_line`). Copy everything from the start of `storage` up to and including the first `\n`. If there is no `\n` (last line of the file), copy everything.
4. **Update the storage** (`update_storage`). Keep only what comes after the first `\n`. If nothing remains, free `storage` and set it to `NULL`.
5. **Return the line.**

### Why the loop stops when it does

- **A `\n` was found:** a complete line is available, so reading more would be wasted work. Anything extra that was read stays in `storage` for the next call.
- **`read()` returned `0`:** end of file. Whatever is left in `storage` is the last line (it may have no trailing `\n`).
- **`read()` returned `-1`:** an error. The function frees its memory and returns `NULL`.

### Why the buffer is `BUFFER_SIZE + 1` bytes

`read()` does not add a `'\0'` at the end of the data. The extra byte lets the code write `buffer[r] = '\0'` so the chunk can be treated as a normal C string (for example by `ft_strchr`).

### Full trace

File contents: `"hello\nworld"`, with `BUFFER_SIZE=4`.

| Call | Step | What happens | `storage` |
|------|------|--------------|-----------|
| 1 | read | `read()` gives `"hell"`, no `\n`, keep reading | `"hell"` |
| 1 | read | `read()` gives `"o\nwo"`, contains `\n`, stop | `"hello\nwo"` |
| 1 | extract | returns `"hello\n"` | `"hello\nwo"` |
| 1 | update | keep text after the `\n` | `"wo"` |
| 2 | read | `read()` gives `"rld"`, no `\n`, keep reading | `"world"` |
| 2 | read | `read()` returns `0` (EOF), stop | `"world"` |
| 2 | extract | returns `"world"` (no `\n`) | `"world"` |
| 2 | update | no `\n`, nothing left, free | `NULL` |
| 3 | read | `read()` returns `0`, nothing to return | `NULL` |
| 3 | result | returns `NULL` | `NULL` |

### The growable buffer (`t_str`)

Appending with "allocate a new string and copy everything" would copy the whole stored text on every `read()`. On a very long line with a small `BUFFER_SIZE`, the cost grows quadratically.

To avoid this, `read_and_store` wraps `storage` in a small struct:

```c
typedef struct s_str
{
	char	*data;   // the text (heap memory)
	size_t	len;     // characters currently used
	size_t	cap;     // bytes allocated
}	t_str;
```

The spare room (`cap - len`) means most appends only copy the new bytes. A block of memory cannot be stretched, so when the text no longer fits, `str_grow` does three things:

1. Allocates a **new, bigger block**.
2. **Copies** the old text into it.
3. **Frees** the old block and updates `data` and `cap`.

The new capacity is `max(cap * 2, need)`, or `256` when the buffer is still empty. Doubling means growth steps become rare, so the total copying work is linear in the line length.

Example: `BUFFER_SIZE=4`, `storage = "wo"` (`len=2`, `cap=3`) and the next chunk is `"rld"`.

- `str_append` asks `str_grow` for `2 + 3 + 1 = 6` bytes.
- `6 > 3`, so it must grow. `cap * 2 = 6`, which is enough, so the new capacity is `6`.
- `"wo"` is copied into the new block, `"rld"` is added, and `'\0'` is written at the end.

The struct is passed as a pointer (`t_str *s`). That way `str_grow` can change `data` and `cap` in the caller's own struct, instead of in a copy, and one parameter carries all three values.

### Other design choices

- The `\n` search in `fill` only looks at the newly read chunk, not at the whole `storage` again.
- `str_grow` only frees the old block after the new one was allocated successfully, so a failed `malloc` never loses data.
- The temporary `buffer` is freed in every case, including errors.

### Example with `"ab\ncd"` and `BUFFER_SIZE=4`

| Call | Read | `storage` after read | Returned | `storage` after update |
|------|------|----------------------|----------|------------------------|
| 1 | `"ab\nc"` | `"ab\nc"` | `"ab\n"` | `"c"` |
| 2 | `"d"`, then EOF | `"cd"` | `"cd"` | `NULL` |
| 3 | EOF | `NULL` | `NULL` | - |

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
| `str_grow(s, need)` *(static)* | Makes sure the `t_str` can hold `need` bytes. If it is already large enough it does nothing; otherwise it allocates `max(cap * 2, need)` bytes (starting from `256` when `cap` is `0`), copies the old data, frees the old block, and updates `cap`. On `malloc` failure it returns `0` and leaves the old data untouched. |
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
- AI usage: AI was used to help understanding of logic.
