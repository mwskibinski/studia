#pragma once

#include <fstream>
#include <vector>
#include "defines.h"

// Abstrakcyjna klasa bazowa.
// Zawiera dane, które poprzednio były zawierane w klasach BinaryFile i CsvFile.
// Metody są czysto wirtualne -- klasy pochodne muszą je implementować po swojemu.
// Destruktor jest wirtualny, co zapewnia poprawne zwalnianie zasobów przy użyciu
// referencji/wskaźników na klasę bazową.
class IFile {
protected:
	std::ios_base::openmode mode;
	std::string path;
	std::fstream file;
public:
	IFile(const char * path, std::ios_base::openmode mode);
	IFile(const char * path, char c);
	virtual ~IFile();

	virtual FileError read(std::vector<Point> & vp) = 0;
	virtual FileError read(Point & p, int idx) = 0;
	virtual FileError write(const std::vector<Point> & vp) = 0;
};
