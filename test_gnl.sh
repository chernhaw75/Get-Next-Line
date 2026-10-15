#!/usr/bin/env bash
# ----------------------------------------------------------------------------
# test_gnl.sh - local Moulinette-style tester for get_next_line (mandatory part)
#
# Usage:   ./test_gnl.sh [path/to/project]        (default: current directory)
# Options: BUFFER_SIZES="1 7 42" ./test_gnl.sh     (override sizes to test)
#          CC=clang ./test_gnl.sh                  (override compiler)
#
# Checks: norminette (if installed), compile with -Wall -Wextra -Werror,
#         output vs. original file for many files x many BUFFER_SIZEs,
#         memory errors + leaks (valgrind > AddressSanitizer > macOS `leaks`),
#         edge cases (bad fds, directory, stdin, BUFFER_SIZE=0).
# Nothing in your project folder is modified: all work happens in a temp dir.
# ----------------------------------------------------------------------------

PROJECT="${1:-.}"
CC="${CC:-cc}"
BUFFER_SIZES="${BUFFER_SIZES:-1 2 3 5 10 42 1000 9999 10000000}"
FLAGS="-Wall -Wextra -Werror"

if [ -t 1 ]; then
	R=$'\033[31m'; G=$'\033[32m'; Y=$'\033[33m'; B=$'\033[1m'; N=$'\033[0m'
else
	R=""; G=""; Y=""; B=""; N=""
fi

PASS=0; FAIL=0
ok()   { PASS=$((PASS + 1)); printf "  %s[OK]%s   %s\n" "$G" "$N" "$1"; }
ko()   { FAIL=$((FAIL + 1)); printf "  %s[KO]%s   %s\n" "$R" "$N" "$1"; [ -n "$2" ] && printf "%s\n" "$2" | head -8 | sed 's/^/         /'; }
info() { printf "  %s[INFO]%s %s\n" "$Y" "$N" "$1"; }
title() { printf "\n%s== %s ==%s\n" "$B" "$1" "$N"; }

