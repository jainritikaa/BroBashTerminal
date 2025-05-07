#include "huffman.h"
#include <sstream>

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    Node(Node* l, Node* r) : ch('\0'), freq(l->freq + r->freq), left(l), right(r) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) { return a->freq > b->freq; }
};

void buildCodeMap(Node* root, const std::string& str, std::unordered_map<char, std::string>& codeMap) {
    if (!root) return;
    if (!root->left && !root->right) codeMap[root->ch] = str;
    buildCodeMap(root->left, str + "0", codeMap);
    buildCodeMap(root->right, str + "1", codeMap);
}

std::unordered_map<char, std::string> buildHuffmanCode(const std::string& text) {
    std::unordered_map<char, int> freq;
    for (char c : text) freq[c]++;

    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    for (auto& [ch, f] : freq) pq.push(new Node(ch, f));

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        pq.push(new Node(left, right));
    }

    std::unordered_map<char, std::string> codeMap;
    buildCodeMap(pq.top(), "", codeMap);
    return codeMap;
}

std::string compressText(const std::string& text) {
    auto codeMap = buildHuffmanCode(text);
    std::string binary;
    for (char c : text) binary += codeMap[c];
    return serializeCodeMap(codeMap) + "\n====\n" + binary; // Include codeMap in file
}

std::string decompressText(const std::string& data, const std::unordered_map<char, std::string>& codeMap) {
    std::unordered_map<std::string, char> reverseMap;
    for (auto& [ch, code] : codeMap) reverseMap[code] = ch;

    std::string result, current;
    for (char bit : data) {
        current += bit;
        if (reverseMap.count(current)) {
            result += reverseMap[current];
            current = "";
        }
    }
    return result;
}

std::string serializeCodeMap(const std::unordered_map<char, std::string>& codeMap) {
    std::ostringstream oss;
    for (auto& [ch, code] : codeMap)
        oss << static_cast<int>(ch) << ":" << code << ",";
    return oss.str();
}

std::unordered_map<char, std::string> deserializeCodeMap(const std::string& str) {
    std::unordered_map<char, std::string> map;
    std::istringstream iss(str);
    std::string item;
    while (getline(iss, item, ',')) {
        if (item.empty()) continue;
        size_t pos = item.find(':');
        char ch = static_cast<char>(stoi(item.substr(0, pos)));
        std::string code = item.substr(pos + 1);
        map[ch] = code;
    }
    return map;
}
