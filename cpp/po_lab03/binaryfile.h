#pragma once

#include "defines.h"
#include <fstream>
#include <vector>

// Klasa jako składowe przechowuje: tryb otwarcia pliku i strumień plikowy
// Dwa konstruktory, różniące się drugim argumentem (tryb otwarcia pliku)
// Wszystko według specyfikacji z instrukcji
class BinaryFile {
private:
	std::ios_base::openmode mode;
	std::fstream file;
public:
	BinaryFile(const char * path, std::ios_base::openmode mode);
	BinaryFile(const char * path, char c);
	~BinaryFile();

	FileError read(std::vector<Point> & vp);
	FileError read(Point & p, int idx);
	FileError write(const std::vector<Point> & vp);
};
