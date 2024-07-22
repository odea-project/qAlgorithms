// qalgorithms_ascii_logo.cpp

#include "../include/qalgorithms_ascii_logo.h"

// define colors
#define BLACK 0
#define _M 1
#define _R 2
#define _T 3
#define _Q 4
#define DARK_MAGENTA 5
#define _L 6
#define GREY 7
#define DARK_GREY 8
#define _H 9
#define _O 10
#define _I 11
#define _A 12
#define _S 13
#define _G 14
#define WHITE 15

namespace q
{
  namespace logo
  {
    void
    print_qpeaks()
    {
      SetConsoleOutputCP(CP_UTF8);
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

      std::cout << "\n";
      std::cout << "    "; SetConsoleTextAttribute(hConsole, _A);
      std::cout << "███"; SetConsoleTextAttribute(hConsole, _L);
      std::cout << " █ "; SetConsoleTextAttribute(hConsole, _G);
      std::cout << "███        "; SetConsoleTextAttribute(hConsole, _I);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, _T);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, _H);
      std::cout << "█\n"; SetConsoleTextAttribute(hConsole, _A);

      std::cout << "    █ █ "; SetConsoleTextAttribute(hConsole, _L);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, _G);
      std::cout << "█ █          "; SetConsoleTextAttribute(hConsole, _T);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, _H);
      std::cout << "█\n"; SetConsoleTextAttribute(hConsole, _Q);

      std::cout << "███ "; SetConsoleTextAttribute(hConsole, _A);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, _L);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, _G);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, _O);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, _R);
      std::cout << "██ "; SetConsoleTextAttribute(hConsole, _I);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, _T);
      std::cout << "██ "; SetConsoleTextAttribute(hConsole, _H);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, _M);
      std::cout << "█████ "; SetConsoleTextAttribute(hConsole, _S);
      std::cout << "█▀▀\n"; SetConsoleTextAttribute(hConsole, _Q);

      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, _A);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, _L);
      std::cout << "█   "; SetConsoleTextAttribute(hConsole, _G);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, _O);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, _R);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, _I);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, _T);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, _H);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, _M);
      std::cout << "█ █ █ "; SetConsoleTextAttribute(hConsole, _S);
      std::cout << "███\n"; SetConsoleTextAttribute(hConsole, _Q);

      std::cout << "███ "; SetConsoleTextAttribute(hConsole, _A);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, _L);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, _G);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, _O);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, _R);
      std::cout << "█  "; SetConsoleTextAttribute(hConsole, _I);
      std::cout << "█ "; SetConsoleTextAttribute(hConsole, _T);
      std::cout << "██ "; SetConsoleTextAttribute(hConsole, _H);
      std::cout << "█ █ "; SetConsoleTextAttribute(hConsole, _M);
      std::cout << "█   █ "; SetConsoleTextAttribute(hConsole, _S);
      std::cout << "▄▄█\n"; SetConsoleTextAttribute(hConsole, _Q);

      std::cout << "  █         "; SetConsoleTextAttribute(hConsole, _G);
      std::cout << "█\n"; SetConsoleTextAttribute(hConsole, _Q);

      std::cout << "  █       "; SetConsoleTextAttribute(hConsole, _G);
      std::cout << "███ "; SetConsoleTextAttribute(hConsole, _T);
      std::cout << "https://github.com/odea-project/qAlgorithms";

      std::cout << "\n\n\n\n";
      SetConsoleTextAttribute(hConsole, 15);
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
