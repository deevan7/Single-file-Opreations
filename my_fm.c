#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <string.h>

#define MAX_TEXT_LENGTH 50
#define MAX_BINARY_RANGE 200

void print_string(const char *str, int fd) {
    while (*str != '\0') {
        syscall(SYS_write, fd, str, 1);
        str++;
    }
}

void print_error() {
    char *err_str = "Error\n";
    print_string(err_str, STDERR_FILENO);
}

int is_text_file(const char *filename) {
    int fd = syscall(SYS_open, filename, O_RDONLY);
    if (fd == -1)
        exit(errno);

    char buffer[1];
    int is_text = 1;
    while (syscall(SYS_read, fd, buffer, 1) > 0) {
        if (!(buffer[0] >= 0 && buffer[0] <= 127)) {
            is_text = 0;
            break;
        }
    }
    syscall(SYS_close, fd);
    return is_text;
}

void add_text_to_file(const char *filename, const char *text) {
    int fd = syscall(SYS_open, filename, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (fd == -1)
        exit(errno);

    int len = strlen(text);
    if (len > MAX_TEXT_LENGTH) {
        len = MAX_TEXT_LENGTH;
    }

    if (syscall(SYS_write, fd, text, len) != len) {
        syscall(SYS_close, fd);
        exit(errno);
    }
    syscall(SYS_close, fd);
}

void add_binary_numbers_to_file(const char *filename, int start_number) {
    int fd = syscall(SYS_open, filename, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (fd == -1)
        exit(errno);

    int num_digits = 0;
    int temp = start_number;
    while (temp != 0) {
        temp /= 10;
        num_digits++;
    }

    char num_str[num_digits + 1];
    num_str[num_digits] = '\0';
    
    int total_written = 0;
    for (int i = start_number; i <= MAX_BINARY_RANGE; i += 2) {
        int num = i;
        int j = num_digits - 1;
        do {
            num_str[j--] = num % 10 + '0';
            num /= 10;
        } while (num != 0);

        int len = strlen(num_str);

        if (total_written + len > MAX_TEXT_LENGTH) {
            break;
        }

        if (syscall(SYS_write, fd, num_str, len) != len) {
            syscall(SYS_close, fd);
            exit(errno);
        }

        total_written += len;
    }

    if (total_written < MAX_TEXT_LENGTH) {
        char spaces[MAX_TEXT_LENGTH - total_written];
        memset(spaces, ' ', MAX_TEXT_LENGTH - total_written);
        if (syscall(SYS_write, fd, spaces, MAX_TEXT_LENGTH - total_written) != MAX_TEXT_LENGTH - total_written) {
            syscall(SYS_close, fd);
            exit(errno);
        }
    }

    syscall(SYS_close, fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        char usage[] = "Usage: ";
        print_string(usage, STDERR_FILENO);
        print_string(argv[0], STDERR_FILENO);
        char newline[] = " <command> <filename> [<text> | <start_number>]\n";
        print_string(newline, STDERR_FILENO);
        print_string("Commands:\n", STDERR_FILENO);
        print_string("create - create a new file\n", STDERR_FILENO);
        print_string("createdir - create a new directory\n", STDERR_FILENO);
        print_string("delete - delete a file or directory\n", STDERR_FILENO);
        print_string("rename - rename a file or directory\n", STDERR_FILENO);
        print_string("appendtext - append text to a file\n", STDERR_FILENO);
        print_string("appendbinary - append binary numbers to a file\n", STDERR_FILENO);
        print_string("read - read contents of a file\n", STDERR_FILENO);
        return EXIT_FAILURE;
    }

    const char *command = argv[1];
    const char *filename = argv[2];

    if (strcmp(command, "create") == 0) {
        int fd = syscall(SYS_open, filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
        if (fd == -1) {
            return errno;
        } else {
            syscall(SYS_close, fd);
            print_string("File created successfully.\n", STDOUT_FILENO);
        }
    } else if (strcmp(command, "createdir") == 0) {
        if (syscall(SYS_mkdir, filename, 0777) == -1) {
            return errno;
        } else {
            print_string("Directory created successfully.\n", STDOUT_FILENO);
        }
    } else if (strcmp(command, "delete") == 0) {
        struct stat file_stat;
        if (syscall(SYS_stat, filename, &file_stat) == -1) {
            print_string("Error: File or directory does not exist.\n", STDERR_FILENO);
            return errno;
        }

        if (S_ISDIR(file_stat.st_mode)) {
            if (syscall(SYS_rmdir, filename) == -1) {
                if (syscall(SYS_unlink, filename) == -1) {
                    print_string("Error: Cannot delete a directory.\n", STDERR_FILENO);
                    return errno;
                }
            }
            print_string("Directory deleted successfully.\n", STDOUT_FILENO);
        } else {
            if (syscall(SYS_unlink, filename) == -1) {
                print_string("Error: Cannot delete the file.\n", STDERR_FILENO);
                return errno;
            }
            print_string("File deleted successfully.\n", STDOUT_FILENO);
        }
    } else if (strcmp(command, "rename") == 0) {
        const char *new_name = argv[3];
        int result = syscall(SYS_renameat2, AT_FDCWD, filename, AT_FDCWD, new_name, 0); // No flags
        if (result != 0) {
            return errno;
        } else {
            print_string("File renamed successfully.\n", STDOUT_FILENO);
        }
    } else if (strcmp(command, "appendtext") == 0) {
        if (argc < 4) {
            print_string("Error: Missing text to append.\n", STDERR_FILENO);
            return EINVAL;
        }
        const char *text = argv[3];
        add_text_to_file(filename, text);
    } else if (strcmp(command, "appendbinary") == 0) {
        if (argc < 4) {
            print_string("Error: Missing starting number for binary sequence.\n", STDERR_FILENO);
            return EINVAL;
        }
        int start_number = atoi(argv[3]);
        if (start_number % 2 == 0 || start_number < 50 || start_number > MAX_BINARY_RANGE) {
            print_string("Invalid start number. Please provide an odd number between 50 and ", STDERR_FILENO);
            char range_str[4];
            int len = snprintf(range_str, sizeof(range_str), "%d", MAX_BINARY_RANGE);
            print_string(range_str, STDERR_FILENO);
            print_string(".\n", STDERR_FILENO);
            return EINVAL;
        }
        add_binary_numbers_to_file(filename, start_number);
    } else if (strcmp(command, "read") == 0) {
        int fd = syscall(SYS_open, filename, O_RDONLY);
        if (fd < 0) {
            char *errMsg = "Error opening file: ";
            print_string(errMsg, STDERR_FILENO);
            char *err = strerror(errno);
            print_string(err, STDERR_FILENO);
            char *newline = "\n";
            print_string(newline, STDERR_FILENO);
            return errno;
        }

        char buffer[MAX_TEXT_LENGTH + 1];
        ssize_t bytesRead = syscall(SYS_read, fd, buffer, MAX_TEXT_LENGTH);
        if (bytesRead < 0) {
            char *errMsg = "Error reading file: ";
            print_string(errMsg, STDERR_FILENO);
            char *err = strerror(errno);
            print_string(err, STDERR_FILENO);
            char *newline = "\n";
            print_string(newline, STDERR_FILENO);
            syscall(SYS_close, fd);
            return errno;
        }

        syscall(SYS_close, fd);

        print_string(buffer, STDOUT_FILENO);
    } else {
        // Execute code2 if an invalid command is entered
        if (argc >= 3) {
            int fd = syscall(SYS_open, filename, O_RDONLY);
            if (fd < 0) {
                char *errMsg = "Error opening file: ";
                print_string(errMsg, STDERR_FILENO);
                char *err = strerror(errno);
                print_string(err, STDERR_FILENO);
                char *newline = "\n";
                print_string(newline, STDERR_FILENO);
                return errno;
            }

            char buffer[MAX_TEXT_LENGTH + 1];
            ssize_t bytesRead = syscall(SYS_read, fd, buffer, MAX_TEXT_LENGTH);
            if (bytesRead < 0) {
                char *errMsg = "Error reading file: ";
                print_string(errMsg, STDERR_FILENO);
                char *err = strerror(errno);
                print_string(err, STDERR_FILENO);
                char *newline = "\n";
                print_string(newline, STDERR_FILENO);
                syscall(SYS_close, fd);
                return errno;
            }

            syscall(SYS_close, fd);

            print_string(buffer, STDOUT_FILENO);
        }
    }

    return EXIT_SUCCESS;
}
