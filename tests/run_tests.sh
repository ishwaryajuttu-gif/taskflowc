#!/usr/bin/env bash
# Builds taskflowc and checks every command, including regression cases for
# the storage, ordering, argument and overflow bugs fixed in #2.
# Usage: bash tests/run_tests.sh   (from any folder; needs gcc)

set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
WORK="$(mktemp -d)"
trap 'cd /; rm -rf "$WORK"' EXIT
BIN="$WORK/taskflowc"

if ! gcc -Wall -Wextra -Werror -std=c99 "$ROOT/src/task_manager.c" "$ROOT/src/main.c" -o "$BIN"; then
    echo "Build failed"
    exit 1
fi

# Every test runs in an empty temp folder so tasks.txt never touches the repo
cd "$WORK"

passed=0
failed=0

# Run the program; sets OUT (stdout without Windows CRs) and STATUS
run() {
    OUT="$("$BIN" "$@")"
    STATUS=$?
    OUT="${OUT//$'\r'/}"
}

# Contents of tasks.txt without Windows CRs
tasks() {
    tr -d '\r' < tasks.txt 2>/dev/null
}

reset() {
    rm -f tasks.txt
}

repeat() {
    printf "%${2}s" "" | tr ' ' "$1"
}

expect_eq() {
    if [ "$2" == "$3" ]; then
        passed=$((passed + 1))
        echo "PASS  $1"
    else
        failed=$((failed + 1))
        echo "FAIL  $1"
        printf '      expected: %s\n      actual:   %s\n' "$2" "$3"
    fi
}

USAGE_LINE="Usage:"
BAD_ID="Error: ID must be a positive whole number (see 'list')."

echo "== Basic commands"
reset
run list
expect_eq "list with no tasks file" "No tasks found." "$OUT"

run add "Buy milk" "grocery" "2026-06-30"
expect_eq "add prints confirmation with ID" "Task added: Buy milk (ID 1)" "$OUT"
expect_eq "add saves the task" "1|Buy milk|grocery|2026-06-30|0" "$(tasks)"

run list
expect_eq "list shows the task" "ID: 1 | Title: Buy milk | Due: 2026-06-30 | Done: No" "$OUT"

run complete 1
expect_eq "complete prints confirmation" "Task marked complete: Buy milk (ID 1)" "$OUT"
expect_eq "complete saves done flag" "1|Buy milk|grocery|2026-06-30|1" "$(tasks)"

run complete 99
expect_eq "complete unknown ID" "Task not found." "$OUT"

run delete 1
expect_eq "delete prints confirmation" "Task deleted: Buy milk (ID 1)" "$OUT"
expect_eq "delete removes the task" "" "$(tasks)"

run delete 99
expect_eq "delete unknown ID" "Task not found." "$OUT"

echo "== Empty fields load (data loss regression)"
reset
run add A first 2026-01-01
run add B "" 2026-01-02
run add C third 2026-01-03
run list
expect_eq "task after an empty description still loads" \
    "$(printf 'ID: 1 | Title: A | Due: 2026-01-01 | Done: No\nID: 2 | Title: B | Due: 2026-01-02 | Done: No\nID: 3 | Title: C | Due: 2026-01-03 | Done: No')" "$OUT"
run add D fourth ""
expect_eq "no tasks lost after saving again" \
    "$(printf '1|A|first|2026-01-01|0\n2|B||2026-01-02|0\n3|C|third|2026-01-03|0\n4|D|fourth||0')" "$(tasks)"

echo "== Order is preserved (ordering regression)"
reset
for t in T1 T2 T3 T4; do run add "$t" d 2026; done
expect_eq "adds keep insertion order" \
    "$(printf '1|T1|d|2026|0\n2|T2|d|2026|0\n3|T3|d|2026|0\n4|T4|d|2026|0')" "$(tasks)"
run delete 1
run delete 4
run add T5 d 2026
expect_eq "delete head and tail, then add appends at the end" \
    "$(printf '2|T2|d|2026|0\n3|T3|d|2026|0\n4|T5|d|2026|0')" "$(tasks)"
run delete 2
run delete 3
run delete 4
run add T6 d 2026
expect_eq "add after deleting every task" "1|T6|d|2026|0" "$(tasks)"

echo "== Missing or extra arguments (crash regression)"
reset
for args in "" "add" "add OnlyTitle" "add T D" "complete" "delete" "complete 1 2" "list extra" "bogus"; do
    # shellcheck disable=SC2086
    run $args
    expect_eq "'${args:-<none>}' exits with status 1" "1" "$STATUS"
    expect_eq "'${args:-<none>}' prints usage" "$USAGE_LINE" "${OUT%%$'\n'*}"
done
expect_eq "bad arguments don't create tasks.txt" "no" "$([ -e tasks.txt ] && echo yes || echo no)"

