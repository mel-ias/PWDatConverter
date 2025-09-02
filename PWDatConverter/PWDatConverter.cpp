#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "PWConverter.hpp"

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Usage:\n"
                  << "  To convert TXT to PW: PWDatConverter.exe txt2pw input.txt output.pw\n"
                  << "  To convert PW to TXT: PWDatConverter.exe pw2txt input.pw output.txt\n";
        return EXIT_FAILURE;
    }

    std::string mode = argv[1];
    std::string input_path = argv[2];
    std::string output_path = argv[3];

    if (!std::filesystem::exists(input_path)) {
        std::cout << "Input file does not exist: " << input_path << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream in(input_path, std::ios::binary);
    if (!in.good()) {
        std::cout << "Cannot open input file: " << input_path << std::endl;
        return EXIT_FAILURE;
    }

    PWConverter converter;

    if (mode == "txt2pw") {
        converter.convert_txt2pw(in, output_path);
    } else if (mode == "pw2txt") {
        converter.convert_bin2txt(in, output_path);
    } else {
        std::cout << "Unknown mode: " << mode << std::endl;
        std::cout << "Use 'txt2pw' or 'pw2txt'." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}