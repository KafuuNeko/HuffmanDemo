#include <iostream>
#include <string.h>
#include "huffman.hpp"

std::string getStrCode(std::vector<bool> vec)
{
    std::string str;
    for (bool p : vec) str.push_back(p?'1':'0');
    return str;
}

void Test()
{
    const char *str = "Hello World";
    std::stringstream ss(str);
    huffman::Tree huffmanTree(ss);

    auto table = huffmanTree.MakeTable();
    for (auto &p : table)
    {
        std::cout << '[' << (char)p.first << "]:" << getStrCode(p.second) << std::endl;   
    }

    auto enc = huffman::encoding(table, (const huffman::DataType *)str, strlen(str));

    std::cout << getStrCode(enc) << std::endl;
    std::cout <<  huffman::decode_string(table, enc) << std::endl;
}

int main()
{
    Test();
    std::cin.get();
    return 0;
}
