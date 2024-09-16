# AIQ: Command-Line AI Query Tool

**AIQ** is a command-line tool that converts natural language instructions into executable terminal commands using a Language Model (LLM). It allows you to describe what you want to do in plain English, and it generates the corresponding command-line command, inserting it into your terminal for you to review and execute.

---

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Notes](#notes)
- [License](#license)
- [Contributing](#contributing)
- [Acknowledgments](#acknowledgments)
- [Contact](#contact)

---

## Features

- **Natural Language to Command-Line**: Converts plain English instructions into shell commands.
- **Interactive**: Automatically inserts the generated command into your terminal for review before execution.
- **Customizable**: Easily extendable and customizable to fit your needs.
- **Integration**: Llama.cpp server.

---

## Installation

### Prerequisites

- **C++ Compiler**: Clang.
- **libcurl**: Install development files for libcurl.
- **xdotool**: For simulating keyboard input.

### Steps

TBD

---

## Usage

Simply type `aiq` followed by your natural language instruction:

```bash
aiq <your instruction here>
```

- The generated command will be inserted into your terminal, allowing you to review and execute it.
- **Important**: Always review the generated command before executing it to ensure it does what you expect.

---

## Examples

Here are some examples of how to use `aiq`:

1. **List the `~/bin` directory and show symlink paths**

   ```bash
   aiq list ~/bin directory and show symlinks paths
   ```

   - **Generated Command:**

     ```bash
     ls -l ~/bin
     ```

2. **Pick randomly from Alice, Bob, or Charlie**

   ```bash
   aiq pick randomly from Alice, Bob or Charlie
   ```

   - **Generated Command:**

     ```bash
     shuf -n 1 -e Alice Bob Charlie
     ```

3. **Calculate 69 + 420**

   ```bash
   aiq calculate 69 + 420
   ```

   - **Generated Command:**

     ```bash
     echo $((69 + 420))
     ```

4. **Current time in human-readable format**

   ```bash
   aiq current time in human readable format
   ```

   - **Generated Command:**

     ```bash
     date +"%A, %B %d, %Y %I:%M:%S %p"
     ```

5. **Get the list of the files in the `libglu1-mesa-dev` package**

   ```bash
   aiq get the list of the files in the libglu1-mesa-dev package
   ```

   - **Generated Command:**

     ```bash
     dpkg -L libglu1-mesa-dev
     ```

6. **Find all `.cpp` files containing "main"**

   ```bash
   aiq find all .cpp files containing "main"
   ```

   - **Generated Command:**

     ```bash
     grep -rl "main" *.cpp
     ```

7. **Show disk usage of the `/home` directory**

   ```bash
   aiq show disk usage of the /home directory
   ```

   - **Generated Command:**

     ```bash
     du -h /home
     ```

8. **Compress all `.log` files in the current directory**

   ```bash
   aiq compress all .log files in the current directory
   ```

   - **Generated Command:**

     ```bash
     tar -czvf logs.tar.gz *.log
     ```

9. **Display the last 20 lines of `syslog`**

   ```bash
   aiq display the last 20 lines of syslog
   ```

   - **Generated Command:**

     ```bash
     tail -n 20 /var/log/syslog
     ```

10. **Monitor network traffic in real-time**

    ```bash
    aiq monitor network traffic in real-time
    ```

    - **Generated Command:**

      ```bash
      iftop
      ```

    - *Note*: May require `sudo` and installation of `iftop`.

11. **Check if port 8080 is open**

    ```bash
    aiq check if port 8080 is open
    ```

    - **Generated Command:**

      ```bash
      netstat -tuln | grep :8080
      ```

12. **Ping `google.com` five times**

    ```bash
    aiq ping google.com five times
    ```

    - **Generated Command:**

      ```bash
      ping -c 5 google.com
      ```

13. **Show all running Docker containers**

    ```bash
    aiq show all running Docker containers
    ```

    - **Generated Command:**

      ```bash
      docker ps
      ```

14. **Clone a Git repository from `https://github.com/example/repo.git`**

    ```bash
    aiq clone a Git repository from https://github.com/example/repo.git
    ```

    - **Generated Command:**

      ```bash
      git clone https://github.com/example/repo.git
      ```

15. **Convert `file.txt` from DOS to Unix format**

    ```bash
    aiq convert file.txt from DOS to Unix format
    ```

    - **Generated Command:**

      ```bash
      dos2unix file.txt
      ```

16. **Create a Python virtual environment in the current directory**

    ```bash
    aiq create a Python virtual environment in the current directory
    ```

    - **Generated Command:**

      ```bash
      python3 -m venv venv
      ```

17. **Find and delete all `.tmp` files**

    ```bash
    aiq find and delete all .tmp files
    ```

    - **Generated Command:**

      ```bash
      find . -type f -name "*.tmp" -delete
      ```

18. **List all installed packages on Ubuntu**

    ```bash
    aiq list all installed packages on Ubuntu
    ```

    - **Generated Command:**

      ```bash
      dpkg --get-selections
      ```

19. **Show current Git branch**

    ```bash
    aiq show current Git branch

    ```

    - **Generated Command:**

      ```bash
      git branch --show-current
      ```

20. **Check disk space usage**

    ```bash
    aiq check disk space usage
    ```

    - **Generated Command:**

      ```bash
      df -h
      ```

---

## Notes

- **Review Commands Carefully**: Always check the generated command before executing it to prevent unintended actions.
- **Error Handling**: If the AI model fails to generate a command or produces an incorrect command, try rephrasing your instruction.
- **Active Terminal Window**: Ensure that your terminal window is active when running `aiq`. The tool types the command into the active terminal window.
- **Shell Compatibility**: The tool uses `xdotool`, which works with X11. It may not function properly on Wayland or non-Linux systems.
- **Security Considerations**: Be cautious when running commands that modify or delete data. Avoid running the tool with elevated privileges.

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Contributing

Contributions are welcome! If you have suggestions for improvements or additional features, feel free to open an issue or submit a pull request.

---

## Acknowledgments

- **xdotool**: For enabling keyboard simulation.

---

## Contact

For any questions or feedback, please contact [mika@mika.global].

---
