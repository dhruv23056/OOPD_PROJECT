#include <vector>
#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <string>
#include <cstdio>
#include <regex>
#include<chrono>
using namespace std;
using namespace std::filesystem;
#define MAX_LEN 250
#define USER "junaid"
#define PASS "jayape12@"
class Shell {
public:
    string Directory;
    
    Shell(string current_directory) {
        string username, password;
        cout << "username: ";
        cin >> username;
        cout << "password: ";
        cin >> password;  
        if(username==USER && password==PASS){
        Directory = current_directory;
        }else{
            cout << "access denied" << endl;
            exit(0);
        }
    }

    void cd(const string& directory) {
        const char *dir = directory.c_str();
        if(chdir(dir)==0){
            Directory = (filesystem::current_path()).string();
            cout << "changed working directory to " << Directory << endl;
        }
        else{
            cout << "no such path exists"<<endl;
        }
    }

  bool mv(string &source,string &fname, string &destination, bool overwrite)
{
    if (source.find('*') != string::npos) // check if source contains '*'
    {
        string parentPath = source.substr(0, source.find('*')); // get the directory path before '*'
        for (const auto & entry : filesystem::directory_iterator(parentPath)) // iterate over all files in the directory
        {
            string individualSource = entry.path().string();
            string fname = entry.path().string().substr(entry.path().string().find_last_of('/') + 1);

            string individualDestination = destination;
            if (!mv(individualSource, fname,individualDestination, overwrite))
            {
                return false;
            }
        }
        cout << "Files moved successfully." << endl;
        return true;
    }
    else
    {
        filesystem::path oldpath = source;
        filesystem::path newpath = destination+"/"+fname;
        cout << "Source is" << source<<endl;
        cout << "Destination is" << destination << endl;
        // Check if the source file exists
        ifstream sourceFile(source);
        if (!sourceFile.is_open())
        {
            cerr << "Error: Source file '" << source << "' not found." << endl;
            return false;
        }
        sourceFile.close();
        if (!overwrite)
        {
            ifstream destFile(destination);
            if (destFile.is_open())
            {
                cerr << "Error: File "<<source<<" already exists in"<<destination<< endl;
                return false;
            }
        }
        try{
            std::filesystem::rename(oldpath, newpath);
            cout << "File moved successfully." << endl;
        }
        catch(const std::filesystem::filesystem_error& e){
            cerr << "Error: Failed to move file." <<e.what()<< endl;
            return false;
        }
        return true;
    }
}


    void rm(const string& files, char options) {
        if (options == 'f' || options == 'F')
        {

            if (filesystem::exists(files))
            {
                filesystem::remove(files);
                cout << "Removed file: " << files << endl;
            }
            else
            {
                cerr << "File not found: " << files << endl;
            }
        }
        else if (options == 'r' || options == 'R')
            {
                if (filesystem::exists(files))
                {
                    filesystem::remove_all(files);
                    cout << "Removed directory: " << files << endl;
                }
                else
                {
                    cerr << "Directory not found: " << files << endl;
                }
            }
        }

    void ls(bool ifLS) {
        if (ifLS)
        {
            for (const auto &entry : filesystem::recursive_directory_iterator(Directory))
            {
                std::cout << filesystem::relative(entry.path(), filesystem::path(".")) << "\t\t";
            }
        }
        else
        {
            for (const auto &entry : filesystem::directory_iterator(Directory))
            {
                std::cout << filesystem::relative(entry.path(), filesystem::path("."))<< "\t\t";
            }
        }
        cout << endl;
    }

