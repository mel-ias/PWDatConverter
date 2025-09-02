#pragma once
#include <fstream>
#include <string>

class PWConverter
{
public:
    PWConverter();
    ~PWConverter();

    // Convert from TXT (XYZrgb, any separator) to PW (binary)
    void convert_txt2pw(std::ifstream &in, std::string &path2);

    // Convert from PW (binary) to TXT (XYZrgb, tab-separated)
    void convert_bin2txt(std::ifstream &in, std::string &path2);

    // Alias for convert_bin2txt
    void convert_pw(std::ifstream &in, std::string &path2);

private:
    bool color;
    int loaded_percent;
};