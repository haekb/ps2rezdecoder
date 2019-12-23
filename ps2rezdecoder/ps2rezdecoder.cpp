// ps2rezdecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "Rez.h"

// See: https://github.com/jarro2783/cxxopts
#include "cxxopts.hpp"

// TODO: Headerize this
unsigned int rezCalcHash(char* szFileName, int g_PS2Rez_56_4_);
void extractRez(Rez* aRez, std::string fileList, int block);
void checkHash(Rez* aRez, std::string fileName);

int main(int argc, char* argv[], char* envp[])
{
    std::string fileList = "file list.txt";
    std::string rezFile = "LITH_PS2.REZ";
    std::string mode = "extract";
    bool showHelp = false;
    int block = 0;

    cxxopts::Options options("PS2 Rez Decoder and Extractor", "Decodes and Extracts a PS2 Lithtech rez file.");
    options.add_options()
        ("h,help", "Help", cxxopts::value(showHelp))
        ("f,file", "File list or name", cxxopts::value(fileList))
        ("r,rez", "Rez name", cxxopts::value(rezFile))
        ("b,block", "Block number", cxxopts::value(block))
        ("m,mode", "Mode", cxxopts::value(mode))
        ;

    auto result = options.parse(argc, argv);

    if (showHelp) {
        std::cout << options.help() << "\n";

        std::cout << "Default Settings:\n";
        std::cout << "File List     : " << fileList << "\n";
        std::cout << "Rez File      : " << rezFile << "\n";
        std::cout << "Block Number  : " << std::to_string(block) << "\n";
        std::cout << "Mode          : " << mode << "\n";
        std::cout << "\n\n";

        return 0;
    }

    if (mode.compare("extract") != 0 && mode.compare("hash") != 0) {
        std::cout << "Error: Mode only supports extract or hash\n";
        return 0;
    }

    std::cout << "Settings:\n";
    std::cout << "File List     : " << fileList << "\n";
    std::cout << "Rez File      : " << rezFile << "\n";
    std::cout << "Block Number  : " << std::to_string(block) << "\n";
    std::cout << "Mode          : " << mode << "\n";
    std::cout << "\n\n";

    Rez* aRez = new Rez();

    if (!aRez->Open(rezFile)) {
        std::cout << "ERROR: Failed to open " << rezFile << "\n";;
        return 1;
    }

    if (mode.compare("extract") == 0) {
        extractRez(aRez, fileList, block);
    }
    else if (mode.compare("hash") == 0) {
        checkHash(aRez, fileList);
    }
    

    aRez->Close();

    return 0;
}

void extractRez(Rez* aRez, std::string fileList, int block)
{
    uint magicNumber = aRez->GetMagicNumber();

    std::vector<std::string> files;

    std::ifstream infile(fileList);

    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iStream(line);
        std::string file = "";
        unsigned int hash = 0;

        if (!(iStream >> file)) {
            std::cout << "Error!";
            break;
        }

        hash = rezCalcHash((char*)file.c_str(), magicNumber);
        aRez->Extract(file, hash, block);
    }

    infile.close();
}

void checkHash(Rez* aRez, std::string fileName)
{
    auto hash = rezCalcHash((char*)fileName.c_str(), aRez->GetMagicNumber());
    std::cout << fileName << "'s hash is " << std::to_string(hash) << "\n";
}

// Extracted from LTIOREZ.IRX
unsigned int rezCalcHash(char* szFileName, int g_PS2Rez_56_4_)
{
    unsigned int uVar1;
    int iVar2;
    int iVar3;

    iVar2 = 0;
    iVar3 = 0;
    while (*(char*)(szFileName + iVar2) != '\0') {
        uVar1 = toupper((int)*(char*)(szFileName + iVar2));
        uVar1 = uVar1 & 0xff;
        if (uVar1 == 0x2f) {
            uVar1 = 0x5c;
        }
        iVar2 = iVar2 + 1;
        iVar3 = iVar3 + uVar1 + iVar3 * g_PS2Rez_56_4_;
    }
    return iVar3;
}