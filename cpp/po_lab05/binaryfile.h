#pragma once

#include "ifile.h"
#include "defines.h"
#include <fstream>
#include <vector>

// Klasa "straciła" dane składowe -- teraz są przechowywane w klasie bazowej.
// Użyłem słowa "override" by jawnie zaznaczyć, że przeciążam wirtualne metody.
class BinaryFile : public IFile
{
public:
	BinaryFile(const char * path, std::ios_base::openmode mode);
	BinaryFile(const char * path, char c);
	~BinaryFile();

	FileError read(std::vector<Point> & vp) override;
	FileError read(Point & p, int idx) override;
	FileError write(const std::vector<Point> & vp) override;
};
