# Midterm Project – Implement `ls(1)`

**Student:** Gia Bảo  
**Project:** Simplified UNIX `ls(1)` implemented in C

> Note: The assignment statement refers to a provided manual page, but that manual page was not included with this starter package. This project implements a common educational subset: `-a`, `-l`, `-h`, and `-R`. If the instructor's manual defines different options, update `options.c` and the listing logic to match that specification exactly.

## 1. Project overview

This project implements a simplified version of the UNIX `ls` command from scratch using POSIX filesystem APIs in C.

The program can:

- List files in the current directory or a specified path.
- Hide entries beginning with `.` by default.
- Include hidden entries with `-a`.
- Display long-format metadata with `-l`.
- Display file sizes using human-readable units with `-h`.
- Recursively list subdirectories with `-R`.
- Sort entries alphabetically.
- Detect and report filesystem errors without crashing.
- Display symbolic-link targets in long format.

## 2. Features implemented

### Default mode

```bash
./myls
./myls path/to/directory
./myls file.txt
```

Only non-hidden entries are shown, sorted alphabetically.

### `-a` – all entries

```bash
./myls -a
```

Shows hidden entries whose names start with `.`.

### `-l` – long listing format

```bash
./myls -l
```

Displays:

- File type and permissions
- Number of hard links
- Owner
- Group
- File size
- Modification time
- File name
- Symbolic-link target, when applicable

### `-h` – human-readable size

```bash
./myls -lh
```

Formats sizes such as `512`, `1.5K`, `20M`, etc. The option is meaningful together with `-l`.

### `-R` – recursive listing

```bash
./myls -R
```

Lists the directory and then recursively lists its subdirectories.

Options can be combined:

```bash
./myls -alhR
```

The parser also accepts `--` to indicate the end of options:

```bash
./myls -- -notes.txt
```

## 3. Project structure

```text
GiaBao_midterm_ls/
├── Makefile
├── README.md
├── .gitignore
├── demo.sh
├── include/
│   └── ls.h
└── src/
    ├── main.c
    ├── options.c
    ├── list.c
    ├── format.c
    └── utils.c
```

## 4. Module responsibilities

### `src/main.c`

Program entry point and command-line workflow. It separates file operands from directory operands and handles default `.` behavior.

### `src/options.c`

Parses command-line options and prints usage information.

### `src/list.c`

Handles directory traversal, dynamic entry storage, sorting, recursion, filtering of hidden files, and printing of directory contents.

### `src/format.c`

Contains formatting logic for short and long listing output, including column widths and metadata presentation.

### `src/utils.c`

Contains helper functions for:

- Error messages
- Path construction
- Symbolic-link target reading
- Permission formatting
- Human-readable sizes
- Date/time formatting
- User/group names

### `include/ls.h`

Shared data structures, constants, and function declarations.

## 5. How to compile

Linux / WSL / Ubuntu / Debian:

```bash
make
```

This creates:

```text
myls
```

To remove generated files:

```bash
make clean
```

## 6. How to run

```bash
./myls
./myls -a
./myls -l
./myls -lh
./myls -R
./myls -alhR
```

You can also provide a file or directory:

```bash
./myls src
./myls README.md
./myls include src
```

## 7. Demo test cases

### Test 1 – Default

```bash
./myls
```

Expected behavior: list visible files/directories in alphabetical order.

### Test 2 – Hidden files

```bash
./myls -a
```

Expected behavior: hidden entries such as `.gitignore` are included.

### Test 3 – Long format

```bash
./myls -l
```

Expected behavior: permission bits, owner/group, size, timestamp, and name are displayed.

### Test 4 – Human-readable sizes

```bash
./myls -lh
```

Expected behavior: the size field uses units such as `K`, `M`, or `G` when appropriate.

### Test 5 – Recursive

```bash
./myls -R .
```

Expected behavior: all subdirectories are traversed recursively.

### Test 6 – Error handling

```bash
./myls does_not_exist
```

Expected behavior: an error message is printed and the program exits with a non-zero status; it must not segfault.

### Test 7 – Symbolic link

```bash
ln -s README.md readme_link
./myls -l readme_link
```

Expected behavior: the output identifies the entry as a symbolic link and displays its target.

## 8. Design and robustness

The implementation uses `lstat()` so a symbolic link can be identified without automatically following it.

Directory entries are stored dynamically, preventing a fixed-size directory-entry limit.

Memory allocated for names, paths, directory entries, and formatting strings is released after use.

Filesystem and allocation failures are reported with meaningful messages instead of dereferencing invalid pointers.

Recursive traversal explicitly ignores `.` and `..` to avoid infinite recursion.

## 9. Git / GitHub submission

Create the required repository:

```bash
git init
git add .
git commit -m "Initial implementation of simplified ls"
git branch -M main
git remote add origin https://github.com/<USERNAME>/Name_studentID_midterm.git
git push -u origin main
```

Do **not** commit binaries or object files. The supplied `.gitignore` already excludes `myls` and `*.o`.

Repository URL to include in the report:

```text
https://github.com/<USERNAME>/Name_studentID_midterm
```

## 10. Demo script for presentation

The project includes `demo.sh`, which builds the program and runs the main demonstration cases automatically:

```bash
./demo.sh
```

The same commands are listed below for a manual presentation.


```bash
make clean
make
./myls
./myls -a
./myls -l
./myls -lh
mkdir -p _demo/dir1/dir2
printf 'hello\n' > _demo/a.txt
printf 'world\n' > _demo/.hidden.txt
printf 'nested\n' > _demo/dir1/file.txt
ln -s ../a.txt _demo/dir1/link.txt
./myls -al _demo
./myls -R _demo
./myls -l _demo/dir1/link.txt
./myls does_not_exist
rm -rf _demo
```

## 11. Limitations

This is an educational subset rather than a full implementation of GNU/BSD `ls`. Features not listed above are intentionally not implemented.

The exact option set should be adjusted to the instructor-provided manual page before final submission if that manual specifies additional or different behavior.
