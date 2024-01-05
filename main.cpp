#include "os.h"
#include "file.h"
#include <iostream> 
int main(){

    OS::os os;
    //os.createDisk();
    os.loadDisk();
    OS::shell shell;
    std::cout<< " <AbduOS>"<< std::endl;
    while(true){
        try{
        shell.run(os);
    }
    catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }
    }
    
    
}