echo "== Task IDs"
reset
run add Same a 2026
run add Same b 2026
run add Same c 2026
run complete 2
expect_eq "complete picks the right duplicate title" \
    "$(printf '1|Same|a|2026|0\n2|Same|b|2026|1\n3|Same|c|2026|0')" "$(tasks)"
run delete 1
expect_eq "delete picks the right duplicate title" "Task deleted: Same (ID 1)" "$OUT"
expect_eq "other duplicates are kept" "$(printf '2|Same|b|2026|1\n3|Same|c|2026|0')" "$(tasks)"
run add Same d 2026
expect_eq "new ID is one more than the highest" "Task added: Same (ID 4)" "$OUT"

before="$(tasks)"
for bad in abc 0 -1 +1 01 1x "" 2147483648 99999999999; do
    run complete "$bad"
    expect_eq "complete '$bad' rejected" "$BAD_ID" "$OUT"
    expect_eq "complete '$bad' exits with status 1" "1" "$STATUS"
    run delete "$bad"
    expect_eq "delete '$bad' rejected" "$BAD_ID" "$OUT"
done
expect_eq "invalid IDs leave the file unchanged" "$before" "$(tasks)"
run complete 2147483647
expect_eq "largest ID is accepted" "Task not found." "$OUT"

reset
printf '7|A|a|d|0\n' > tasks.txt
run add B b d
expect_eq "IDs continue from the highest in the file" "Task added: B (ID 8)" "$OUT"

echo "== Files without IDs still load"
reset
printf 'Old|one|2026|0\nOld2||2026|1\n' > tasks.txt
run list
expect_eq "old-format tasks are numbered on load" \
    "$(printf 'ID: 1 | Title: Old | Due: 2026 | Done: No\nID: 2 | Title: Old2 | Due: 2026 | Done: Yes')" "$OUT"
expect_eq "list doesn't rewrite the file" "$(printf 'Old|one|2026|0\nOld2||2026|1')" "$(tasks)"
run add New n 2026
expect_eq "next save writes IDs" \
    "$(printf '1|Old|one|2026|0\n2|Old2||2026|1\n3|New|n|2026|0')" "$(tasks)"

reset
printf '5|A|a|d|0\nB|b|d|0\n' > tasks.txt
run list
expect_eq "old-format task after an ID gets the next free ID" \
    "$(printf 'ID: 5 | Title: A | Due: d | Done: No\nID: 6 | Title: B | Due: d | Done: No')" "$OUT"

echo "== Input validation (overflow regression)"
reset
run add "$(repeat x 99)" d 2026
expect_eq "99-character title accepted" "Task added: $(repeat x 99) (ID 1)" "$OUT"
run add "$(repeat x 100)" d 2026
expect_eq "100-character title rejected" "Error: Title is too long (max 99 characters)." "$OUT"
run add ok "$(repeat y 254)" 2026
expect_eq "254-character description accepted" "Task added: ok (ID 2)" "$OUT"
run add ok2 "$(repeat y 255)" 2026
expect_eq "255-character description rejected" "Error: Description is too long (max 254 characters)." "$OUT"
run add ok3 d "$(repeat 9 20)"
expect_eq "20-character due date rejected" "Error: Due date is too long (max 19 characters)." "$OUT"
run add "a|b" d 2026
expect_eq "'|' in title rejected" "Error: Title cannot contain '|' or line breaks." "$OUT"
run add t "$(printf 'line1\nline2')" 2026
expect_eq "line break in description rejected" "Error: Description cannot contain '|' or line breaks." "$OUT"
run add "" d 2026
expect_eq "empty title rejected" "Error: title cannot be empty." "$OUT"
expect_eq "only the two valid tasks were saved" "2" "$(tasks | wc -l | tr -d ' ')"
run list
expect_eq "max-length fields load back" "2" "$(printf '%s\n' "$OUT" | grep -c '^ID:')"

echo "== Corrupt tasks.txt is never overwritten"
for bad in "broken line" "2|A|b|c|2" "A|b|c|0|extra" "2|A|b|c|0|extra" "0|A|b|c|0" "1|Dup|b|c|0" "$(repeat z 600)"; do
    reset
    run add Keep me 2026
    printf '%s\n' "$bad" >> tasks.txt
    before="$(tasks)"
    run list
    expect_eq "list refuses '${bad:0:20}'" "1" "$STATUS"
    run add New task 2026
    expect_eq "add refuses '${bad:0:20}'" "1" "$STATUS"
    expect_eq "file untouched after '${bad:0:20}'" "$before" "$(tasks)"
done

echo "== Windows line endings"
reset
printf '3|W|win|2026|1\r\n' > tasks.txt
run list
expect_eq "CRLF file loads" "ID: 3 | Title: W | Due: 2026 | Done: Yes" "$OUT"
printf 'W|win|2026|1\r\n' > tasks.txt
run list
expect_eq "CRLF file without IDs loads" "ID: 1 | Title: W | Due: 2026 | Done: Yes" "$OUT"

echo
echo "$passed passed, $failed failed"
[ "$failed" -eq 0 ]
