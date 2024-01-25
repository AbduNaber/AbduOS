# File System Simulation

This repository contains a simple simulation of a file system in C++. The file system is designed to mimic basic file operations such as creating files and directories, removing files and directories, copying files, linking files, displaying directory contents, and more.

## Overview

### File System Structure

- **File Class:** A base class representing generic files in the file system. It includes common attributes such as name, path, size, date, and type. Additionally, it declares pure virtual functions for file-specific operations like removal and display (`rm`, `cat`).

- **Directory Class:** Inherits from the `File` class and represents a directory in the file system. It contains a collection of files and directories within it. It overrides the base class's functions for operations that don't apply to directories, such as setting data or removing the directory.

- **RegularFile Class:** Inherits from the `File` class and represents a regular file. It includes additional functionality for handling file data. It implements iterators for convenient data manipulation.

- **LinkedFile Class:** Inherits from the `File` class and represents a linked file that points to another file within the system. It provides functionality to set data, remove, and display content based on the linked file.

### Operating System (OS) Class

- **OS Class:** Represents a simple operating system with basic commands like listing files (`ls`), creating directories (`mkdir`), removing files (`rm`), copying files (`cp`), linking files (`link`), displaying file content (`cat`), changing the current directory (`cd`), removing directories (`rmdir`), and shutting down the system (`shutdown`). It interacts with the file system through file operations.

- **Shell Class:** A simple shell interface to interact with the operating system. It parses user commands, validates them, and calls the corresponding OS functions.

## Usage

1. **Compile the Code:** Use the provided Makefile to compile the code.

    ```bash
    make
    ```

2. **Run the Program:** Execute the compiled program.

    ```bash
    ./test
    ```

3. **Use the Shell:** The program will provide a shell interface where you can enter various commands to interact with the file system.

    ```bash
    > ls
    [List of files and directories]
    > mkdir my_directory
    [Directory created]
    > cd my_directory
    [Changed current directory]
    > touch my_file.txt
    [File created]
    > cat my_file.txt
    [Display file content]
    > ...
    ```

## Example Commands

- `ls`: List files and directories in the current directory.
- `mkdir <directory_name>`: Create a new directory.
- `rm <file_or_directory_name>`: Remove a file or directory.
- `cp <source_path> <target_path>`: Copy a file to a new location.
- `link <file_name> <target_path>`: Create a linked file pointing to another file.
- `cat <file_name>`: Display the content of a file.
- `cd <directory_path>`: Change the current directory.
- `rmdir <directory_name>`: Remove a directory.
- `shutdown`: Shut down the operating system.

## Notes

- The file system is designed to work with a virtual disk of size 10 MB (`DISK_SIZE`). Adjust this parameter in the `OS.h` file if needed.
- The file system automatically saves its state upon shutdown and loads it during startup.

Feel free to explore and experiment with various commands to understand the functionality of this simple file system simulation.
For Question: abdturkmen@hotmail.com
