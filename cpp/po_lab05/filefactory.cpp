#include "filefactory.h"
#include <string>
#include "csvfile.h"
#include "binaryfile.h"

// Metoda open_file.
// Sprawdzam 4 ostanie znaki, by wyznaczyć jakiego rodzaju plik jest otwierany.
// Jeśli nie zgadza się to z konwencją zadania, funkcja zwraca nullptr.
// Jako że tu używam 'string'a a konstruktory oczekują 'constr char *'
// używam metody c_str().
IFile * FileFactory::open_file(const std::string & path, std::ios_base::openmode mode) {
	if (path.substr(path.size() - 4) == ".csv")
		return new CsvFile(path.c_str(), mode);
	else if (path.substr(path.size() - 4) == ".bin")
		return new BinaryFile(path.c_str(), mode);
	else
		return nullptr;
}

// To samo, ale tryb podawany jako znak.
IFile * FileFactory::open_file(const std::string & path, char c) {
	if (path.substr(path.size() - 4) == ".csv")
		return new CsvFile(path.c_str(), c);
	else if (path.substr(path.size() - 4) == ".bin")
		return new BinaryFile(path.c_str(), c);
	else
		return nullptr;
}
