#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include<filesystem>

using namespace std;
mutex mtx; // Mutex to synchronize output to console

void copyFile(const filesystem::path &source, const filesystem::path &destination)
{
    try
    {
        scoped_lock lock(mtx);
        cout << "Copying: " << source << " to " << destination << endl;
        filesystem::copy_file(source, destination, filesystem::copy_options::overwrite_existing);
    }
    catch (const filesystem::filesystem_error &e)
    {
        cerr << "Error copying file: " << e.what() << endl;
    }
}

void copyDirectory(const filesystem::path &source, const filesystem::path &destination)
{
    try
    {
        scoped_lock lock(mtx);
        cout << "Copying directory: " << source << " to " << destination << endl;
        filesystem::copy(source, destination, filesystem::copy_options::recursive | filesystem::copy_options::overwrite_existing);
    }
    catch (const filesystem::filesystem_error &e)
    {
        cerr << "Error copying directory: " << e.what() << endl;
    }
}

void cp(string &source, string &destination, vector<string> &options)
{
    filesystem::path sourcePath(source);
    filesystem::path destinationPath(destination);

    try
    {
        if (source.find('*') != string::npos)
        {
            string parentPath = source.substr(0, source.find('*'));
            for (const auto &entry : filesystem::directory_iterator(parentPath))
            {
                filesystem::path individualSourcePath(entry.path());
                filesystem::path individualDestinationPath(destination + '/' + entry.path().filename().string());

                if (filesystem::is_regular_file(individualSourcePath))
                {
                    thread(copyFile, individualSourcePath, individualDestinationPath).detach();
                }
                else if (filesystem::is_directory(individualSourcePath) && (options[0] == "-r" || options[0] == "-R"))
                {
                    thread(copyDirectory, individualSourcePath, individualDestinationPath).detach();
                }
            }
            // this_thread::sleep_for(chrono::seconds(1)); // Allow threads to finish before printing the success message
            cout << "Files copied successfully." << endl;
        }
        else if (filesystem::is_regular_file(sourcePath))
        {
            thread(copyFile, sourcePath, destinationPath).detach();
            // this_thread::sleep_for(chrono::seconds(1));
            cout << "File copied successfully." << endl;
        }
        else if (filesystem::is_directory(sourcePath) && (options[0] == "-r" || options[0] == "-R"))
        {
            thread(copyDirectory, sourcePath, destinationPath).detach();
            // this_thread::sleep_for(chrono::seconds(1));
            cout << "Directory copied successfully." << endl;
        }
        else if (filesystem::is_directory(sourcePath))
        {
            cerr << "Source is a directory. Please Use -r flag" << endl;
        }
    }
    catch (const filesystem::filesystem_error &e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}
int main(){
    string cmnd;
    string flag;
    string source;
    string destination;
    cout << "shell> ";
    bool overwrite = true;
    cin >> cmnd >> flag >> source >> destination;
    vector<string> options = {"-r"}; // Adjust options as needed
    auto now = chrono::high_resolution_clock::now();
    cp(source, destination, options);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - now);
    std::cout << "Time taken by function " << "cp" << " is :"
              << duration.count() << " microseconds" << std::endl;
    return 0;
}
