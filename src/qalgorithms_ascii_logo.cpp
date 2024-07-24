// qalgorithms_ascii_logo.cpp

#include "../include/qalgorithms_ascii_logo.h"

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

namespace q
{
  namespace logo
  {
    void
    printREDpeaks()
    {
      SetConsoleOutputCP(CP_UTF8);
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

      std::cout << "\n";
      std::cout << "    "; SetConsoleTextAttribute(hConsole, LIGHT_RED);
      std::cout << "███"; SetConsoleTextAttribute(hConsole, YELLOW);
      std::cout << " █ "; SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);
      std::cout << "███        "; SetConsoleTextAttribute(hConsole, LIGHT_AQUA);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, AQUA);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, LIGHT_BLUE);
      std::cout << "█\n"; SetConsoleTextAttribute(hConsole, LIGHT_RED);

      std::cout << "    █ █ "; SetConsoleTextAttribute(hConsole, YELLOW);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);
      std::cout << "█ █          "; SetConsoleTextAttribute(hConsole, AQUA);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, LIGHT_BLUE);
      std::cout << "█\n"; SetConsoleTextAttribute(hConsole, RED);

      std::cout << "███ "; SetConsoleTextAttribute(hConsole, LIGHT_RED);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, YELLOW);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, LIGHT_GREEN);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, GREEN);
      std::cout << "██ "; SetConsoleTextAttribute(hConsole, LIGHT_AQUA);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, AQUA);
      std::cout << "██ "; SetConsoleTextAttribute(hConsole, LIGHT_BLUE);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, BLUE);
      std::cout << "█████ "; SetConsoleTextAttribute(hConsole, LIGHT_PURPLE);
      std::cout << "█▀▀\n"; SetConsoleTextAttribute(hConsole, RED);

      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, LIGHT_RED);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, YELLOW);
      std::cout << "█   "; SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, LIGHT_GREEN);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, GREEN);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, LIGHT_AQUA);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, AQUA);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, LIGHT_BLUE);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, BLUE);
      std::cout << "█ █ █ "; SetConsoleTextAttribute(hConsole, LIGHT_PURPLE);
      std::cout << "███\n"; SetConsoleTextAttribute(hConsole, RED);

      std::cout << "███ "; SetConsoleTextAttribute(hConsole, LIGHT_RED);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, YELLOW);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, LIGHT_GREEN);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, GREEN);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, LIGHT_AQUA);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, AQUA);
      std::cout << "██ "; SetConsoleTextAttribute(hConsole, LIGHT_BLUE);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, BLUE);
      std::cout << "█   █ "; SetConsoleTextAttribute(hConsole, LIGHT_PURPLE);
      std::cout << "▄▄█\n"; SetConsoleTextAttribute(hConsole, RED);

      std::cout << "  █         "; SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);
      std::cout << "█\n"; SetConsoleTextAttribute(hConsole, RED);

      std::cout << "  █       "; SetConsoleTextAttribute(hConsole, LIGHT_YELLOW);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, AQUA);
      std::cout << "https://github.com/odea-project/qAlgorithms";

      std::cout << "\n\n\n\n";
      SetConsoleTextAttribute(hConsole, BRIGHT_WHITE);
    }
  }
}

/* NEW LOGO

    ███ █ ███        █ █  █                                                            
    █ █ █ █ █          █  █                                                            
███ ███ █ ███ ███ ██ █ ██ ███ █████ █▀▀                                                
█ █ █ █ █   █ █ █ █  █ █  █ █ █ █ █ ███                                                
███ █ █ ███ █ ███ █  █ ██ █ █ █   █ ▄▄█                                                
  █         █                                                                          
  █       ███ https://github.com/odea-project/qAlgorithms

*/
