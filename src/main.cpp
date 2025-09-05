#include <iostream>
#include <string>
#include "cmd/cmd.h"

int main(int argc, char *argv[])
{

    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    std::cerr << "Logs from your program will appear here!\n";

     if (argc < 2) {
         std::cerr << "No command provided.\n";
         return EXIT_FAILURE;
     }

     std::string name = argv[1];
     std::vector<std::string> args;
     for(int i = 2; i < argc; ++i){
         args.emplace_back(argv[i]);
     }
     Cmd cmd{name, args};

     return switchCmd(cmd);
}
