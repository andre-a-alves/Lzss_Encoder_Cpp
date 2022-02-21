/**
 * @file Lzss.c
 *
 * @brief Contains the functions to perform LZSS encoding of an input file. The program is designed to look for tokens
 * with a maximum size of 15 bytes and looks back up to 4095 bytes to find a matching pattern.
 *
 * @version 21 Feb 2022
 *
 * @author Andre Alves
 * Contact: andre@alves.me
 *
 */

#include "Lzss.h"

/**
 * Function that encodes a file using LZSS encoding.
 *
 * @param inputFileName The filename to be encoded
 * @param outputFileName The output filename
 */
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
        // Write data to file if there are 8 tokens in the buffer
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
        encodingList <<= 1;

        // Token size of 1 means we are writing the character
        if (tokenSize == 1)
        {
            outputBuffer.push_back(inputBuffer.front());
            patternSearchWindow.push_back(inputBuffer.front());
            inputBuffer.erase(inputBuffer.begin());
        }
        // Other sizes need to be encoded in 2 characters. The first 12 bits are the distance, and the last four bits
        // are the token size.
        else
        {
            for (int i = 0; i < tokenSize; ++i)
            {
                patternSearchWindow.push_back(inputBuffer.front());
                inputBuffer.erase(inputBuffer.begin());
            }
            encodingList |= 0x01;
            distance <<= 4;
            distance |= tokenSize;
            char firstChar = distance >> 8;
            char secondChar = distance & 0x00ff;
            outputBuffer.push_back(firstChar);
            outputBuffer.push_back(secondChar);
        }
        nextCharIndex++;
        loadInputBuffer(&inputBuffer, &inputFile);
    }

    // Fill the buffer so it has 8 bytes and fill the remaining values with null
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

/**
 * Function that decodes a file encoded in the LZSS method used within this program.
 *
 * @param inputFileName The filename to be encoded
 * @param outputFileName The output filename
 */
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
        // The first byte will say whether the next 8 are encoded or not
        inputFile.get(nextByte);
        char encoding[8] = {0};
        for (char & i : encoding)
        {
            i =  (nextByte >> 7) & 0x01;
            nextByte <<= 1;
        }
        for (char i : encoding)
        {
            // if encoded
            if(i)
            {
                unsigned short distance = 0;
                unsigned char tokenSize = 0;
                // The first byte contains the first 8 bits of the distance
                inputFile.get(nextByte);
                distance |= (nextByte << 4);
                // The second byte contains both the last 4 bits of the distance and the 4 bit token size
                inputFile.get(nextByte);
                distance |= (nextByte >> 4) & 0x000f;
                distance &= 0x0fff;
                tokenSize |= nextByte & 0x000f;
                // Look up the correct characters in the outputWindow vector
                while (tokenSize-- > 0)
                {
                    nextByte = outputWindow.at(outputWindow.size() - distance);
                    outputWindow.push_back(nextByte);
                    outputFile << nextByte;
                }
            }
            // if not encoded
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

/**
 * Utility function which searches for a matching token and returns the size of the token. It also returns the distance
 * to the starting point of the token through dereferencing a pointer to the distance variable.
 *
 * @param distancePtr Variable holding the distance to the beginning of the token
 * @param patternSearchWindowPtr Vector containing the previous characters to search for matching patterns in
 * @param inputBufferPtr Vector containing the possible characters that can appear within the pattern search window
 * @return The number of bytes that match the pattern
 */
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

/**
 * Utility function that will fill an input buffer up to a maximum size with new characters from an input filestream.
 *
 * @param inputBufferPtr The vector being used as an input buffer.
 * @param inputFilePtr The input filestream being loaded.
 */
void Lzss::loadInputBuffer(std::vector<char>* inputBufferPtr, std::ifstream* inputFilePtr)
{
    char byte;
    while (!inputFilePtr->eof() && inputBufferPtr->size() < MAX_TOKEN_SIZE && inputFilePtr->get(byte))
        inputBufferPtr->push_back(byte);
}