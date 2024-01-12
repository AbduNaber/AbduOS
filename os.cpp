#include "os.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <iomanip>


/**
 * os.cpp
 * Purpose: Implementation of OS class
 * Author: Abdullah TURKMEN
*/


namespace OS{

    // set static variables
    std::string OS::shell::currentPath = "";
    OS::File * OS::shell::currentFile = nullptr;

    /**
     * createDisk
     * Purpose: Creates a disk file with the given name
     * Parameters: None
     * Returns: None
     * Notes: Function read the disk name from the user and creates the disk file with the given name 
     * 
    */
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

    /**
     * loadDisk
     * Purpose: Loads the disk file into the memory
     * Parameters: None
     * Returns: None
     * Notes: Function read the disk name from the user and loads the disk file into the memory
    */
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

        // read line by line
        while(std::getline(diskFile,line)){
            
            std::map <std::string,std::string> diskContents;
            std::stringstream ss(line);
            std::string token;
            
            // split line into tokens
            unsigned int i = 0;
            while(std::getline(ss,token,',')){
                diskContents[diskFormat[i]] = token;
                i++;
            }
            
            // if it is a directory create directory object and push it to files vector
            if(diskContents["type"]== "d"){
                Directory * dir = new Directory(diskContents["name"],diskContents["path"],static_cast<size_t>(std::stoi(diskContents["size"])),diskContents["date"],diskContents["type"]);
                this->files.push_back(dir);

            }
            // if it is a regular file create regular file object and push it to files vector
            else if(diskContents["type"] == "rf"){
                
                
                std::string fileContents = getContents(&diskFile,static_cast<size_t>(std::stoi(diskContents["size"])));
            
                RegularFile * rfile = new RegularFile(diskContents["name"],diskContents["path"],static_cast<size_t>(std::stoi(diskContents["size"])),diskContents["date"],diskContents["type"],fileContents);
                this->files.push_back(rfile);
        }
            // if it is a linked file create linked file object and push it to files vector
            else if(diskContents["type"] == "lf"){
                std::string fileContents = getContents(&diskFile,static_cast<size_t>(std::stoi(diskContents["size"])));
                
                File * lfile = new linkedFile(diskContents["name"],diskContents["path"],static_cast<size_t>(std::stoi(diskContents["size"])),diskContents["date"],diskContents["type"],fileContents);
                linkedFile * lf = dynamic_cast<linkedFile *>(lfile);
                lf->setMainFile(lf->findMainFile(this->files));
                this->files.push_back(lfile);

            }

        setDiskSize();
        }

        // add files to directories . It places files in the right directory
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

