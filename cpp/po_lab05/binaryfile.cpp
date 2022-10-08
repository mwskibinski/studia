#include "binaryfile.h"
#include "defines.h"
#include <fstream>

// Pierwszy konstruktor.
// Wywołanie kontruktora klasy bazowej.
BinaryFile::BinaryFile(const char * path, std::ios_base::openmode mode)
	: IFile(path, mode | std::ios_base::binary)
{
	file.open(path, mode);
}

// Drugi konstruktor.
// Wywołanie (drugiego) kontruktora klasy bazowej.
BinaryFile::BinaryFile(const char * path, char c)
	: IFile(path, c)
{
	mode |= std::ios_base::binary;
	file.open(path, mode);
}

// Destruktor: teraz plik zamyka klasa bazowa
BinaryFile::~BinaryFile() {
	// file.close();
}

// Odczyt całego pliku.
FileError BinaryFile::read(std::vector<Point> & vp) {
	if (!file.is_open())
		return FileError::FileClosed;
	else if (!(mode & std::ios_base::in))
		return FileError::OperationNotPermitted;
	else {
		Point p;
		while (file.read((char *) &p, sizeof p))
			vp.push_back(p);

		return FileError::Success;
	}
}

// Odczyt jednego punktu z pliku.
FileError BinaryFile::read(Point & p, int idx) {
	if (!file.is_open())
		return FileError::FileClosed;
	else if (!(mode & std::ios_base::in) || idx <= 0)
		return FileError::OperationNotPermitted;
	else {
		file.seekg((idx-1) * sizeof p);

		if (file.peek() != EOF) {
			file.read((char *) &p, sizeof p);
			return FileError::Success;
		} else
			return FileError::OperationNotPermitted;
	}
}

// Zapis.
FileError BinaryFile::write(const std::vector<Point> & vp) {
	if (!file.is_open())
		return FileError::FileClosed;
	else if (!(mode & (std::ios_base::out | std::ios_base::app)))
		return FileError::OperationNotPermitted;
	else {
		for (const auto p : vp)
			file.write((char *) &p, sizeof p);

		return FileError::Success;
	}
}
