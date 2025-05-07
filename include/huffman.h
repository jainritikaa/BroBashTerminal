#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <unordered_map>
#include <queue>

using namespace std;

string decompressText(const std::string& binary, const std::unordered_map<char, std::string>& huffmanCode);
unordered_map<char, std::string> buildHuffmanCode(const std::string& text);
string compressText(const std::string& text);
string serializeCodeMap(const std::unordered_map<char, std::string>& huffmanCode);
unordered_map<char, std::string> deserializeCodeMap(const std::string& mapString);

#endif
