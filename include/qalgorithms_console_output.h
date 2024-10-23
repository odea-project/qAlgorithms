#ifndef CONSOLE_OUTPUT_H
#define CONSOLE_OUTPUT_H

#include <iostream>

#ifdef _WIN32
#include <windows.h>

// define colors
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define AQUA 3
#define RED 4
#define PURPLE 5
#define YELLOW 6
#define WHITE 7
#define GREY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_AQUA 11
#define LIGHT_RED 12
#define LIGHT_PURPLE 13
#define LIGHT_YELLOW 14
#define BRIGHT_WHITE 15

// console output
#define PRINT_DONE                                \
  SetConsoleTextAttribute(hConsole, LIGHT_GREEN); \
  std::cout << "done\n";                          \
  SetConsoleTextAttribute(hConsole, BRIGHT_WHITE);

// console output
#define PRINT_DONE_TIME(duration, numItems)        \
  SetConsoleTextAttribute(hConsole, LIGHT_GREEN);  \
  std::cout << "done";                             \
  SetConsoleTextAttribute(hConsole, BRIGHT_WHITE); \
  std::cout << " in " << duration.count() << " s"; \
  std::cout << " (" << numItems << " units)" << std::endl;

// console output
#define PRINT_NO_CENTROIDS                      \
  SetConsoleTextAttribute(hConsole, LIGHT_RED); \
  std::cout << "no centroids found\n";          \
  SetConsoleTextAttribute(hConsole, BRIGHT_WHITE);

// console output
#define PRINT_POLARITY(polarity)                   \
  SetConsoleTextAttribute(hConsole, LIGHT_AQUA);   \
  std::cout << "\npolarity: " << polarity << "\n"; \
  SetConsoleTextAttribute(hConsole, BRIGHT_WHITE);

// console output
#define PRINT_FILE(filename, i, i_total)                                          \
  SetConsoleTextAttribute(hConsole, YELLOW);                                      \
  std::cout << "\n[" << i << " / " << i_total << "]\nfile: " << filename << "\n"; \
  SetConsoleTextAttribute(hConsole, BRIGHT_WHITE);

#define PRINT_STEP(step)             \
  std::cout << step;                 \
  int num_dots = 35 - step.length(); \
  for (int i = 0; i < num_dots; ++i) \
  {                                  \
    std::cout << ".";                \
  }

#define PRINT_READ_FILE             \
  {                                 \
    std::string step = "read file"; \
    PRINT_STEP(step)                \
  }

#define PRINT_FIND_CENTROIDS             \
  {                                      \
    std::string step = "find centroids"; \
    PRINT_STEP(step)                     \
  }

#define PRINT_FIND_EICS             \
  {                                 \
    std::string step = "find EICs"; \
    PRINT_STEP(step)                \
  }

#define PRINT_FIND_CHROMATROGRAPHIC_PEAKS            \
  {                                                  \
    std::string step = "find chromatographic peaks"; \
    PRINT_STEP(step)                                 \
  }

#define PRINT_WRITE_FEATURES_TO_FILE             \
  {                                              \
    std::string step = "write features to file"; \
    PRINT_STEP(step)                             \
  }