    void cp(string &source, string &destination, vector<string> &options)
    {
        filesystem::path sourcePath(source);
        filesystem::path destinationPath(destination);

        try
        {
            if (source.find('*') != string::npos) // check if source contains '*'
            {
                string parentPath = source.substr(0, source.find('*'));              // get the directory path before '*'
                for (const auto &entry : filesystem::directory_iterator(parentPath)) // iterate over all files in the directory
                {
                    filesystem::path individualSourcePath(entry.path());
                    filesystem::path individualDestinationPath(destination + '/' + entry.path().filename().string());

                    if (filesystem::is_regular_file(individualSourcePath))
                    {
                       filesystem::copy_file(individualSourcePath, individualDestinationPath, filesystem::copy_options::overwrite_existing);
                    }
                    else if (filesystem::is_directory(individualSourcePath) && (options[0] == "-r" || options[0] == "-R"))
                    {
                        filesystem::copy(individualSourcePath, individualDestinationPath, filesystem::copy_options::recursive | filesystem::copy_options::overwrite_existing);
                    }
                }
                std::cout << "Files copied successfully." << std::endl;
            }
            else if (filesystem::is_regular_file(sourcePath))
            {
                filesystem::copy_file(sourcePath, destinationPath, filesystem::copy_options::overwrite_existing);
                std::cout << "File copied successfully." << std::endl;
            }
            else if (filesystem::is_directory(sourcePath) && (options[0] == "-r" || options[0] == "-R"))
            {
                filesystem::copy(sourcePath, destinationPath, filesystem::copy_options::recursive | filesystem::copy_options::overwrite_existing);
                std::cout << "Directory copied successfully." << std::endl;
            }
            else if (filesystem::is_directory(sourcePath))
            {
                std::cerr << "Source is a directory. Please Use -r flag" << std::endl;
            }
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};
string handle_command (Shell &shell, string Commmand){
    string output;
    if (Commmand.substr(0, 3) == "cd ")
    {
        string destdirectory = Commmand.substr(3);
        shell.cd(destdirectory);
        output = "success";
    }
    if (Commmand.substr(0, 2) == "ls")
    {
        if(Commmand.find("-r")!=string::npos){
        shell.ls(true);
        output = "success";
        }
        else{
            shell.ls(false);
            output = "success";
        }
    }
    if(Commmand.substr(0,3)=="rm "){
        if(Commmand.size()==2){
            cout << "please specify path and any option " << endl;
            return "0";
        }
        string path = Commmand.substr(6);
        char option = Commmand.at(4);
        shell.rm(path, option);
    }
    if(Commmand.substr(0,3)=="mv "){
        std::istringstream iss(Commmand);

        std::string substrings[4];

        for (int i = 0; i < 4; ++i)
        {
            // Extract substrings using getline and space as a delimiter
            std::getline(iss, substrings[i], ' ');
        }
        cout << substrings[0]<<" "<<substrings[1]<<" "<<substrings[2]<<" "<<substrings[3]<<endl;
        if (substrings[1].find('-')==string::npos){
            substrings[3] = substrings[2];
            substrings[2] = substrings[1];
            substrings[1] = "NULL";
        }
        if(substrings[1]=="-i" or substrings[1]=="-I"){
            shell.mv(substrings[2],substrings[2] ,substrings[3], false);
        }
        else{
            shell.mv(substrings[2], substrings[2],substrings[3], true);
        }
    }
    if(Commmand.substr(0,3)=="cp "){
        std::istringstream iss(Commmand);

        std::string substrings[4];

        for (int i = 0; i < 4; ++i)
        {            std::getline(iss, substrings[i], ' ');
        }
        if (substrings[1].find('-') == string::npos)
        {
            substrings[3] = substrings[2];
            substrings[2] = substrings[1];
            substrings[1] = "NULL";
        }
        if (substrings[1] == "-r" or substrings[1] == "-R")
        {
            vector<string> options{"-r"};
            shell.cp(substrings[2], substrings[3], options);
        }
        else
        {
            vector<string> options{"NULL"};
            shell.cp(substrings[2], substrings[3],options );
        }
    }
    return output;
}

int main()
{
   
    system("clear");
    string currentPath = (filesystem::current_path()).string();
    Shell myShell(currentPath);
    while(true){
        string InputCommand;
        printf("\x1B[31m");
        cout << USER<<"@"<<myShell.Directory<<"$ ";
        getline(cin,InputCommand);
        printf("\033[0m");
        auto start = chrono::high_resolution_clock::now();
        handle_command(myShell,InputCommand);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Time taken by function '"<<InputCommand.substr(0,3)<<"' is :"
                  << duration.count() << " microseconds" << std::endl;
    }
    
    return 0;
}