PROJECT="$(cd "$PROJECT" 2>/dev/null && pwd)" || { echo "Folder not found: $1"; exit 2; }
# use every .c in the folder (your project splits GNL over several files)
SRC=$(ls "$PROJECT"/*.c 2>/dev/null | grep -v '_bonus')
[ -f "$PROJECT/get_next_line.h" ] || { echo "No get_next_line.h in $PROJECT"; exit 2; }
[ -n "$SRC" ] || { echo "No .c files in $PROJECT"; exit 2; }

TMP="$(mktemp -d 2>/dev/null || mktemp -d -t gnl)"
trap 'rm -rf "$TMP"' EXIT
TIMEOUT=""
command -v timeout >/dev/null 2>&1 && TIMEOUT="timeout 10"
command -v gtimeout >/dev/null 2>&1 && TIMEOUT="gtimeout 10"

# Pick a memory checker
MEMMODE="none"
if command -v valgrind >/dev/null 2>&1 && [ "$(uname)" = "Linux" ]; then
	MEMMODE="valgrind"
elif [ "$(uname)" = "Linux" ]; then
	MEMMODE="asan"
elif command -v leaks >/dev/null 2>&1; then
	MEMMODE="leaks"
fi

# ----------------------------------------------------------------- harnesses
cat > "$TMP/main.c" <<'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "get_next_line.h"
int main(int ac, char **av)
{
	int		fd = (ac > 1) ? open(av[1], O_RDONLY) : 0;
	char	*l;

	while ((l = get_next_line(fd)))
	{
		fputs(l, stdout);
		free(l);
	}
	return (0);
}
EOF

cat > "$TMP/edge.c" <<'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "get_next_line.h"
static void	chk(const char *name, char *l)
{
	printf("%s %s\n", l ? "BAD" : "GOOD", name);
	free(l);
}
int main(int ac, char **av)
{
	int fd;

	(void)ac;
	chk("fd=-1 returns NULL", get_next_line(-1));
	chk("fd=1000 (never opened) returns NULL", get_next_line(1000));
	fd = open(av[1], O_RDONLY);
	close(fd);
	chk("closed fd returns NULL", get_next_line(fd));
	fd = open(".", O_RDONLY);
	chk("directory fd (read error) returns NULL", get_next_line(fd));
	return (0);
}
EOF

cat > "$TMP/multi_fd.c" <<'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "get_next_line.h"
int main(int ac, char **av)
{
	int		f1 = open(av[1], O_RDONLY), f2 = open(av[2], O_RDONLY);
	char	*a = get_next_line(f1), *b = get_next_line(f2);
	char	*c = get_next_line(f1);
	int		good = a && b && strcmp(a, "hello\n") == 0
			&& strncmp(b, "x", 1) == 0 && c && strcmp(c, "world\n") == 0;
	printf("%s\n", good ? "MULTIFD_OK" : "MULTIFD_NO");
	free(a); free(b); free(c);
	return (0);
}
EOF

# ---------------------------------------------------------------- test files
T="$TMP/files"; mkdir -p "$T"
printf 'hello\nworld\n'              > "$T/01_two_lines.txt"
printf 'no newline at end'           > "$T/02_no_final_newline.txt"
: >                                    "$T/03_empty.txt"
printf '\n\n\n'                      > "$T/04_only_newlines.txt"
printf 'x'                           > "$T/05_one_char.txt"
printf '\n'                          > "$T/06_one_newline.txt"
printf 'a\nb\nc\nd\ne\n'             > "$T/07_short_lines.txt"
printf 'first\n\nthird after blank\n' > "$T/08_blank_line_middle.txt"
head -c 5000  /dev/zero | tr '\0' 'A' > "$T/09_long_line_no_nl.txt"
{ head -c 5000 /dev/zero | tr '\0' 'B'; echo; } > "$T/10_long_line_nl.txt"
awk 'BEGIN{srand(1); for(i=0;i<400;i++){n=int(rand()*130); s=""; for(j=0;j<n;j++) s=s substr("abc XYZ",int(rand()*7)+1,1); print s}}' > "$T/11_random_lines.txt"
awk 'BEGIN{for(i=1;i<=20000;i++) print "line number " i}' > "$T/12_big_file.txt"
printf 'ends with two\n\n'           > "$T/13_ends_blank.txt"

# -------------------------------------------------------------------- norm
title "Norminette"
if command -v norminette >/dev/null 2>&1; then
	out=$(cd "$PROJECT" && norminette $(ls *.c *.h 2>/dev/null) 2>&1)
	bad=$(echo "$out" | grep -v ': OK!' | grep -v '^$')
	if [ -z "$bad" ]; then ok "norminette: all files OK"; else ko "norminette reports errors" "$bad"; fi
else
	info "norminette not installed -> skipped (pip install norminette)"
fi

# ----------------------------------------------------------------- compile
title "Compilation ($CC $FLAGS)"
COMPILE_OK=1
mkdir -p "$TMP/obj"
if out=$(cd "$TMP/obj" && $CC $FLAGS -D BUFFER_SIZE=42 -I"$PROJECT" -c $SRC 2>&1); then
	ok "library compiles clean with BUFFER_SIZE=42"
else
	ko "does not compile -> Moulinette stops here (score 0)" "$out"; COMPILE_OK=0
fi
if [ $COMPILE_OK = 1 ]; then
	if out=$(cd "$TMP/obj" && $CC $FLAGS -I"$PROJECT" -c $SRC 2>&1); then
		ok "compiles with no -D BUFFER_SIZE (header default is used)"
	else
		ko "fails without -D BUFFER_SIZE (needs #ifndef default in header)" "$out"
	fi
	rm -rf "$TMP/obj"
fi

if [ $COMPILE_OK = 0 ]; then
	printf "\n%sRESULT: KO (cannot continue)%s  %d ok / %d ko\n" "$R" "$N" "$PASS" "$FAIL"
	exit 1
fi

# ------------------------------------------------------------- correctness
build() { # $1=BS $2=main.c $3=output $4=extra flags
	$CC $FLAGS $4 -g -D BUFFER_SIZE=$1 -I"$PROJECT" $SRC "$TMP/$2" -o "$3" 2>&1
}

title "Output correctness (every file x every BUFFER_SIZE)"
for BS in $BUFFER_SIZES; do
	if ! out=$(build $BS main.c "$TMP/bin_$BS"); then ko "build BUFFER_SIZE=$BS" "$out"; continue; fi
	bad=""
	for f in "$T"/*.txt; do
		if ! $TIMEOUT "$TMP/bin_$BS" "$f" > "$TMP/out.txt" 2>/dev/null; then
			bad="$bad\n$(basename "$f"): crashed / timed out / non-zero exit"
		elif ! cmp -s "$TMP/out.txt" "$f"; then
			bad="$bad\n$(basename "$f"): output differs"
		fi
	done
	if [ -z "$bad" ]; then ok "BUFFER_SIZE=$BS: all $(ls "$T" | wc -l | tr -d ' ') files identical"
	else ko "BUFFER_SIZE=$BS" "$(printf "$bad")"; fi
done

title "stdin"
build 42 main.c "$TMP/bin_stdin" >/dev/null
if [ "$(printf 'from\nstdin' | $TIMEOUT "$TMP/bin_stdin")" = "$(printf 'from\nstdin')" ]; then
	ok "reads from stdin (fd 0)"; else ko "stdin read wrong"; fi

# ---------------------------------------------------------- memory / leaks
title "Memory errors and leaks (mode: $MEMMODE)"
mem_files="01_two_lines 02_no_final_newline 03_empty 04_only_newlines 09_long_line_no_nl 10_long_line_nl 11_random_lines"
mem_sizes="1 5 42 10000000"
run_mem() { # $1=BS  $2=file ; prints problems
	case $MEMMODE in
	valgrind)
		valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all \
			--error-exitcode=42 -q "$TMP/bin_$1" "$2" > /dev/null 2> "$TMP/mem.txt"
		[ $? = 42 ] && head -6 "$TMP/mem.txt" ;;
	asan)
		ASAN_OPTIONS=detect_leaks=1 "$TMP/asan_$1" "$2" > /dev/null 2> "$TMP/mem.txt"
		grep -q "AddressSanitizer\|runtime error" "$TMP/mem.txt" && grep -E "ERROR|SUMMARY|leaked" "$TMP/mem.txt" | head -4 ;;
	leaks)
		leaks --atExit -- "$TMP/bin_$1" "$2" > "$TMP/mem.txt" 2>&1
		grep -E "[1-9][0-9]* leaks? for" "$TMP/mem.txt" | head -3 ;;
	esac
}
if [ "$MEMMODE" = "none" ]; then
	info "no valgrind / ASan / leaks available -> skipped"
else
	for BS in $mem_sizes; do
		[ "$MEMMODE" = "asan" ] && build $BS main.c "$TMP/asan_$BS" "-fsanitize=address,undefined" >/dev/null
		bad=""
		for n in $mem_files; do
			r=$(run_mem $BS "$T/$n.txt")
			[ -n "$r" ] && bad="$bad$n:\n$r\n"
		done
		if [ -z "$bad" ]; then ok "BUFFER_SIZE=$BS: no leaks, no invalid reads/writes"
		else ko "BUFFER_SIZE=$BS: memory problem" "$(printf "$bad")"; fi
	done
	# early stop: caller stops reading before EOF, then frees the line (static storage remains by design)
	info "leaks of the static 'storage' when you stop before EOF are expected by the subject"
fi

# ---------------------------------------------------------------- edge cases
title "Edge cases"
if out=$(build 42 edge.c "$TMP/bin_edge"); then
	res=$($TIMEOUT "$TMP/bin_edge" "$T/01_two_lines.txt" 2>&1)
	while IFS= read -r line; do
		case "$line" in
		GOOD*) ok "${line#GOOD }" ;;
		BAD*)  ko "${line#BAD }" ;;
		esac
	done <<EOF
$res
EOF
else ko "edge harness would not build" "$out"; fi

if out=$($CC -g -D BUFFER_SIZE=0 -I"$PROJECT" $SRC "$TMP/main.c" -o "$TMP/bin_zero" 2>&1); then
	if [ -z "$($TIMEOUT "$TMP/bin_zero" "$T/01_two_lines.txt" 2>/dev/null)" ]; then
		ok "BUFFER_SIZE=0 returns NULL (no crash)"; else ko "BUFFER_SIZE=0 should return NULL"; fi
else ko "BUFFER_SIZE=0 build failed" "$out"; fi

# -------------------------------------------------------------- bonus (info)
title "Bonus readiness (informational only, does not count)"
printf 'hello\nworld\n' > "$TMP/f1.txt"; printf 'xxxx\nyyyy\n' > "$TMP/f2.txt"
if $CC -D BUFFER_SIZE=42 -I"$PROJECT" $SRC "$TMP/multi_fd.c" -o "$TMP/bin_multi" >/dev/null 2>&1; then
	if [ "$($TIMEOUT "$TMP/bin_multi" "$TMP/f1.txt" "$TMP/f2.txt")" = "MULTIFD_OK" ]; then
		info "interleaving 2 fds works (bonus-ready)"
	else
		info "interleaving 2 fds NOT supported (fine for mandatory, needed for bonus)"
	fi
fi

# ------------------------------------------------------------------- summary
printf "\n"
if [ $FAIL = 0 ]; then
	printf "%sRESULT: OK%s  %d checks passed\n" "$G" "$N" "$PASS"; exit 0
else
	printf "%sRESULT: KO%s  %d passed, %d failed\n" "$R" "$N" "$PASS" "$FAIL"; exit 1
fi
