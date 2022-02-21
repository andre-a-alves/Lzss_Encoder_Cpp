//
// Created by andre on 2/20/22.
//
#include <bitset> // For testing encoding/decoding
#include "Lzss.h"

unsigned char Lzss::tokenSize(unsigned short* distancePtr, std::ifstream* inputFilePtr, std::vector<char>* inputWindowPtr, char firstByte)
{
    // If there are not 3 previous characters, there is nothing worth checking
    if (inputWindowPtr->size() < 3)
        return 1;

    unsigned char tokenLength = 0;
    unsigned short sizeBeingChecked = 1;
    char nextByte;
    bool tokenFound = true;
    auto inputWindowSize = inputWindowPtr->size();
    std::vector<char> stringToCheck {firstByte};
    while (inputFilePtr->get(nextByte) && tokenFound && sizeBeingChecked++ <= MAX_TOKEN_SIZE)
    {
        stringToCheck.push_back(nextByte);
        if (sizeBeingChecked > 2)
        {
            tokenFound = false;
            for (int i = 0; i <= inputWindowSize - sizeBeingChecked; i++)
            {
                for (short j = 0; j < sizeBeingChecked; j++)
                {
                    if (stringToCheck.at(j) != inputWindowPtr->at(i + j))
                        break;
                    if (j + 1 == sizeBeingChecked)
                    {
                        tokenFound = true;
                        tokenLength = sizeBeingChecked;
                        *distancePtr = (short)inputWindowSize - i - 1;
                    }
                }
            }
        }
    }
    while (sizeBeingChecked-- > 0)
        inputFilePtr->unget();


    if (tokenLength)
        return tokenLength;
    return 1;
}

void Lzss::encode(const std::string& inputFileName, const std::string& outputFileName)
{
    char byte;
    std::ifstream inputFile(inputFileName);
    std::ofstream outputFile(outputFileName);
    std::vector<char> inputWindow; // sliding window of earlier text
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

//    inputFile.putback('.');
//    inputFile.putback('.');

    while(inputFile.get(byte))
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
        while (inputWindow.size() > WINDOW_SIZE - 1)
            inputWindow.erase(inputWindow.begin());
        inputWindow.push_back(byte);

        //
        unsigned short distance;
        unsigned char tokenSize = Lzss::tokenSize(&distance, &inputFile, &inputWindow, byte); //(short &distance, ifstream &inputFile, vector<char> &inputWindow)
        encodingList <<= 1;

        // Token size of 1 means we are writing the character
        if (tokenSize == 1)
            outputBuffer.push_back(byte);
        else
        {
            for (int i = 1; i < tokenSize; ++i)
            {
                inputFile.get(byte);
                inputWindow.push_back(byte);
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
    }
    inputFile.clear();
    inputFile.seekg(-2, std::ios_base::end);
    int newCount = 0;
    while (nextCharIndex++ < 8)
    {
        encodingList << 1;
        if (newCount++ < 2)
        {
            inputFile.get(byte);
            outputBuffer.push_back(byte);
        }
        else
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

                // Testing the decoding
                std::cout << "Encoded:  " << std::bitset<8>(nextByte);

                distance |= nextByte;
                distance <<= 4;
                inputFile.get(nextByte);

                // Testing the decoding
                std::cout << std::bitset<8>(nextByte) << std::endl;

                distance |= (nextByte >> 4) & 0x000f;
                tokenSize |= nextByte & 0x000f;

                //Testing the decoding
                std::cout << "Distance: " << std::bitset<12>(distance) << std::endl;
                std::cout << "Token:                " << std::bitset<4>(tokenSize) << std::endl << std::endl;

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