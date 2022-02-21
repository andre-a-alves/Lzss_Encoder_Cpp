/**
 * @file Lzss.h
 *
 * @brief Declares the namespace for the LZSS functions.
 *
 * @version 21 Feb 2022
 *
 * @author Andre Alves
 * Contact: andre@alves.me
 *
 */

#ifndef LZSS_ENCODER_CPP_LZSS_H
#define LZSS_ENCODER_CPP_LZSS_H

#define WINDOW_SIZE 4095
#define MAX_TOKEN_SIZE 15

#include <iostream>
#include <fstream>
#include <vector>

namespace Lzss
{
//    unsigned char tokenSize(unsigned short* distancePtr, std::ifstream* inputFilePtr, std::vector<char>* patterSearchWindowPtr, char firstByte);
    unsigned char tokenSize(unsigned short* distancePtr, std::vector<char>* patternSearchWindowPtr , std::vector<char>* inputBufferPtr);
    void loadInputBuffer(std::vector<char>* inputBufferPtr, std::ifstream* inputFilePtr);
    void encode(const std::string& inputFileName, const std::string& outputFileName);
    void decode(const std::string& inputFileName, const std::string& outputFileName);
}

#endif //LZSS_ENCODER_CPP_LZSS_H
