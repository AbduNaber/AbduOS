#include "os.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <sstream>

namespace OS{
    // os::os(){
    //     loadDisk();
    // }

    void os::createDisk(){
        
        std::string diskName;
        std::cout << "Enter disk name: ";

        std::cin >> diskName;
        // create disk file if it doesn't exist else say it already exists
        std::ifstream diskFile(diskName);

        if(diskFile.good()){
            std::cout << "Disk already exists" << std::endl;
            throw std::runtime_error("Disk already exists");
        }
        
        // create disk file and fill with asterisks
        std::ofstream disk(diskName);
        for(int i = 0; i < DISK_SIZE*MB; i++){
            disk << ASTERISK;
        }

        std::cout << "Disk created" << std::endl;
        disk.close();


    }
    
    void os::loadDisk(){
        std::string diskName;
        std::cout << "Enter disk name: ";

        std::cin >> diskName;
        // create disk file if it doesn't exist else say it already exists
        std::ifstream diskFile(diskName);

        if(!diskFile.good()){
            std::cout << diskName <<" does not exist" << std::endl;
            throw std::runtime_error("Disk does not exist");
        }
        
        
        // load disk file into memory disk format is that name,path,size,date,type
        std::string line;
        std::getline(diskFile,line);// get rid of first line

        std::vector<std::string> diskFormat = {"name","path","size","date","type"};

        std::vector<std::unordered_map<std::string,std::string>> disk;

        while(std::getline(diskFile,line)){
           
            std::map <std::string,std::string> diskContents;
            std::stringstream ss(line);
            std::string token;
            unsigned int i = 0;
            while(std::getline(ss,token,',')){
                diskContents[diskFormat[i]] = token;
                i++;
            }
            
            if(diskContents["type"]== "d"){
                Directory * dir = new Directory(diskContents["name"],diskContents["path"],static_cast<size_t>(std::stoi(diskContents["size"])),diskContents["date"],nullptr,diskContents["type"]);
                this->files.push_back(dir);
            }
            for (char c : diskContents["type"]) {
            std::cout << "|" << static_cast<int>(c) << "|";
            }
            if(diskContents["type"] == diskFormat[4]){
                std::cout << "asdas"<< std::endl; 
                // get char by char until catch > then break
                std::string fileContents;
                char c;
                diskFile.get(c); // get rid of new line character
                while(diskFile.get(c)){
                   std::cout<< c;
                    if(c == '>'){
                        break;
                    }
                    fileContents += c;
                }
                std::cout << fileContents << std::endl;
               
        }
        
        break;
       
       
       

    }
     diskFile.close();
        std::cout << "Disk loaded" << std::endl;
}


}