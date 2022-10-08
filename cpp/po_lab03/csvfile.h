#pragma once

#include "defines.h"
#include <fstream>
#include <vector>

// Klasa jako składowe przechowuje: tryb otwarcia pliku i strumień plikowy
// Dwa konstruktory, różniące się drugim argumentem (tryb otwarcia pliku)
// Wszystko według specyfikacji z instrukcji
class CsvFile {
private:
	std::ios_base::openmode mode;
	std::fstream file;
public:
	CsvFile(const char * path, std::ios_base::openmode mode);
	CsvFile(const char * path, char c);
	~CsvFile();

	FileError read(std::vector<Point> & vp);
	FileError read(Point & p, int idx);
	FileError write(const std::vector<Point> & vp);
};
