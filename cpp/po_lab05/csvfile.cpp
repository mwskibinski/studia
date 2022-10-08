#include "csvfile.h"
#include "defines.h"
#include <fstream>
#include <string>
#include <iostream>
#include <limits>

// Pierwszy konstruktor.
// Wywołanie kontruktora klasy bazowej.
CsvFile::CsvFile(const char * path, std::ios_base::openmode mode)
	: IFile(path, mode)
{
	file.open(path, mode);
}

// Drugi konstruktor.
// Wywołanie (drugiego) kontruktora klasy bazowej.
CsvFile::CsvFile(const char * path, char c)
	: IFile(path, c)
{
	file.open(path, mode);
}

// Destruktor: teraz plik zamyka klasa bazowa
CsvFile::~CsvFile() {
	// file.close();
}

// Odczyt całego pliku.
FileError CsvFile::read(std::vector<Point> & vp) {
	if (!file.is_open())
		return FileError::FileClosed;
	else if (!(mode & std::ios_base::in))
		return FileError::OperationNotPermitted;
	else {
		std::string line;
		while (getline(file, line)) {
			int comma1 = line.find(',');
			int comma2 = line.find(',', comma1+1);
			vp.push_back(Point(
				std::stod(line.substr(0, comma1)),
				std::stod(line.substr(comma1+1, comma2-(comma1+1))),
				std::stod(line.substr(comma2+1))
			));
		}

		return FileError::Success;
	}
}

// Odczyt jednego punktu.
FileError CsvFile::read(Point & p, int idx) {
	if (!file.is_open())
		return FileError::FileClosed;
	else if (!(mode & std::ios_base::in) || idx <= 0)
		return FileError::OperationNotPermitted;
	else {
		auto max = std::numeric_limits<std::streamsize>::max();
		for (int i = 0; i < idx-1 && file.ignore(max, '\n'); i++)
			;
		std::string line;
		getline(file, line);

		if (!file.eof()) {
			int comma1 = line.find(',');
			int comma2 = line.find(',', comma1+1);

			p.x = std::stod(line.substr(0, comma1));
			p.y = std::stod(line.substr(comma1+1, comma2-(comma1+1)));
			p.z = std::stod(line.substr(comma2+1));

			return FileError::Success;
		} else
			return FileError::OperationNotPermitted;
	}
}

// Zapis.
FileError CsvFile::write(const std::vector<Point> & vp) {
	if (!file.is_open())
		return FileError::FileClosed;
	else if (!(mode & (std::ios_base::out | std::ios_base::app)))
		return FileError::OperationNotPermitted;
	else {
		for (const auto p : vp) {
			file << std::to_string(p.x) << ',' <<
				std::to_string(p.y) << ',' <<
				std::to_string(p.z) << '\n';
		}

		return FileError::Success;
	}
}
