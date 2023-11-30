#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>
using namespace std;
mutex mtx; // Mutex to synchronize output to console

bool moveFile(const string &source, const string &fname, const string &destination, bool overwrite) {
    try {
        scoped_lock lock(mtx); // Lock for synchronized output
        cout << "Moving file: " << source << " to " << destination << "/" << fname << endl;
        // Actual file move logic
        filesystem::rename(source, destination + "/" + fname);
        cout << "File moved successfully." << endl;
    } catch (const filesystem::filesystem_error &e) {
        cerr << "Error moving file: " << e.what() << endl;
        return false;
    }
    return true;
}

bool mover(string &source, string &fname, string &destination, bool overwrite)
{
    if (source.find('*') != string::npos) {
        string parentPath = source.substr(0, source.find('*'));
        vector<thread> threads;
        for (const auto &entry : filesystem::directory_iterator(parentPath)) {
            string individualSource = entry.path().string();
            string individualFname = entry.path().string().substr(entry.path().string().find_last_of('/') + 1);

            string individualDestination = destination;
            threads.emplace_back(moveFile, individualSource, individualFname, individualDestination, overwrite);
        }
        // Join threads
        for (thread &thread : threads) {
            thread.join();
        }
        cout << "Files moved successfully." << endl;
        return true;
    } else {
        filesystem::path oldpath = source;
        filesystem::path newpath = destination + "/" + fname;
        try {
            scoped_lock lock(mtx); // Lock for synchronized output
            cout << "Moving file: " << source << " to " << destination << "/" << fname << endl;
            // Actual file move logic
            filesystem::rename(oldpath, newpath);
            cout << "File moved successfully." << endl;
        } catch (const filesystem::filesystem_error &e) {
            cerr << "Error moving file: " << e.what() << endl;
            return false;
        }
        return true;
    }
}

int main() {
    string cmnd;
    string flag;
    string source;
    string fname = "";
    string destination;
    cout << "shell> ";
    bool overwrite = true;
    cin >> cmnd >> flag >> source >> destination;
    if (flag.find("-i")!=string::npos){
    bool overwrite = false; 
    }else{
    source = flag;
    }
    auto now = chrono::high_resolution_clock::now();
    mover(source, fname, destination, overwrite);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - now);
    std::cout << "Time taken by function "
              << "cp"
              << " is :"
              << duration.count() << " microseconds" << std::endl;
    return 0;
}
