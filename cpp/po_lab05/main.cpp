#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>

#include "defines.h"
#include "binaryfile.h"
#include "csvfile.h"
#include "filefactory.h"

std::ostream & operator<<(std::ostream & os, const Point & p);

// Prezentacja to właściwie kod z lab nr 3 bez zmian.
// Jedyną zmianą jest użycie wskaźników i referencji na bazową klasę abstrakcyjną IFile
// zamiast obiektów klas pochodnych.
// Dla urozmaicenia kodu, w jednym bloku używam obiektów klasy unique_ptr.
int main() {
	using std::cout;
	using std::endl;
	using std::cin;
	using std::vector;

	// Odczyt całego pliku CSV i binarnego
	// Użycie wskaźnika na klasę bazową.
	{
		IFile * if_csv = FileFactory::open_file("test.csv", 'o');
		IFile * if_bin = FileFactory::open_file("test.bin", 'o');
		// Jakaś uboga kontrola czy można kontynuować działanie dalej.
		// Dalej to pomijam.
		if (if_csv == nullptr || if_bin == nullptr) {
			std::cerr << "Błąd: podano plik o nie obsługiwanym rozszerzeniu.\n"
				<< "Kończę działanie\n";
			return -1;
		}

		vector<Point> vp_csv, vp_bin;

		cout << R"(--- Odczyt pliku "test.csv" i "test.bin":)" << '\n';
		if (if_csv->read(vp_csv) == FileError::Success) {
			cout << "Wektor pliku CSV:\n";
			cout << "\tsize: " << vp_csv.size() << '\n';
			cout << "\tat(123): " << vp_csv.at(123) << '\n';
		} else
			cout << "CSV, odczyt całego pliku nieudany!\n";

		if (if_bin->read(vp_bin) == FileError::Success) {
			cout << "Wektor pliku binarnego:\n";
			cout << "\tsize: " << vp_bin.size() << '\n';
			cout << "\tat(123): " << vp_bin.at(123) << '\n';
		} else
			cout << "bin, odczyt całego pliku nieudany!\n";

		cout << '\n';

		delete if_csv;
		delete if_bin;
	}

	// Odczyt jednego punktu
	// Użycie referencji na klasę bazową.
	{
		IFile & if_csv = *(FileFactory::open_file("test.csv", 'o'));
		IFile & if_bin = *(FileFactory::open_file("test.bin", 'o'));
		Point p_csv, p_bin;
		int idx = 42069;

		cout << R"(--- Odczyt jednego punktu:)" << '\n';
		if (if_csv.read(p_csv, idx) == FileError::Success)
			cout << "\tCSV, Punkt o idx " << idx << ": " << p_csv << '\n';
		else
			cout << "\tCSV, Odczyt punktu o nr " << idx << " nieudany!\n";

		if (if_bin.read(p_bin, idx) == FileError::Success)
			cout << "\tbin, Punkt o idx " << idx << ": " << p_bin << '\n';
		else
			cout << "\tbin, Odczyt punktu o nr " << idx << " nieudany!\n";

		cout << '\n';

		delete &if_csv;
		delete &if_bin;
	}

	// Utworzenie nowego pliku, i dodanie kilku punktów
	// Użycie obiektu klasy unique_ptr.
	{
		std::unique_ptr<IFile> up_csv(FileFactory::open_file("out.csv", 'z'));
		std::unique_ptr<IFile> up_bin(FileFactory::open_file("out.bin", 'z'));
		vector<Point> v;

		std::srand(std::time(0));
		for (int i = 0; i < 3; i++)
			v.push_back(Point(std::rand(), std::rand(), std::rand()));

		cout << R"(--- Zapis do pliku "out.csv" i "out.bin":)" << '\n';
		if (up_csv->write(v) == FileError::Success)
			cout << "\tCSV, zapis wektora udany!\n";
		else
			cout << "\tCSV, zapis wektora nieudany!\n";

		if (up_bin->write(v) == FileError::Success)
			cout << "\tbin, zapis wektora udany!\n";
		else
			cout << "\tbin, zapis wektora nieudany!\n";

		cout << '\n';
	}

	// Nadpisanie danych
	{
		IFile & if_csv = *(FileFactory::open_file("out.csv", 'n'));
		IFile & if_bin = *(FileFactory::open_file("out.bin", 'n'));
		vector<Point> v {Point(-1, -2, -3)};

		cout << R"(--- Nadpisanie "out.csv" i "out.bin":)" << '\n';
		if (if_csv.write(v) == FileError::Success)
			cout << "\tCSV, nadpisanie danych udane!\n";
		else
			cout << "\tCSV, nadpisanie danych nieudane!\n";

		if (if_bin.write(v) == FileError::Success)
			cout << "\tbin, nadpisanie danych udane!\n";
		else
			cout << "\tbin, nadpisanie danych nieudane!\n";

		cout << '\n';

		delete &if_csv;
		delete &if_bin;
	}

	// Odczyt zapisanych danych
	{
		IFile * if_csv = FileFactory::open_file("out.csv", 'o');
		IFile * if_bin = FileFactory::open_file("out.bin", 'o');
		vector<Point> vp_csv, vp_bin;

		cout << R"(--- Odczyt "out.csv" i "out.bin":)" << '\n';
		if (if_csv->read(vp_csv) == FileError::Success) {
			cout << "Wektor pliku CSV:\n";
			for (auto x : vp_csv)
				cout << '\t' << x << '\n';
		} else
			cout << "\tCSV, odczyt całego pliku nieudany!\n";

		if (if_bin->read(vp_bin) == FileError::Success) {
			cout << "Wektor pliku bin:\n";
			for (auto x : vp_bin)
				cout << '\t' << x << '\n';
			cout << '\n';
		} else
			cout << "\tbin, odczyt całego pliku nieudany!\n";

		cout << '\n';

		delete if_csv;
		delete if_bin;
	}

	return 0;
}

// Ułatwienie odczytu punktu
std::ostream & operator<<(std::ostream & os, const Point & p) {
	os << p.x << ", " << p.y << ", " << p.z;
	return os;
}