    /**
     * saveDisk
     * Purpose: Saves the disk file into the memory
     * Parameters: None
     * Returns: None
     * Notes: Function read the disk name from the user and saves the disk file into the memory
    */
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
        
    }
    
    /**
     * getContents
     * Purpose: Gets the contents of the file
     * Parameters: std::ifstream * diskFile,size_t size
     * Returns: std::string
     * Notes: Function reads the file contents from the disk file and returns it
    */
    std::string os::getContents( std::ifstream * diskFile,size_t size) const {

        std::string fileContents;
        char c;
        diskFile->get(c);
        if (c != '<')
            throw std::runtime_error("File format error");
        // using size_t read file content
        for(size_t i = 0; i < size; i++){
            diskFile->get(c);
            fileContents += c;
        }
    
        return fileContents;
    }

    /**
     * ls
     * Purpose: Lists the files in the current directory
     * notes: if parameter is -R it lists all the files in the current directory recursively
    */
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
                    std::cout<< "   " << f->getSize()<<"bytes";
                    std::cout<< " " << " -> " <<dynamic_cast<linkedFile *>(f)->getMainFilePath();
                  }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

    }

    /**
     * mkdir
     * Purpose: Creates a directory with the given name
     * notes: if parameter is -R it lists all the files in the current directory recursively
    */
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
    
    /**
     * cd
     * Purpose: Changes the current directory
     * notes: .. goes to the parent directory
     *       / goes to the root directory
     *      /path goes to the path directory
     *     if the directory does not exist throws error
     *    if the path is not a directory throws error
    */
    void os::cd(std::string path){

        if(path[0] == '.' && path != ".."){
            if(path == "."){
                return;
            }
            path = path.substr(2);
        }
        else if(path == ".."){
            
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
            if(dynamic_cast<Directory *>(shell::findFile(path,files)) != nullptr){
                 shell::currentPath = path;
                shell::currentFile = shell::findFile(path,files);
                
            }
            else if (dynamic_cast<RegularFile *>(shell::findFile(path,files)) != nullptr){ 
                throw std::invalid_argument("cd: " + getNameFromPath(path) + ": Not a directory");
            }
            else if(dynamic_cast<linkedFile *>(shell::findFile(path,files)) != nullptr){
                if(shell::findFile((dynamic_cast<linkedFile *>(shell::findFile(path,files)))->getMainFilePath(),files)->getType() == "d"){
                    shell::currentPath = path;
                    shell::currentFile = shell::findFile((dynamic_cast<linkedFile *>(shell::findFile(path,files)))->getMainFilePath(),files);
                }
                else{
                    throw std::invalid_argument("cd: " + getNameFromPath(path) + ": Not a directory");
                }
                
            }
            else{
                throw std::invalid_argument("cd: " + getNameFromPath(path) + ": No such file or directory");
            }
           
        }
    }

    /**
     * rm
     * Purpose: Deletes the file with the given name
     * notes: if the file does not exist throws error
     *        if the file is a directory throws error
    */
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
    

    /**
     * link
     * Purpose: Creates a link file with the given name
     * notes: if the source file does not exist throws error
    */
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


        linkedFile * lfile = new linkedFile(getNameFromPath(destpath),destpath,sourcepath.size(),getdate(),"lf",sourcepath);




        if(std::count(destpath.begin(), destpath.end(), '/') != 1){
            
             dynamic_cast<Directory *>(shell::findFile(destpath.substr(0,destpath.find_last_of('/')),files))->addFile(lfile);
        }
           
        else{
             
            dynamic_cast<Directory *>(shell::findFile("/",files))->addFile(lfile);
        }

        files.push_back(lfile);

        std::cout << lfile->getType()<< " link created" << std::endl;


    }
    

    /**
     * getNameFromPath
     * Purpose: Gets the name of the file from the path
     * notes: if the path is / returns root
    */
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
    

    /**
     * cat
     * Purpose: Prints the contents of the file
     * notes: if the file does not exist throws error
     *       if the file is a directory throws error
    */
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
    
    /**
     * cp
     * Purpose: Copies the file with the given name
     * notes: if the source file does not exist throws error
     *       if the source and target files are the same throws error
     *     if there is not enough space throws error
    */
    void os::cp(std::string sourceFilePath, std::string targetFilePath){
        // first try in real file system
        if(targetFilePath == "."){

            targetFilePath = shell::relativeToAbsolutePath(shell::currentPath);
            std::cout << targetFilePath << std::endl;
        }
        
        std::ifstream sourceFile(sourceFilePath);
        if(!sourceFile.good()){
            sourceFile.close();
            
            // if it is not in real file system try in virtual file system
            sourceFilePath = shell::relativeToAbsolutePath(sourceFilePath);
            targetFilePath = shell::relativeToAbsolutePath(targetFilePath);
             std::string targetFileDirPath = "";
            if(targetFilePath.find_last_of('/') == 0){
                targetFileDirPath = "/";
            }
            else{
                targetFileDirPath = targetFilePath.substr(0,targetFilePath.find_last_of('/'));
            }
            

            if(shell::findFile(sourceFilePath,files) == nullptr){
                throw std::invalid_argument("source File does not exist");
            }
            if(sourceFilePath == targetFilePath){
                throw std::invalid_argument("source and target files are the same");
            }
            File * cpfile = shell::findFile(sourceFilePath,files);
            if(this->getDiskSize() + cpfile->getSize() > DISK_SIZE*MB){
                throw std::invalid_argument("Not enough space");
            }
            
            
        
        if(dynamic_cast<Directory *>(cpfile) != nullptr){

        }
        else if (dynamic_cast<RegularFile *>(cpfile) != nullptr){ 
            RegularFile * rfile = new RegularFile(cpfile->getName(),targetFilePath,cpfile->getSize(),getdate(),"rf",dynamic_cast<RegularFile *>(cpfile)->getData());
            
            dynamic_cast<Directory *>(shell::findFile(targetFileDirPath,files))->addFile(rfile);
            files.push_back(rfile);
            std::cout << "File copied" << std::endl;
        }
        else if(dynamic_cast<linkedFile *>(cpfile) != nullptr){
            linkedFile * lfile = new linkedFile(cpfile->getName(),targetFilePath,cpfile->getSize(),getdate(),"lf",dynamic_cast<linkedFile *>(cpfile)->getMainFilePath());
            dynamic_cast<Directory *>(shell::findFile(targetFileDirPath,files))->addFile(lfile);
            files.push_back(lfile);
            std::cout << "File copied" << std::endl;
        }
        
            


        }
        else {
            std::string fileContents;
            while(sourceFile.good()){
                char c;
                sourceFile.get(c);
                fileContents += c;
            }
            sourceFile.close();
            File * cpfile = new RegularFile(getNameFromPath(sourceFilePath),targetFilePath,fileContents.size(),getdate(),"rf",fileContents);
            if(targetFilePath.find_last_of('/')  == 0){
                dynamic_cast<Directory *>(shell::findFile("/",files))->addFile(cpfile);
                files.push_back(cpfile);
                std::cout << "File copied" << std::endl;
            }
            else{
                dynamic_cast<Directory *>(shell::findFile(targetFilePath.substr(0,targetFilePath.find_last_of('/')),files))->addFile(cpfile);
                files.push_back(cpfile);
                std::cout << "File copied" << std::endl;
            }

        }
    }


    /**
     * rmdir
     * Purpose: Deletes the directory with the given name
     * notes: if the directory does not exist throws error
     *      if the directory is not empty throws error
     *    if the directory is root throws error
    */
    void os::rmdir(std::string name){
        std::string path = shell::relativeToAbsolutePath(name);
        if(shell::findFile(path,files) == nullptr){
            throw std::invalid_argument("Directory does not exist");
        }
        if(dynamic_cast<Directory *>(shell::findFile(path,files)) == nullptr){
            throw std::invalid_argument("This is not a directory");
        }
        if(dynamic_cast<Directory *>(shell::findFile(path,files))->getFiles().size() != 0){
            throw std::invalid_argument("Directory is not empty");
        }
        
        
        if(path.find_last_of('/')  == 0){
            
            (dynamic_cast<Directory *>(shell::findFile("/",files)))->deleteFile(shell::findFile(path,files));
        }
        else{
            std::cout << "Directory deleted" << std::endl;
            if(shell::findFile(path.substr(0,path.find_last_of('/')),files) == nullptr){
                throw std::invalid_argument("Directory does not exist");
            
            }
           
            (dynamic_cast<Directory *>(shell::findFile(path.substr(0,path.find_last_of('/')),files)))->deleteFile(shell::findFile(path,files));
        }
        for(int i=0;i<files.size();i++){
            if(files[i]->getPath() == path){
                files.erase(files.begin()+i);
                break;
            }
        }
        

    }
    

    /**
     * shutdown
     * Purpose: Shuts down the OS
     * notes: Saves the disk file into the memory
    */
    void os::shutdown(){
        std::cout << "Shutting down..." << std::endl;
        saveDisk();
        exit(0);
    }
    
    /**
     * getdate
     * Purpose: Gets the current date
    */
    std::string os::getdate(){
        time_t now = time(0);
        tm *ltm = localtime(&now);
        std::string date = std::to_string(ltm->tm_mday) + "." + std::to_string(1 + ltm->tm_mon) + "." + std::to_string(1900 + ltm->tm_year) + " " + std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min);
        return date;
    }

    /**
     * getDiskSize
     * Purpose: Gets the disk size
     * notes: Disk size is the sum of the sizes of the files in the disk
    */
    size_t os::getDiskSize()const {
        return DISK_SIZE;
    }

    /**
     * setDiskSize
     * Purpose: Sets the disk size
     * notes: Disk size is the sum of the sizes of the files in the disk
    */
    void os::setDiskSize(){

        for(auto f : files){
            if(dynamic_cast<Directory *> (f) != nullptr){
                diskSize += 4096;
            }
            else if(dynamic_cast<RegularFile *> (f) != nullptr){
                diskSize += f->getSize();
            }
            else if(dynamic_cast<linkedFile *> (f) != nullptr){
                //
            }
        } 
    }

    ////////////////////////////////////////////
    /**
     * run
     * Purpose: Runs the shell
     
    */
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
    
    /**
     * parseCommand
     * Purpose: Parses the command
    */
    std::vector < std::string > shell::parseCommand(const std::string& command) const {
        std::stringstream ss(command);
        std::string token = "";
        std::vector<std::string> tokens = {};

        // split command into tokens
        while(std::getline(ss,token,' ')){
           
            tokens.push_back(token);
        }

        return tokens;

    }
    
    /**
     * isValidCommand
     * Purpose: Checks if the command is valid
     * notes: if the command is not valid throws error
    */
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
                    if(commands.size() != 3)
                        throw std::invalid_argument("invalid parameter. example usage : cp sourceFilePath targetFilePath");
                    return true;
                }
                else if (commands[0]== "link"){
                    if(commands.size() != 3)
                        throw std::invalid_argument("invalid parameter. example usage : link sourceFilePath targetFilePath");
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
                else if(commands[0] == "rmdir"){
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

    /**
     * findFile
     * Purpose: Finds the file with the given name
     * notes: if the file does not exist returns nullptr
    */
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

    /**
     * callOS
     * Purpose: Calls the OS function with the given command
    */
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
    else if (commands[0]== "rmdir"){
        os.rmdir(commands[1]);
    }
    else if(commands[0] == "shutdown"){
        os.shutdown();
    }
    
}

    /**
     * relativeToAbsolutePath
     * Purpose: Converts the relative path to absolute path
     * notes: if the path is / returns root
    */
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