#define PRINT_LOGO                                            \
  SetConsoleOutputCP(CP_UTF8);                                \
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);          \
  std::cout << "\n";                                          \
  std::cout << "    ";                                        \
  SetConsoleTextAttribute(hConsole, LIGHT_RED);               \
  std::cout << "███";                                         \
  SetConsoleTextAttribute(hConsole, YELLOW);                  \
  std::cout << " █ ";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);            \
  std::cout << "███        ";                                 \
  SetConsoleTextAttribute(hConsole, LIGHT_AQUA);              \
  std::cout << "█ ";                                          \
  SetConsoleTextAttribute(hConsole, AQUA);                    \
  std::cout << "█  ";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_BLUE);              \
  std::cout << "█\n";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_RED);               \
  std::cout << "    █ █ ";                                    \
  SetConsoleTextAttribute(hConsole, YELLOW);                  \
  std::cout << "█ ";                                          \
  SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);            \
  std::cout << "█ █          ";                               \
  SetConsoleTextAttribute(hConsole, AQUA);                    \
  std::cout << "█  ";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_BLUE);              \
  std::cout << "█\n";                                         \
  SetConsoleTextAttribute(hConsole, RED);                     \
  std::cout << "███ ";                                        \
  SetConsoleTextAttribute(hConsole, LIGHT_RED);               \
  std::cout << "███ ";                                        \
  SetConsoleTextAttribute(hConsole, YELLOW);                  \
  std::cout << "█ ";                                          \
  SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);            \
  std::cout << "███ ";                                        \
  SetConsoleTextAttribute(hConsole, LIGHT_GREEN);             \
  std::cout << "███ ";                                        \
  SetConsoleTextAttribute(hConsole, GREEN);                   \
  std::cout << "██ ";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_AQUA);              \
  std::cout << "█ ";                                          \
  SetConsoleTextAttribute(hConsole, AQUA);                    \
  std::cout << "██ ";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_BLUE);              \
  std::cout << "███ ";                                        \
  SetConsoleTextAttribute(hConsole, BLUE);                    \
  std::cout << "█████ ";                                      \
  SetConsoleTextAttribute(hConsole, LIGHT_PURPLE);            \
  std::cout << "█▀▀\n";                                       \
  SetConsoleTextAttribute(hConsole, RED);                     \
  std::cout << "█ █ ";                                        \
  SetConsoleTextAttribute(hConsole, LIGHT_RED);               \
  std::cout << "█ █ ";                                        \
  SetConsoleTextAttribute(hConsole, YELLOW);                  \
  std::cout << "█   ";                                        \
  SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);            \
  std::cout << "█ ";                                          \
  SetConsoleTextAttribute(hConsole, LIGHT_GREEN);             \
  std::cout << "█ █ ";                                        \
  SetConsoleTextAttribute(hConsole, GREEN);                   \
  std::cout << "█  ";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_AQUA);              \
  std::cout << "█ ";                                          \
  SetConsoleTextAttribute(hConsole, AQUA);                    \
  std::cout << "█  ";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_BLUE);              \
  std::cout << "█ █ ";                                        \
  SetConsoleTextAttribute(hConsole, BLUE);                    \
  std::cout << "█ █ █ ";                                      \
  SetConsoleTextAttribute(hConsole, LIGHT_PURPLE);            \
  std::cout << "███\n";                                       \
  SetConsoleTextAttribute(hConsole, RED);                     \
  std::cout << "███ ";                                        \
  SetConsoleTextAttribute(hConsole, LIGHT_RED);               \
  std::cout << "█ █ ";                                        \
  SetConsoleTextAttribute(hConsole, YELLOW);                  \
  std::cout << "███ ";                                        \
  SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);            \
  std::cout << "█ ";                                          \
  SetConsoleTextAttribute(hConsole, LIGHT_GREEN);             \
  std::cout << "███ ";                                        \
  SetConsoleTextAttribute(hConsole, GREEN);                   \
  std::cout << "█  ";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_AQUA);              \
  std::cout << "█ ";                                          \
  SetConsoleTextAttribute(hConsole, AQUA);                    \
  std::cout << "██ ";                                         \
  SetConsoleTextAttribute(hConsole, LIGHT_BLUE);              \
  std::cout << "█ █ ";                                        \
  SetConsoleTextAttribute(hConsole, BLUE);                    \
  std::cout << "█   █ ";                                      \
  SetConsoleTextAttribute(hConsole, LIGHT_PURPLE);            \
  std::cout << "▄▄█\n";                                       \
  SetConsoleTextAttribute(hConsole, RED);                     \
  std::cout << "  █         ";                                \
  SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);            \
  std::cout << "█\n";                                         \
  SetConsoleTextAttribute(hConsole, RED);                     \
  std::cout << "  █       ";                                  \
  SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);            \
  std::cout << "███ ";                                        \
  SetConsoleTextAttribute(hConsole, AQUA);                    \
  std::cout << "https://github.com/odea-project/qAlgorithms"; \
  std::cout << "\n\n\n\n";                                    \
  SetConsoleTextAttribute(hConsole, BRIGHT_WHITE);
#else
// console output
#define BLACK "\033[0;30m"
#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"
#define AQUA "\033[0;36m"
#define RED "\033[0;31m"
#define PURPLE "\033[0;35m"
#define YELLOW "\033[0;33m"
#define WHITE "\033[0;37m"
#define GREY "\033[1;30m"
#define LIGHT_BLUE "\033[1;34m"
#define LIGHT_GREEN "\033[1;32m"
#define LIGHT_AQUA "\033[1;36m"
#define LIGHT_RED "\033[1;31m"
#define LIGHT_PURPLE "\033[1;35m"
#define LIGHT_YELLOW "\033[1;33m"
#define BRIGHT_WHITE "\033[1;37m"

#define PRINT_DONE std::cout << LIGHT_GREEN << "done" << BRIGHT_WHITE << std::endl;

#define PRINT_DONE_TIME(duration, numItems) \
  std::cout << LIGHT_GREEN << "done" << BRIGHT_WHITE << " in " << duration.count() << " s" << " (" << numItems << " units)" << std::endl;

#define PRINT_NO_CENTROIDS std::cout << LIGHT_RED << "no centroids found" << BRIGHT_WHITE << std::endl;

#define PRINT_POLARITY(polarity) std::cout << LIGHT_AQUA << "\npolarity: " << polarity << "\n" \
                                           << BRIGHT_WHITE;

#define PRINT_FILE(filename, i, i_total) std::cout << YELLOW << "\n[" << i << " / " << i_total << "]\nfile: " << filename << "\n" \
                                                   << BRIGHT_WHITE;

#define PRINT_STEP(step)             \
  std::cout << step;                 \
  int num_dots = 35 - step.length(); \
  for (int i = 0; i < num_dots; ++i) \
  {                                  \
    std::cout << ".";                \
  }

#define PRINT_READ_FILE             \
  {                                 \
    std::string step = "read file"; \
    PRINT_STEP(step)                \
  }

#define PRINT_FIND_CENTROIDS             \
  {                                      \
    std::string step = "find centroids"; \
    PRINT_STEP(step)                     \
  }

#define PRINT_FIND_EICS             \
  {                                 \
    std::string step = "find EICs"; \
    PRINT_STEP(step)                \
  }

#define PRINT_FIND_CHROMATROGRAPHIC_PEAKS            \
  {                                                  \
    std::string step = "find chromatographic peaks"; \
    PRINT_STEP(step)                                 \
  }

#define PRINT_WRITE_FEATURES_TO_FILE             \
  {                                              \
    std::string step = "write features to file"; \
    PRINT_STEP(step)                             \
  }

#define PRINT_LOGO              \
  std::cout << "\n";            \
  std::cout << "qAlgorithms\n"; \
  std::cout << "\n";

#endif
#endif // CONSOLE_OUTPUT_H