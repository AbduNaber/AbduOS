#include "os.h"
#include "file.h"
#include <iostream> 
#include <limits>

int main(){

    OS::os os;
    //os.createDisk();
    os.loadDisk();
    OS::shell shell;
    OS::shell::currentPath = "/";
    OS::shell::currentFile = os.getFiles()[0];
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.clear(); 
 
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