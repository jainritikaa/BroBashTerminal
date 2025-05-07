#include "commands.h"
#include "linkedlist.h"
#include "hashtable.h"
#include "datastructure.h"
#include "huffman.h"
#include <bits/stdc++.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stack>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

HashTable metadataTable;
LinkedList commandHistory;
DirectoryTree directoryTree;

string trim(const string &str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

int levenshteinDistance(const string &s1, const string &s2) {
    int len1 = s1.size(), len2 = s2.size();
    vector<vector<int>> dp(len1 + 1, vector<int>(len2 + 1, 0));
    for (int i = 0; i <= len1; i++) dp[i][0] = i;
    for (int j = 0; j <= len2; j++) dp[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            dp[i][j] = (s1[i - 1] == s2[j - 1]) 
                        ? dp[i - 1][j - 1] 
                        : 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }
    return dp[len1][len2];
}

string reportPerformance(const string& operation, const string& timeComplexity, const string& spaceComplexity, const steady_clock::time_point& start) {
    auto end = steady_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();

    stringstream ss;
    ss << "\n\u2699\ufe0f Operation: " << operation
       << "\n\u23f1\ufe0f Time taken: " << duration << " \u03bcs"
       << "\n\U0001f9e0 Time Complexity: " << timeComplexity
       << "\n\U0001f4be Space Complexity: " << spaceComplexity;
    return ss.str();
}

string banaoCommand(const string &fileName) {
    auto start = steady_clock::now();
    ofstream file(fileName);
    string result;
    if (file) {
        struct stat fileInfo;
        if (stat(fileName.c_str(), &fileInfo) == 0) {
            HashTable::Metadata metadata{fileName, fileInfo.st_size, ctime(&fileInfo.st_mtime)};
            metadata.lastModified.erase(remove(metadata.lastModified.begin(), metadata.lastModified.end(), '\n'), metadata.lastModified.end());
            metadataTable.insertFileMetadata(fileName, metadata);
            result = "Bhai! File '" + fileName + "' ban gaya aur metadata store ho gaya!";
        }
    } else {
        result = "Bhai! Error: File '" + fileName + "' nahi ban paya!";
    }
    result += reportPerformance("banao", "O(1)", "O(1)", start);
    return result;
}

string dikhaoCommand() {
    auto start = steady_clock::now();
    string output = "Bhai! Files ka list dikhao...\n";
    for (const auto &[name, node] : directoryTree.current->children)
        output += "- " + name + "\n";
    output += reportPerformance("dikhao", "O(n)", "O(1)", start);
    return output;
}

string mitaoCommand(const string &fileName) {
    auto start = steady_clock::now();
    string result;
    if (remove(fileName.c_str()) == 0) {
        metadataTable.removeFileMetadata(fileName);
        result = "Bhai! File '" + fileName + "' mita diya gaya!";
    } else {
        result = "Bhai! Error: File '" + fileName + "' nahi mita!";
    }
    result += reportPerformance("mitao", "O(1)", "O(1)", start);
    return result;
}

vector<int> preprocessBadChar(const string &pattern) {
    vector<int> badChar(256, -1);
    for (int i = 0; i < pattern.size(); i++)
        badChar[pattern[i]] = i;
    return badChar;
}

vector<int> boyerMooreSearch(const string &text, const string &pattern) {
    int m = pattern.size(), n = text.size();
    vector<int> badChar = preprocessBadChar(pattern);
    vector<int> result;
    int shift = 0;

    while (shift <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[shift + j]) j--;
        if (j < 0) {
            result.push_back(shift);
            shift += (shift + m < n) ? m - badChar[text[shift + m]] : 1;
        } else {
            shift += max(1, j - badChar[text[shift + j]]);
        }
    }
    return result;
}

string dhoondoCommand(const string &fileName, const string &pattern) {
    auto start = steady_clock::now();
    ifstream file(fileName);
    if (!file) return "Bhai! File '" + fileName + "' nahi mil rahi.";

    string line;
    int lineNum = 0;
    int totalChars = 0;
    set<int> matchedLines;
    vector<pair<int, int>> occurrences;

    // Read the file line-by-line (stream-based)
    while (getline(file, line)) {
        lineNum++;
        totalChars += line.length();
        
        // Apply Boyer-Moore Search on the current line
        vector<int> matches = boyerMooreSearch(line, pattern);
        for (int pos : matches) {
            occurrences.emplace_back(lineNum, pos);
            matchedLines.insert(lineNum);
        }
    }

    struct stat fileInfo;
    double fileSizeKB = 0.0;
    if (stat(fileName.c_str(), &fileInfo) == 0) {
        fileSizeKB = fileInfo.st_size / 1024.0; // Convert size to KB
    }

    // Prepare output message
    string output = occurrences.empty()
        ? "Bhai! Pattern '" + pattern + "' file '" + fileName + "' mein nahi mila."
        : "Bhai! Pattern '" + pattern + "' mila:\n";

    for (auto &[line, pos] : occurrences)
        output += "Line " + to_string(line) + ", Position " + to_string(pos) + "\n";

    output += "\n\U0001f50d Matches: " + to_string(occurrences.size()) + " in " +
              to_string(matchedLines.size()) + " different lines";
    output += "\n\U0001f4c4 File Size: " + to_string(fileSizeKB).substr(0, 4) + " KB | \U0001f4cf Lines: " + to_string(lineNum);
    output += "\n\U0001f520 Text Length: " + to_string(totalChars) + " chars | \U0001f50d Pattern Length: " + to_string(pattern.size());

    // Include performance report
    output += reportPerformance("dhoondo", "O(n + m)", "O(m)", start);
    return output;
}


