#include "os.h"
#include "file.h"
#include <iostream> 
#include <limits>

int main(){

    // init the OS
    OS::os os;

    // load the disk
    try {
        os.loadDisk();
    }
    catch(std::exception &e){
        std::cout << e.what() << std::endl;
        return 0;
    }
    
    // init the shell
    OS::shell shell;
    OS::shell::currentPath = "/";
    OS::shell::currentFile = os.getFiles()[0];
    
    // clear the buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.clear(); 
    
    // run the shell
    std::cout<< " <AbduOS>"<< std::endl;
    while(true){
        try{
            shell.run(os);
        }
        catch(std::exception &e){
            std::cout << e.what() << std::endl;
        }
    }
   
    return 0;

    
}