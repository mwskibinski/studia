#include "binaryfile.h"
#include "defines.h"
#include <fstream>

// Pierwszy konstruktor
BinaryFile::BinaryFile(const char * path, std::ios_base::openmode mode)
	: mode(mode), file(path, mode | std::ios_base::binary)
{}

// Drugi konstruktor: tryb otwarcia jest ustawiany według znaku 'c'
// W przypadku niepoprawnego znaku, może można byłoby wysłać informację o błędzie
// na np. std::cerr
BinaryFile::BinaryFile(const char * path, char c)
	: mode(std::ios_base::binary)
{
	switch (c) {
	case 'o': case 'O':
		mode |= std::ios_base::in;
		break;
	case 'z': case 'Z':
		mode |= std::ios_base::out;
		break;
	case 'n': case 'N':
		mode |= std::ios_base::app;
		break;
	default:
		break;
	}
	file.open(path, mode);
}

// Destruktor: zamknięcie pliku
BinaryFile::~BinaryFile() {
	file.close();
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
// Jeśli do odczytu dochodzi (pierwsze dwa ify) najpierw ustawiany jest
// kursor w odpowiednie miejsce (idx-1). Jeśli podany indeks jest zbyt duży
// funkcja zwraca błąd.
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
