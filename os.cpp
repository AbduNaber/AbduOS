#include "os.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <iomanip>


namespace OS{

    // os::os(){
    //     loadDisk();
    // }

    std::string OS::shell::currentPath = "";
    OS::File * OS::shell::currentFile = nullptr;

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

        std::string diskName ;
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
            
            File * lfile = new linkedFile(diskContents["name"],diskContents["path"],static_cast<size_t>(std::stoi(diskContents["size"])),diskContents["date"],diskContents["type"],fileContents);
            linkedFile * lf = dynamic_cast<linkedFile *>(lfile);
            lf->setMainFile(lf->findMainFile(this->files));
            this->files.push_back(lfile);

            std::cout << lfile->getPath() << std::endl;
        }
        
        }

        int i,j;      
        for(i=0;i<files.size();i++){

            if(files[i]->getType() == "d"){
                j=1;
                for(j= j+i;j<files.size();j++){
                    if(files[i]->getPath() == "/"){
                        if(files[j]->getPath() == "/" + files[j]->getName()){
                            dynamic_cast<Directory *>(files[i])->addFile(files[j]);
                        }
                    
                    }
                    else if(files[j]->getPath() == files[i]->getPath() + "/" + files[j]->getName()){
                        dynamic_cast<Directory *>(files[i])->addFile(files[j]);
                        
                    }
                    
                }
            }

        }
           
        diskFile.close();
        std::cout << "Disk loaded" << std::endl;
    }

    void os::saveDisk(){
        std::ofstream diskFile(diskName);
        if(!diskFile.good()){
            throw std::runtime_error("Disk does not exist");
        }
        diskFile << "name,path,size,date,type" << std::endl;
       for(auto f: files){
           if(dynamic_cast<Directory *>(f) != nullptr){
                diskFile<< f->getName() << "," << f->getPath() << "," << f->getSize() << "," << f->getDate() << "," << f->getType() << std::endl;  
            }
            else if(dynamic_cast<RegularFile *>(f) != nullptr){
                diskFile<< f->getName() << "," << f->getPath() << "," << f->getSize() << "," << f->getDate() << "," << f->getType() << std::endl;  
                diskFile << "<" << dynamic_cast<RegularFile *>(f)->getData() << ">" << std::endl;
            }
            else if(dynamic_cast<linkedFile *>(f) != nullptr){
                diskFile<< f->getName() << "," << f->getPath() << "," << f->getSize() << "," << f->getDate() << "," << f->getType() << std::endl;  
                diskFile << "<" << dynamic_cast<linkedFile *>(f)->getMainFilePath() << ">" << std::endl;
            }
            else{
                std::cout <<"File type error passing..." << std::endl;
            }

        }
        diskFile.close();
        std::cout << "Disk saved" << std::endl;
    }
    std::string os::getContents( std::ifstream * diskFile) const {

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

    void os::ls(std::string parameter,File * cf){
        if(parameter == ""){
            
            for(auto f : (dynamic_cast<Directory *>(cf))->getFiles())
            {    
                std::cout << std::left << std::setw(3) << std::setfill(' ') << f->getType()
                  << std::setw(15) << std::setfill(' ') << f->getName()
                  << std::setw(10) << std::setfill(' ') << f->getDate() ;
                  if(f->getType() == "rf") {
                    std::cout<< "   " << f->getSize()<<"bytes";
                  }
                  else if(f->getType() == "lf") {
                    std::cout<< " " << " -> " <<dynamic_cast<linkedFile *>(f)->getMainFilePath();
                  }
                std::cout << std::endl;
            }
            
                
        }
        else if(parameter == "-R"){
            for(auto f : (dynamic_cast<Directory *>(cf))->getFiles())
            {
                if(f->getType() == "d"){
                    ls("-R",f);
                }
                std::cout << std::left << std::setw(3) << std::setfill(' ') << f->getType()
                  << std::setw(15) << std::setfill(' ') << f->getName()
                  << std::setw(10) << std::setfill(' ') << f->getDate() ;
                  if(f->getType() == "rf") {
                    std::cout<< "   " << f->getSize()<<"bytes";
                  }
                  else if(f->getType() == "lf") {
                    std::cout<< " " << " -> " <<dynamic_cast<linkedFile *>(f)->getMainFilePath();
                  }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

    }
 
    void os::mkdir(std::string name){
        std::string path = shell::relativeToAbsolutePath(name);
        Directory * dir = new Directory(name,path,0,getdate(), "d");
        for(auto f : files){
            if(f->getPath() == dir->getPath()){
                delete dir;
                throw std::invalid_argument("Directory already exists");
            }

        }
        std::cout << shell::currentPath<< std::endl;
        for(int i=0;i<files.size();i++){
            if(files[i]->getPath() == shell::currentPath){
                std::cout << "Directory created" << std::endl;
                dynamic_cast<Directory *>(files[i])->addFile(dir);
                files.insert(files.begin()+i+1,dir);
                break;
            }
        }
        std::cout << "Directory created" << std::endl;

    }
    
    void os::cd(std::string path){

        if(path == ".."){
            
            if(shell::currentPath == "/"){
                throw std::invalid_argument("You are already in root directory");
            }

            if(shell::currentPath == "/"+shell::currentFile->getName()){
                shell::currentPath = "/";
                shell::currentFile = shell::findFile("/",files);
                
            }
            else{
                shell::currentPath = shell::currentPath.substr(0,shell::currentPath.find_last_of('/'));
                shell::currentFile = shell::findFile(shell::currentPath,files);
            }
                
        }
        else{
            path = shell::relativeToAbsolutePath(path);
            if(shell::findFile(path,files) == nullptr){
                throw std::invalid_argument("Directory does not exist");
            }
            if(dynamic_cast<Directory *>(shell::findFile(path,files)) == nullptr){
                throw std::invalid_argument("This is not a directory");
            }
            shell::currentPath = path;
            shell::currentFile = shell::findFile(path,files);
        }
    }
    
    void os::rm(std::string name){
        bool found = false;
        for(auto f : (dynamic_cast<Directory *>(shell::currentFile))->getFiles()){
             
            if(f->getName() == name){
                if( dynamic_cast<Directory *>(f) != nullptr){
                    throw std::invalid_argument("Cannot delete directory");
                }

                (dynamic_cast<Directory *>(shell::currentFile))->deleteFile(f);
                files.erase(std::remove(files.begin(), files.end(), f));
                delete f;
                found = true;
            }

        }  
        if(!found){
            throw std::invalid_argument("File does not exist");
        }

    }
    
    void os::link(std::string source, std::string dest) {

        std::string sourcepath = shell::relativeToAbsolutePath(source);
        std::string destpath = shell::relativeToAbsolutePath(dest);
        
        if(shell::findFile(sourcepath,files) == nullptr){
            throw std::invalid_argument("source File does not exist");
        }
        
        if(shell::findFile(destpath.substr(0,destpath.find_last_of('/')),files) == nullptr){
            if(std::count(destpath.begin(), destpath.end(), '/') != 1){
                throw std::invalid_argument("destination directory does not exist");
            }

        }
        if(shell::findFile(destpath,files) != nullptr){
            throw std::invalid_argument("destination file already exists");
        }


        linkedFile * lfile = new linkedFile(getNameFromPath(destpath),destpath,0,"00.00.0000 00:00","lf",sourcepath);




        if(std::count(destpath.begin(), destpath.end(), '/') != 1){
            std::cout << "asdad "<<std::endl;
             dynamic_cast<Directory *>(shell::findFile(destpath.substr(0,destpath.find_last_of('/')),files))->addFile(lfile);
        }
           
        else{
             
            dynamic_cast<Directory *>(shell::findFile("/",files))->addFile(lfile);
        }

        files.push_back(lfile);

        std::cout << lfile->getType()<< " link created" << std::endl;


    }
    
    std::string os::getNameFromPath(std::string path) const {
        if(path == "/"){
            return "root";
        }
        else{
            if(path.find_last_of('/')  == std::string::npos){
                return path;
                }
            else{
                return path.substr(path.find_last_of('/')+1);
            }
        }

    }
    
    void os::cat(std::string name){
        std::string filepath = shell::relativeToAbsolutePath(name);
        if(shell::findFile(filepath,files) == nullptr){
            throw std::invalid_argument("File does not exist");
        }
        if(dynamic_cast<RegularFile *>(shell::findFile(filepath,files)) != nullptr){
            dynamic_cast<RegularFile *>(shell::findFile(filepath,files))->cat();
        }
        else if (dynamic_cast<linkedFile *>(shell::findFile(filepath,files)) != nullptr){ 
            dynamic_cast<linkedFile *>(shell::findFile(filepath,files))->cat();
        }
        // if it is a directory throw cat: filename: is a directory
        else if (dynamic_cast<Directory *>(shell::findFile(filepath,files)) != nullptr){ 
            throw std::invalid_argument("cat: " + getNameFromPath(filepath) + ": is a directory");
        }
        else{
            throw std::invalid_argument("cat: " + getNameFromPath(filepath) + ": No such file or directory");
        }
        
    }
    
    void os::cp(std::string sourceFilePath, std::string targetFilePath){
        // first try in real file system
        std::ifstream sourceF(sourceFilePath);
        if(!sourceF.good()){
            sourceF.close();

            // if it is not in real file system try in virtual file system
            File * sourceFile = shell::findFile(shell::relativeToAbsolutePath(sourceFilePath),files);
            if(sourceFile == nullptr){
                throw std::invalid_argument("source file does not exist");
            }
            else{
                // if it is in virtual file system check if it is a directory
                if(dynamic_cast<Directory *>(sourceFile) != nullptr){
                    throw std::invalid_argument("cannot copy directory");
                }
                // if it is a regular file copy it
                else if(dynamic_cast<RegularFile *>(sourceFile) != nullptr){
                    std::string data = dynamic_cast<RegularFile *>(sourceFile)->getData();
                    File * rf = new RegularFile(getNameFromPath(targetFilePath),shell::relativeToAbsolutePath(targetFilePath),data.size(),"00.00.0000 00:00","rf",data);
                    // cp relative path to absoulte in root burada kaldın.
                    for(int i=0;i<files.size();i++){
                        if(files[i]->getPath() == shell::relativeToAbsolutePath(targetFilePath.substr(0,targetFilePath.find_last_of('/')))){
                            dynamic_cast<Directory *>(files[i])->addFile(rf);
                            files.insert(files.begin()+i+1,rf);
                            break;
                        }
                    }
                    files.push_back(rf);
                    std::cout << "file copied" << std::endl;
                    
        }

    }
        }
        else{
            // check destination path is valid 
            if(shell::findFile(shell::relativeToAbsolutePath(targetFilePath.substr(0,targetFilePath.find_last_of('/'))),files) == nullptr){
                throw std::invalid_argument("destination directory does not exist");
            }
            std::string data = "";    
            while(sourceF.good()){
                char c;
                sourceF.get(c);
                data += c;
            }
            File * rf = new RegularFile(getNameFromPath(targetFilePath),shell::relativeToAbsolutePath(targetFilePath),data.size(),"00.00.0000 00:00","rf",data);
            for(int i=0;i<files.size();i++){
                        if(files[i]->getPath() == shell::relativeToAbsolutePath(targetFilePath.substr(0,targetFilePath.find_last_of('/')))){
                            dynamic_cast<Directory *>(files[i])->addFile(rf);
                            files.insert(files.begin()+i+1,rf);
                            break;
                        }
                    }
            files.push_back(rf);
                    
            sourceF.close();

            std::cout << "file copied" << std::endl;
    }
    }

    void os::shutdown(){
        std::cout << "Shutting down..." << std::endl;
        saveDisk();
        exit(0);
    }
    
    std::string os::getdate(){
        time_t now = time(0);
        tm *ltm = localtime(&now);
        std::string date = std::to_string(ltm->tm_mday) + "." + std::to_string(1 + ltm->tm_mon) + "." + std::to_string(1900 + ltm->tm_year) + " " + std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min);
        std::cout << date << std::endl;
        return date;
    }
    ////////////////////////////////////////////
    void shell::run(OS::os & os){
        
        std::string command;
        std::cout << "abdu@abduPC:" ;    
        std::cout << shell::currentPath << "$ ";

        std::getline(std::cin,command);
       if (command.empty())
            return;

        
        std::vector < std::string >  commands = parseCommand(command);
        if(isValidCommand(commands,os.getFiles())){
            callOS(commands,os);
            os.saveDisk();
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
                            if (!std::regex_match(commands[1], std::regex("^[a-zA-Z0-9_.-]+$"))) {
                                    
                                    throw std::invalid_argument("Invalid input. Only alphanumeric characters, underscores, and hyphens are allowed.");
                            }
                            return true;
                    }
                    
                    throw std::invalid_argument("invalid parameter. example usage : mkdir foldername");
                }

                else if (commands[0]== "rm"){
                    if(commands.size() == 2){

                            if (!std::regex_match(commands[1], std::regex("^[a-zA-Z0-9_.-]+$"))) {
                                    
                                    throw std::invalid_argument("Invalid input. Only alphanumeric characters, underscores, and hyphens are allowed.");
                            }
                            
                            if(findFile(shell::relativeToAbsolutePath(commands[1]),files) == nullptr){
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
                    if(commands.size() == 2){
                        return true;
                    }
                    throw std::invalid_argument("file does not exists.");
                }

                else if (commands[0]== "cat"){
                    if(commands.size() == 2){
                        if(findFile(shell::relativeToAbsolutePath(commands[1]),files) != nullptr){
                            return true;
                        }
                    }
                    throw std::invalid_argument("file does not exists.");
                }
                else if (commands[0]== "shutdown"){
                    return true;
                }
                throw std::invalid_argument("Invalid command");
        

}

    File* shell::findFile(const std::string& filepath, const std::vector<File*>& files){
    for (auto file : files){
        if(file->getPath() == filepath){
            //std:: cout << file->getName()<<" file found" << std::endl;
            return file;
        }
    }
    //std::cout << "not found"<<" "<< filepath << std::endl;
    return nullptr;

}

    void shell::callOS(const std::vector < std::string > commands, OS::os & os)const{

    if(commands[0] == "ls"){
        if(commands.size() == 1)
            os.ls("",shell::currentFile);
        else 
            os.ls(commands[1],shell::currentFile);

    }
    else if(commands[0] == "mkdir"){
        os.mkdir(commands[1]);
    }
    else if(commands[0] == "cd"){
        os.cd(commands[1]);
    }
    else if(commands[0] == "cat"){
        os.cat(commands[1]);
    }
    else if(commands[0] == "rm"){
        os.rm(commands[1]);
    }
    else if(commands[0] == "link"){
        os.link(commands[1],commands[2]);
    }
   
    
     else if(commands[0] == "cp"){
        os.cp(commands[1],commands[2]);
    }    
    else if(commands[0] == "shutdown"){
        os.shutdown();
    }
    
}

std::string shell::relativeToAbsolutePath(const std::string& path){

        if(path[0] == '/'){
            return path;
        }
        else{
            if(shell::currentPath == "/"){
                return shell::currentPath + path;
            }
            else{
                return shell::currentPath + "/" + path;   
            }
        }   

    }
}