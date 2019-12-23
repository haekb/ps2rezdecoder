#include "Rez.h"
#include <windows.h>

Rez::Rez()
{
	m_Header = {};
	m_Blocks = {};
}

Rez::~Rez()
{
	m_Blocks.clear();
}

bool Rez::Open(std::string file)
{
	m_pStream.open(file, std::ios_base::binary);

	if (!m_pStream.is_open()) {
		return false;
	}

	// Read in the data
	m_pStream.read((char*)&m_Header, sizeof(m_Header));

	// Magic number, sorry!
	m_pStream.seekg(1312);

	// Read in the blocks
	for (int i = 0; i < m_Header.iTotalBlocks; i++) {
		BlockData block;

		m_pStream.read((char*)&block.padding, sizeof(block.padding));

		// Read in the files
		for (int j = 0; j < m_Header.iTotalFiles; j++) {
			FileData file;

			m_pStream.read((char*)&file, sizeof(file));

			block.Files.push_back(file);
		}

		m_Blocks.push_back(block);
	}

	return true;
}

bool Rez::Close()
{
	m_pStream.close();

	return true;
}

bool Rez::Extract(std::string filename, uint hash, int blockNumber)
{
	BlockData block = m_Blocks[blockNumber];
	for (auto file : block.Files) {
		if (file.iNameHash != hash) {
			continue;
		}

		// Handle the path jazz -- this is bad, but i just need it to work once!
		char drive[MAX_PATH];
		char dir[MAX_PATH];
		char name[MAX_PATH];
		char ext[MAX_PATH];
		_splitpath(filename.c_str(), drive, dir, name, ext);

		std::string outputDir = "out/";
		outputDir += dir;
		auto result = CreateDir((char*)outputDir.c_str());

		if (!result) {
			return false;
		}

		uint currentPos = 0;
		uint togo = file.iSize;
		uint inc = 2048;
		auto seek = file.iOffset * 2048;

		std::string outputFilename = "out/";
		outputFilename += filename;
		std::ofstream fileOut((char*)outputFilename.c_str(), std::ios_base::binary);

		if (!fileOut.is_open()) {

			return false;
		}

		m_pStream.seekg(seek);

		// Extract 2048 by 2048
		while (togo > 0) {
			if (togo < inc) {
				inc = togo;
			}

			char* buffer = new char[inc];

			m_pStream.read(buffer, inc);
			currentPos += m_pStream.gcount();

			fileOut.write(buffer, inc);


			togo -= inc;
		}
		
		fileOut.close();
		return true;
	}


	return false;
}

//
// From WinUtil of NOLF SDK
// 
BOOL DirExist(char* strPath)
{
	if (!strPath || !*strPath) return FALSE;

	BOOL bDirExists = FALSE;

	BOOL bRemovedBackSlash = FALSE;
	if (strPath[strlen(strPath) - 1] == '/')
	{
		strPath[strlen(strPath) - 1] = '\0';
		bRemovedBackSlash = TRUE;
	}

	UINT oldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
	struct stat statbuf;
	int error = stat(strPath, &statbuf);
	SetErrorMode(oldErrorMode);
	if (error != -1) bDirExists = TRUE;

	if (bRemovedBackSlash)
	{
		strPath[strlen(strPath)] = '/';
	}

	return bDirExists;
}

//
// From WinUtil of NOLF SDK
// 
BOOL CreateDir(char* strPath)
{
	if (DirExist(strPath)) return TRUE;
	if (strPath[strlen(strPath) - 1] == ':') return FALSE;		// special case

	char strPartialPath[MAX_PATH];
	strPartialPath[0] = '\0';

	char* token = strtok(strPath, "/");
	while (token)
	{
		strcat(strPartialPath, token);
		if (!DirExist(strPartialPath) && strPartialPath[strlen(strPartialPath) - 1] != ':')
		{
			if (!CreateDirectoryA(strPartialPath, NULL)) return FALSE;
		}
		strcat(strPartialPath, "/");
		token = strtok(NULL, "/");
	}

	return TRUE;
}
