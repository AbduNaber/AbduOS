#include "os.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
#include <regex>
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
        //if it exists say it already exists
        std::ifstream diskFile(diskName);

        if(!diskFile.good()){
            std::cout << diskName <<" does not exist" << std::endl;
            throw std::runtime_error("Disk does not exist");
        }
        
        
        // load disk file into memory disk format is that name,path,size,date,type
        std::string line;
        std::getline(diskFile,line);// get rid of first line

        std::vector<std::string> diskFormat = {"name","path","size","date","type"};

        

        while(std::getline(diskFile,line)){
            
            std::map <std::string,std::string> diskContents;
            std::stringstream ss(line);
            std::string token;
            
            unsigned int i = 0;
            while(std::getline(ss,token,',')){
                diskContents[diskFormat[i]] = token;
                i++;
            }
            std::string currentPath = "";
            std::string previousPath = "";
            if(diskContents["type"]== "d"){
                Directory * dir = new Directory(diskContents["name"],diskContents["path"],static_cast<size_t>(std::stoi(diskContents["size"])),diskContents["date"],diskContents["type"]);
                this->files.push_back(dir);
            }
            else if(diskContents["type"] == "rf"){
                
                
                std::string fileContents = getContents(&diskFile);
            
                RegularFile * rfile = new RegularFile(diskContents["name"],diskContents["path"],static_cast<size_t>(std::stoi(diskContents["size"])),diskContents["date"],diskContents["type"],fileContents);
                this->files.push_back(rfile);
        }
        else if(diskContents["type"] == "lf"){
            std::string fileContents = getContents(&diskFile);
            File * lfile = new RegularFile(diskContents["name"],diskContents["path"],static_cast<size_t>(std::stoi(diskContents["size"])),diskContents["date"],diskContents["type"],fileContents);
            this->files.push_back(lfile);
        }
        
        }
        diskFile.close();
        std::cout << "Disk loaded" << std::endl;
    }

    std::string os::getContents( std::ifstream * diskFile){

        std::string fileContents;

        char c;
        diskFile->get(c);
        if (c != '<')
            throw std::runtime_error("File format error");
        
            
        while(diskFile->get(c)){
        
            if(c == '>'){
                break;
            }
            fileContents += c;
        }
        diskFile->get(c);// get rid of newline
        return fileContents;
    }

    void shell::run(OS::os & os){
        
        std::string command;
        // clear buffer from error and characters
        

        while(true){
        std::cin.clear();
        std::cin.ignore(10000,'\n');    
        std::string currentPath = os.files[0]->getPath();
        std::cout << "abdu@abduPC:" ;    
        std::cout << currentPath << "$ ";
        
        std::getline(std::cin,command);
        
        std::vector < std::string >  commands = parseCommand(command);
        if(isValidCommand(commands,os.files)){
            std::cout << "valid command" << std::endl;
        
        }
        
    }
    }
    std::vector < std::string > shell::parseCommand(const std::string& command) const {
        std::stringstream ss(command);
        std::string token = "";
        std::vector<std::string> tokens = {};

        // split command into tokens
        while(std::getline(ss,token,' ')){
            std::cout << token << std::endl;
            tokens.push_back(token);
        }

        return tokens;

    }
    bool shell::isValidCommand(const std::vector < std::string > commands,const std::vector<File *> files) const{

                if (commands[0]== "ls"){
                    if(commands.size()==1 ){
                        return true;
                    }
                    else if(commands.size() == 2){
                        if(commands[1] == "-R")
                            return true;

                    }
                    else
                        throw std::invalid_argument("invalid parameter. -R is avaible"); 
                    
                }

                else if (commands[0]== "mkdir"){
                    if(commands.size() == 2){
                            if(isdigit(commands[1][0])){
                                throw std::invalid_argument("Invalid input. First character cannot be a digit.");
                            }
                            if (!std::regex_match(commands[1], std::regex("^[a-zA-Z0-9_-]+$"))) {
                                    
                                    throw std::invalid_argument("Invalid input. Only alphanumeric characters, underscores, and hyphens are allowed.");
                            }
                            return true;
                    }
                    
                    throw std::invalid_argument("invalid parameter. example usage : mkdir foldername");
                }

                else if (commands[0]== "rm"){
                    if(commands.size() == 2){

                            if (!std::regex_match(commands[1], std::regex("^[a-zA-Z0-9_-]+$"))) {
                                    
                                    throw std::invalid_argument("Invalid input. Only alphanumeric characters, underscores, and hyphens are allowed.");
                            }
                            if(findFile(commands[1],files) == nullptr){
                                throw std::invalid_argument("file does not exist");
                            }
                            return true;
                    }
                    throw std::invalid_argument("invalid parameter. example usage : rm filename");
                        
                }
                else if (commands[0]== "cp"){
                    return true;
                }
                else if (commands[0]== "link"){
                   return true;
                }
                else if (commands[0]== "cd"){
                    return true;
                }

                else if (commands[0]== "cat"){
                    if(commands.size() == 2){
                        if(findFile(commands[1],files) != nullptr){
                            return true;
                        }
                    }
                    throw std::invalid_argument("file does not exists.");
                }

                throw std::invalid_argument("Invalid command");
        

}

File* shell::findFile(const std::string& fileName, const std::vector<File*>& files) const{
    for (auto file : files){
        if(file->getName() == fileName){
            std:: cout << "file found" << std::endl;
            return file;
        }
    }
    return nullptr;

}
/*
void shell::callOS(const std::vector < std::string > commands, OS::os & os)const{
    if(commands[0] == "ls"){
        if(commands.size() == 1)
            os.ls("");
        else 
            os.ls(commands[1]);

    }
    else if(commands[0] == "mkdir"){
        os.mkdir(commands[1]);
    }
    else if(commands[0] == "rm"){
        os.rm(commands[1]);
    }
    else if(commands[0] == "cp"){
        os.cp(commands[1],commands[2]);
    }
    else if(commands[0] == "link"){
        os.link(commands[1],commands[2]);
    }
    else if(commands[0] == "cd"){
        os.cd(commands[1]);
    }
    else if(commands[0] == "cat"){
        OS::RegularFile* fp = dynamic_cast<RegularFile *>(findFile(commands[1],os.files));
        fp->cat();
    }
    
}
*/
}