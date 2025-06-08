# Color Lines - GTK/C++ Version

This is a C++ implementation of the Color Lines game using the GTKmm 4.0 library for the graphical user interface.

## System Requirements

- A Linux-based operating system is recommended (e.g., Ubuntu, Fedora).
- C++ Compiler (g++ is used in the Makefile).
- pkg-config utility.
- GTKmm 4.0 development libraries.

## Dependencies

The primary dependencies are:

- **g++:** The GNU C++ compiler.
- **pkg-config:** A helper tool used to compile and link against libraries.
- **GTKmm 4.0:** The C++ interface for GTK. On Debian-based systems (like Ubuntu), this can be installed via the `libgtkmm-4.0-dev` package.

### Installation Example (Ubuntu/Debian):

```bash
sudo apt update
sudo apt install g++ pkg-config libgtkmm-4.0-dev
```

For other distributions, use the appropriate package manager to install these dependencies. For example, on Fedora:

```bash
sudo dnf install gcc-c++ pkgconfig gtkmm4.0-devel
```

## Build Instructions

1.  Navigate to the `GTK_CPP` directory in your terminal:
    ```bash
    cd path/to/your/project/GTK_CPP
    ```
2.  Run the `make` command to compile the application:
    ```bash
    make
    ```
    This will generate an executable file named `color_lines_gtk` in the current directory.

3.  To clean the build files (object files and the executable), you can run:
    ```bash
    make clean
    ```

## Running the Application

After successfully building the application, you can run it from the `GTK_CPP` directory:

```bash
./color_lines_gtk
```

This will launch the Color Lines game window.
