#include "csvfile.h"
#include "defines.h"
#include <fstream>
#include <string>
#include <iostream>
#include <limits>

// Konstruktor pierwszy
CsvFile::CsvFile(const char * path, std::ios_base::openmode mode)
	: mode(mode), file(path, mode)
{}

// Konstruktor drugi. Tryb otwarcia ustawiany według znaku 'c'
CsvFile::CsvFile(const char * path, char c) {
	switch (c) {
	case 'o': case 'O':
		mode = std::ios_base::in;
		break;
	case 'z': case 'Z':
		mode = std::ios_base::out;
		break;
	case 'n': case 'N':
		mode = std::ios_base::app;
		break;
	}
	file.open(path, mode);
}

// Destruktor: zamyka plik.
CsvFile::~CsvFile() {
	file.close();
}

// Odczyt całego pliku.
// W pętli odczytywana jest cała linia, a następnie zapisywana do 'line'.
// Znajdowane są pozycje dwóch przecinków, co pozwala odpowiednio podzielić łańcuch
// na 3 fragmenty. Te są zamieniane na double i tworzony jest punkt.
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
// W pierwszej kolejności ignorowana jest odpowiednia liczba linii.
// Następnie wykonywana jest próba odczytu -- jeśli się powiodła, znaczy że podany
// indeks nie jest zbyt duży, i można zmienić wartość punktu podanego przez referencję jako argument.
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

// Zapis. Składowe punktu są zamieniane na obiekty klasy string.
// Dane są odpowiednio formatowane i wysyłane do pliku.
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
