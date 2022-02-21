//
// Created by andre on 2/20/22.
//
#include <bitset> // For testing encoding/decoding
#include "Lzss.h"

unsigned char Lzss::tokenSize(unsigned short* distancePtr, std::vector<char>* patternSearchWindowPtr, std::vector<char>* inputBufferPtr)
{
    // If there are not 3 previous characters or 3 more characters, there is nothing worth checking
    if (patternSearchWindowPtr->size() < 3 || inputBufferPtr->size() < 3)
        return 1;

    unsigned char tokenLength = 0;
    auto patternSearchWindowSize = patternSearchWindowPtr->size();
    auto maxSearchSize = patternSearchWindowSize < inputBufferPtr->size() ?
            patternSearchWindowSize :
            inputBufferPtr->size();
    unsigned char sizeBeingChecked = 3;

    char i = 0;
    while(i < patternSearchWindowSize - sizeBeingChecked && sizeBeingChecked <= maxSearchSize)
    {
        char j = 0;
        while (j < sizeBeingChecked && j < maxSearchSize && i + j < patternSearchWindowSize)
        {
            if (inputBufferPtr->at(j) != patternSearchWindowPtr->at(i + j))
                break;
            if (j == sizeBeingChecked - 1)
            {
                tokenLength = sizeBeingChecked++;
                *distancePtr = patternSearchWindowSize - i;
            }
            j++;
        }
        i++;
    }

    if (tokenLength)
        return tokenLength;
    return 1;
}

void Lzss::loadInputBuffer(std::vector<char>* inputBufferPtr, std::ifstream* inputFilePtr)
{
    char byte;
    while (!inputFilePtr->eof() && inputBufferPtr->size() < MAX_TOKEN_SIZE && inputFilePtr->get(byte))
        inputBufferPtr->push_back(byte);
}

void Lzss::encode(const std::string& inputFileName, const std::string& outputFileName)
{
    std::ifstream inputFile(inputFileName);
    std::ofstream outputFile(outputFileName);
    std::vector<char> patternSearchWindow; // sliding window of earlier text
    std::vector<char> inputBuffer; // contains the text to be analyzed. Necessary to function with fail after EOF
    short nextCharIndex = 0;
    char encodingList = '\0';   // 0 bits means not encoded
    std::vector<char> outputBuffer;

    // Make sure file exists
    if (!inputFile.is_open())
    {
        std::cerr << "Could not open the file - '"
                  << inputFileName << "'" << std::endl;
        return;
    }

    loadInputBuffer(&inputBuffer, &inputFile);
    while(!inputBuffer.empty())
    {
        // Write data if there are 8 tokens
        if (nextCharIndex == 8)
        {
            outputFile << encodingList;
            for (char i : outputBuffer)
                outputFile << i;
            encodingList = '\0';
            outputBuffer.clear();
            nextCharIndex = 0;
        }

        // keep the sliding window no larger than the maximum window size
        while (patternSearchWindow.size() > WINDOW_SIZE)
            patternSearchWindow.erase(patternSearchWindow.begin());

        // get the distance and token size for repeated patterns
        unsigned short distance;
        unsigned char tokenSize = Lzss::tokenSize(&distance, &patternSearchWindow, &inputBuffer);
//        unsigned char tokenSize = Lzss::tokenSize(&distance, &inputFile, &patternSearchWindow, byte); //(short &distance, ifstream &inputFile, vector<char> &patternSearchWindow)
        encodingList <<= 1;

        // Token size of 1 means we are writing the character
        if (tokenSize == 1)
        {
            outputBuffer.push_back(inputBuffer.front());
            patternSearchWindow.push_back(inputBuffer.front());
            inputBuffer.erase(inputBuffer.begin());
        }
        else
        {
            for (int i = 0; i < tokenSize; ++i)
            {
//                outputBuffer.push_back(inputBuffer.front());
                patternSearchWindow.push_back(inputBuffer.front());
                inputBuffer.erase(inputBuffer.begin());
            }
            encodingList |= 0x01;
//            // This was here to test the encoding
//            std::cout << "target:   ";
//            std::cout << std::bitset<12>(distance) << std::bitset<4>(tokenSize) << std::endl;
            distance <<= 4;
            distance |= tokenSize;
            char firstChar = distance >> 8;
            char secondChar = distance & 0x00ff;
//            // This was here to test the encoding
//            std::cout << "Encoding: ";
//            std::cout << std::bitset<8>(firstChar) << std::bitset<8>(secondChar) << std::endl << std::endl;
            outputBuffer.push_back(firstChar);
            outputBuffer.push_back(secondChar);
        }
        nextCharIndex++;
        loadInputBuffer(&inputBuffer, &inputFile);
    }

    while (nextCharIndex++ < 8)
    {
        encodingList <<= 1;
        outputBuffer.push_back('\0');
    }
    outputFile << encodingList;
    for (char c : outputBuffer)
        outputFile << c;

    inputFile.close();
    outputFile.close();
}

void Lzss::decode(const std::string& inputFileName, const std::string& outputFileName)
{
    char nextByte;
    std::ifstream inputFile(inputFileName);
    std::ofstream outputFile(outputFileName);
    std::vector<char> outputWindow;

    // Make sure file exists
    if (!inputFile.is_open())
    {
        std::cerr << "Could not open the file - '"
                  << inputFileName << "'" << std::endl;
        return;
    }

    bool endFlag = false;
    while(!endFlag)
    {
        inputFile.get(nextByte);
        char encoding[8] {0};
        for (char & i : encoding)
        {
            i =  (nextByte >> 7) & 0x01;
            nextByte <<= 1;
        }
        for (char i : encoding)
//        for (int i = 0; i < 8; i++)
        {
            if(i)
//            if (encoding[i])
            {
                unsigned short distance = 0;
                unsigned char tokenSize = 0;
                inputFile.get(nextByte);

//                // Testing the decoding
//                std::cout << "Encoded:  " << std::bitset<8>(nextByte);

                distance |= (nextByte << 4);
                inputFile.get(nextByte);

//                // Testing the decoding
//                std::cout << std::bitset<8>(nextByte) << std::endl;

                distance |= (nextByte >> 4) & 0x000f;
                distance &= 0x0fff;
                tokenSize |= nextByte & 0x000f;

////                Testing the decoding
//                std::cout << "Distance: " << std::bitset<16>(distance) << std::endl;
//                std::cout << "Token:                " << std::bitset<4>(tokenSize) << std::endl << std::endl;

                while (tokenSize-- > 0)
                {
                    nextByte = outputWindow.at(outputWindow.size() - distance);
                    outputWindow.push_back(nextByte);
                    outputFile << nextByte;
                }
            }
            else
            {
                inputFile.get(nextByte);
                if (nextByte == '\0')
                {
                    endFlag = true;
                    break;
                }
                outputWindow.push_back(nextByte);
                outputFile << nextByte;
            }
            while (outputWindow.size() > WINDOW_SIZE)
                outputWindow.erase(outputWindow.begin());
        }
    }

    inputFile.close();
    outputFile.close();
}