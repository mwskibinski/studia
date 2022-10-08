#include "ifile.h"

// Pierwszy konstruktor.
// Jeszcze nie otwiera pliku, to będą robiły klasy pochodne.
// W przypadku klasy bazowej składowa 'mode' otrzyma dodatkowo ios_base::binary.
IFile::IFile(const char * path, std::ios_base::openmode mode)
	: mode(mode), path(path)
{}

// Drugi konstruktor.
IFile::IFile(const char * path, char c)
	: path(path)
{
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
	default:
		break;
	}
}

// Destruktor: zamknięcie pliku.
IFile::~IFile() {
	file.close();
}