string jaaneCommand(const string &fileName) {
    auto start = steady_clock::now();
    auto *meta = metadataTable.getFileMetadata(fileName);
    string result = meta
        ? "Bhai! Dekho file ke baare mein kuch baatein:\nFile ka naam: " + meta->filePath +
          "\nSize: " + to_string(meta->fileSize) + " bytes\nLast Modified: " + meta->lastModified
        : "Bhai! Kuch gadbad hai, metadata fetch karne mein!";
    result += reportPerformance("jaane", "O(1)", "O(1)", start);
    return result;
}

string padhCommand(const string &fileName) {
    auto start = steady_clock::now();
    ifstream file(fileName);
    string result;
    if (file) {
        stringstream buffer;
        buffer << file.rdbuf();
        string fileContent = buffer.str();

        size_t delimiterPos = fileContent.find("\n====\n");
        if (delimiterPos != string::npos) {
            string codeMapStr = fileContent.substr(0, delimiterPos);
            string binaryData = fileContent.substr(delimiterPos + 7);
            auto codeMap = deserializeCodeMap(codeMapStr);
            string decompressed = decompressText(binaryData, codeMap);
            result = "Bhai! Yeh raha decompress karke file '" + fileName + "' ka content:\n" + decompressed;
        } else {
            result = "Bhai! Format gadbad hai ya compression use nahi hua tha.";
        }
    } else {
        result = "Bhai! File '" + fileName + "' nahi mil raha!";
    }
    result += reportPerformance("padh (Huffman)", "O(n)", "O(n)", start);
    return result;
}

string likhCommand(const string &fileName, const string &content) {
    auto start = steady_clock::now();
    string compressed = compressText(content);
    ofstream file(fileName);
    string result = file
        ? (file << compressed, "Bhai! File '" + fileName + "' mein compress karke likh diya gaya!")
        : "Bhai! File '" + fileName + "' nahi bana sakte!";
    result += reportPerformance("likh (Huffman)", "O(n log n)", "O(n)", start);
    return result;
}
string parseBhaiLang(const string &input) {
    string output;
    size_t spacePos = input.find(" ");
    string command = input.substr(0, spacePos);
    string arg = (spacePos != string::npos) ? input.substr(spacePos + 1) : "";

    commandHistory.addCommand(input);
    metadataTable.incrementCommandCount(command);

    vector<string> validCommands = {
        "banao", "dikhao", "mitao", "jaane", "padh", "likh",
        "chalo", "wapas", "itihas", "dhoondo", "khojo",
        "banaoDir", "jaha", "bye"
    };

    if (find(validCommands.begin(), validCommands.end(), command) == validCommands.end()) {
        string closestMatch = "";
        int minDistance = INT_MAX;
        for (const string &validCommand : validCommands) {
            int distance = levenshteinDistance(command, validCommand);
            if (distance < minDistance) {
                minDistance = distance;
                closestMatch = validCommand;
            }
        }
        return "Bhai! Yeh command nahi samjha: " + command +
               "\nKya tum '" + closestMatch + "' likhna chahte the?";
    }

    if (command == "banao") output = banaoCommand(arg);
    else if (command == "dikhao") output = dikhaoCommand();
    else if (command == "mitao") output = mitaoCommand(arg);
    else if (command == "jaane") output = jaaneCommand(arg);
    else if (command == "padh") output = padhCommand(arg);
    else if (command == "likh") {
        size_t contentPos = arg.find(" ");
        if (contentPos != string::npos) {
            string fileName = arg.substr(0, contentPos);
            string content = arg.substr(contentPos + 1);
            output = likhCommand(fileName, content);
        } else output = "Bhai! File aur likhne ka content specify karo.";
    } else if (command == "dhoondo") {
        size_t separator = arg.find(" ");
        if (separator != string::npos) {
            string fileName = arg.substr(0, separator);
            string pattern = arg.substr(separator + 1);
            output = dhoondoCommand(fileName, pattern);
        } else output = "Bhai! File aur pattern dono specify karo.";
    } else if (command == "chalo") output = directoryTree.chalo(arg);
    else if (command == "wapas") output = directoryTree.wapas();
    else if (command == "itihas") output = commandHistory.itihas();
    else if (command == "khojo") output = directoryTree.khojo(arg);
    else if (command == "banaoDir") output = directoryTree.banaoDir(arg);
    else if (command == "jaha") output = directoryTree.jaha();
    else if (command == "bye") exit(0);

    return output;
}