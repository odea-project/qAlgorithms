#ifndef CONSOLE_OUTPUT_H
#define CONSOLE_OUTPUT_H

#include <iostream>
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

#endif // CONSOLE_OUTPUT_H