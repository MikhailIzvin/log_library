# Logger Library

This is a simple header only, thread-safe logging library for C and C++ projects. It supports multiple log levels, colored output, and logging to a file.

## Features

- Thread-safe logging
- Multiple log levels: DEBUG, INFO, WARN, ERROR
- Colored log output to console (can be disabled)
- Log to file or stderr
- Simple log format option
- Customizable log file path
- Support for pretty function names in log output (optional)
- Tag support for log messages (optional)

## Requirements

- C++98 or higher for C++ projects
- C90 or higher for C projects

## Installation

Choose avaliable options in CMakeLists.txt

```sh
cmake -B build
cmake --build build
```

## Usage

### Including the Logger

Include the `logger.h` header file in your source files:
```cpp
#include "logger.h"
```

### Setting the Log File (if you want)

Set the log file path using `log_library_set_log_file`:
```cpp
log_library_set_log_file("log.txt");
```

When you log in file log library remember size of log file and you can write wrapper that track
size of flushed logs in file and swap files when it's needed. 


```cpp
// Return size of flushed logs
log_library_get_log_size();
```

Or you can you use callback

```cpp
// Set max size before use
log_library_set_log_max_size(unsigned int max_size);
log_library_set_max_file_size_callback(log_library_callback callback, void *userdata);
```

### Important if you are using log in file

Before use any log library functions in callback you should
release mutex via `LOG_LIBRARY_UNLOCK()`

```cpp
void maxFileSizeCallback(void *userdata) {
  // You should unlock before using library functions
  LOG_LIBRARY_UNLOCK();
  log_library_set_log_file(logFile);
}
```

Simple example you can find in file_size_tracking

### Logging Messages

Use the provided macros to log messages at different levels:

With tags (LOG_LIBRARY_TAG_SUPPORT)

```cpp
LOGDEBUG("TAG", "This is a debug message");
LOGINFO("TAG", "This is an info message");
LOGWARN("TAG", "This is a warning message");
LOGERROR("TAG", "This is an error message");
```

Without tags

```cpp
LOGDEBUG("This is a debug message");
LOGINFO("This is an info message");
LOGWARN("This is a warning message");
LOGERROR("This is an error message");
```

Using exceptions

```cpp
throw std::runtime_error(EXCEPTION("An error occurred"));
```

`EXCEPTION` macro write time when exception occured in exception message

After that you can print exception whatever you want

Example

```cpp
LOGERROR("%s", e.what());
```

### Examples

You can found examples in exampels directory

## Configuration Options

You can configure the logger using CMake options:

- `PRETTY_FUNCTION`: Enable pretty function names in log output
- `DISABLE_COLORS`: Disable colored log output
- `LOG_LEVEL_ERROR`: Set log level to ERROR
- `LOG_LEVEL_DEBUG`: Set log level to DEBUG
- `LOG_LEVEL_WARN`: Set log level to WARN
- `LOG_LEVEL_INFO`: Set log level to INFO
- `LOG_SIMPLE`: Enable simple log output
- `CUSTOM_LOG_FILE`: Set custom log file
- `DISABLE_FLUSH`: Disable flush after each log message

All avaliable log options

- `LOG_LIBRARY_PRETTY_FUNCTION`: Enable pretty function names in log output
- `LOG_LIBRARY_DISABLE_COLORS`: Disable colored log output
- `LOG_LIBRARY_LOG_LEVEL_ERROR`: Set log level to ERROR
- `LOG_LIBRARY_LOG_LEVEL_DEBUG`: Set log level to DEBUG
- `LOG_LIBRARY_LOG_LEVEL_WARN`: Set log level to WARN
- `LOG_LIBRARY_LOG_LEVEL_INFO`: Set log level to INFO
- `LOG_LIBRARY_LOG_SIMPLE`: Enable simple log output
- `LOG_LIBRARY_DISABLE_FLUSH`: Disable flush after each log message

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request on GitHub.
