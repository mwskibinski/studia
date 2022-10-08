#pragma once

#include "defines.h"
#include <fstream>
#include <vector>
#include "ifile.h"

// Klasa "straciła" dane składowe -- teraz są przechowywane w klasie bazowej.
// Użyłem słowa "override" by jawnie zaznaczyć, że przeciążam wirtualne metody.
class CsvFile : public IFile
{
public:
	CsvFile(const char * path, std::ios_base::openmode mode);
	CsvFile(const char * path, char c);
	~CsvFile();

	FileError read(std::vector<Point> & vp) override;
	FileError read(Point & p, int idx) override;
	FileError write(const std::vector<Point> & vp) override;
};
