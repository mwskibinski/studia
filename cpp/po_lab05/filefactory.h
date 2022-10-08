#pragma once

#include "ifile.h"

// Klasa FileFactory, zawiera tylko metodę statyczną open_file
class FileFactory {
public:
	FileFactory() {}
	~FileFactory() {}

	static IFile * open_file(const std::string & path, std::ios_base::openmode mode);
	static IFile * open_file(const std::string & path, char c);
};
