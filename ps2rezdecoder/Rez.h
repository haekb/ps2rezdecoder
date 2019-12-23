#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

typedef unsigned int uint;

int DirExist(char* strPath);
int CreateDir(char* strPath);

// Pascally-cased, because of habit. Sorry.
struct Header {
	uint unknown;
	uint iTotalFiles;
	uint iMagicNumber; // Maybe? It matched up with the value I brute forced, there's no other lithtech ps2 games to test...
	uint iDataOffset;
	uint iTotalBlocks;
};

struct FileData {
	uint iNameHash;
	uint iOffset;
	uint iSize;
	uint iXSize; // ?
};

struct BlockData {
	char padding[736];
	std::vector<FileData> Files;
};

//
// Thanks to https://zenhax.com/viewtopic.php?t=11139 and aluigi for his original extraction script!
//
class Rez
{
public:
	Rez();
	~Rez();

	bool Open(std::string file);
	bool Close();

	bool Extract(std::string filename, uint hash, int blockNumber = 0);

	uint GetMagicNumber() { return m_Header.iMagicNumber; }

protected:
	// Some useful structs
	Header m_Header;
	std::vector<BlockData> m_Blocks;

	std::ifstream m_pStream;
};

