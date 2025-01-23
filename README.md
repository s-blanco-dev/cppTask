# cppTask

cppTask is a simple terminal user interface (TUI) task manager written in C++.

![cpptask-2500](https://github.com/user-attachments/assets/6bd5ef79-3ff7-4ed9-8a3f-697092fe2fc1)

## Features

- Create, edit, and remove tasks
- Add extended descriptions to tasks
- Assign due dates to tasks
- View tasks categorized by tags
- Tasks ordered by due date
- Track and update task progress
- Mark tasks as completed or uncompleted
- Assign priority levels to tasks
- Task colors based on priority levels

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

## Keybindings

- `n`: New task
- `e`: Edit task
- `r`: Remove task
- `enter/spacebar`: Toggle completed
- `tab`: Cycle through tags

## Usage

Once you have built the application, you can run it using the command mentioned above. The TUI will allow you to add, edit, and delete tasks directly from the terminal.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---
