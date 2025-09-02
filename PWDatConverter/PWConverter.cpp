#include "PWConverter.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

PWConverter::PWConverter() : color(true), loaded_percent(0) {}

PWConverter::~PWConverter() {}

void PWConverter::convert_txt2pw(std::ifstream &in, std::string &path2) {
    std::cout << "Converting TXT to PW: " << path2 << std::endl;
    std::ofstream out(path2, std::ios::binary | std::ios::trunc);
    if (!out.good()) {
        std::cout << "Can't open output file " << path2 << std::endl;
        return;
    }

    // Always write color flag (1)
    out.put(1);

    std::string line;
    size_t point_count = 0;
    std::cout << "Start reading TXT...\n";
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        // Detect separator
        char sep = '\t';
        if (line.find(',') != std::string::npos) sep = ',';
        else if (line.find(';') != std::string::npos) sep = ';';
        else if (line.find('\t') != std::string::npos) sep = '\t';
        else sep = ' '; // fallback

        std::istringstream ss(line);
        std::string token;
        double x = 0, y = 0, z = 0;
        int r = 0, g = 0, b = 0;

        // Read X
        if (!std::getline(ss, token, sep)) continue;
        x = std::stod(token);

        // Read Y
        if (!std::getline(ss, token, sep)) continue;
        y = std::stod(token);

        // Read Z
        if (!std::getline(ss, token, sep)) continue;
        z = std::stod(token);

        // Read R
        if (!std::getline(ss, token, sep)) continue;
        r = std::stoi(token);

        // Read G
        if (!std::getline(ss, token, sep)) continue;
        g = std::stoi(token);

        // Read B
        if (!std::getline(ss, token, sep)) continue;
        b = std::stoi(token);

        // Write to binary
        out.write(reinterpret_cast<const char*>(&x), sizeof(x));
        out.write(reinterpret_cast<const char*>(&y), sizeof(y));
        out.write(reinterpret_cast<const char*>(&z), sizeof(z));
        out.put(static_cast<unsigned char>(r));
        out.put(static_cast<unsigned char>(g));
        out.put(static_cast<unsigned char>(b));
        ++point_count;
    }
    in.close();
    out.close();
    std::cout << "Converted " << point_count << " points to PW.\n";
}

void PWConverter::convert_bin2txt(std::ifstream &in, std::string &path2) {
    std::cout << "Converting PW to TXT: " << path2 << std::endl;

    // Read color flag
    int color_flag = in.get();
    if (color_flag != 1) {
        std::cout << "PW file does not contain color. Aborting.\n";
        return;
    }

    // Determine number of points
    std::streampos start = in.tellg();
    in.seekg(0, std::ios::end);
    std::streampos end = in.tellg();
    in.seekg(start, std::ios::beg);

    size_t point_size = 3 * sizeof(double) + 3 * sizeof(unsigned char);
    size_t num_points = (end - start) / point_size;

    std::ofstream out(path2, std::ios::trunc);
    if (!out.good()) {
        std::cout << "Can't open output file " << path2 << std::endl;
        return;
    }

	// Set adaptive precision for floating point output
    out << std::setprecision(10) << std::defaultfloat;

    std::cout << "Start reading PW...\n";
    for (size_t i = 0; i < num_points; ++i) {
        double x, y, z;
        unsigned char r, g, b;
        in.read(reinterpret_cast<char*>(&x), sizeof(x));
        in.read(reinterpret_cast<char*>(&y), sizeof(y));
        in.read(reinterpret_cast<char*>(&z), sizeof(z));
        r = in.get();
        g = in.get();
        b = in.get();
        out << x << "\t" << y << "\t" << z << "\t"
            << static_cast<int>(r) << "\t"
            << static_cast<int>(g) << "\t"
            << static_cast<int>(b) << "\n";
    }
    in.close();
    out.close();
    std::cout << "Converted " << num_points << " points to TXT.\n";
}

void PWConverter::convert_pw(std::ifstream &in, std::string &path2) {
    convert_bin2txt(in, path2);
}