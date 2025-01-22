Here is a README for your repository:

# cppTask

Simple TUI task manager written in C++.

## Description

cppTask is a simple terminal user interface (TUI) task manager built with C++. It allows users to manage tasks efficiently directly from the terminal.

## Libraries Used

- [nlohmann/json](https://github.com/nlohmann/json) - JSON for Modern C++
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) - Functional Terminal User Interface

## Build Instructions

### Prerequisites

- C++17 compatible compiler
- [CMake](https://cmake.org/)
- [Ninja](https://ninja-build.org/)

### Steps

1. Clone the repository:
   ```sh
   git clone https://github.com/s-blanco-dev/cppTask.git
   cd cppTask
   ```

2. Create a build directory:
   ```sh
   mkdir build
   cd build
   ```

3. Run CMake with Ninja:
   ```sh
   cmake -G Ninja ..
   ```

4. Build the project:
   ```sh
   ninja
   ```

5. Run the application:
   ```sh
   ./cppTask
   ```

## Usage

Once you have built the application, you can run it using the command mentioned above. The TUI will allow you to add, edit, and delete tasks directly from the terminal.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

Feel free to modify the instructions or add any additional details as necessary.
