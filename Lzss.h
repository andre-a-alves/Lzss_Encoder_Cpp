/**
 * @file Lzss.h
 *
 * @brief Declares the namespace for the LZSS functions.
 *
 * @author Andre Alves
 * Contact: andre@alves.me
 *
 */

#ifndef LZSS_ENCODER_CPP_LZSS_H
#define LZSS_ENCODER_CPP_LZSS_H

#define WINDOW_SIZE 128
#define MAX_TOKEN_SIZE 15

#include <iostream>
#include <fstream>
#include <vector>

namespace Lzss
{
    unsigned char tokenSize(unsigned short* distancePtr, std::ifstream* inputFilePtr, std::vector<char>* inputWindowPtr, char firstByte);
    void encode(const std::string& inputFileName, const std::string& outputFileName);
    void decode(const std::string& inputFileName, const std::string& outputFileName);
}

#endif //LZSS_ENCODER_CPP_LZSS_H
