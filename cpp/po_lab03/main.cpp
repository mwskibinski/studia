#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "defines.h"
#include "binaryfile.h"
#include "csvfile.h"

// Ułatwienie odczytu punktu
std::ostream & operator<<(std::ostream & os, const Point & p);

// Przy prezentacji działania klas zamykam fragmenty kodu w blokach {}
// W instrukcji nie podano jak radzić sobie z ustawianiem kursora w pliku z poziomu klasy
// przez co, po odczycie pliku obiekt jest do "wyrzucenia", bo ustawione zostały odpowiednie
// flagi std::fstream i z tego poziomu nie można ich zmienić. By się tym nie przejmować wszystko
// odbywa się na osobnych obiektach w osobnych blokach kodu.
int main() {
	using std::cout;
	using std::endl;
	using std::cin;
	using std::vector;

	// Odczyt całego pliku CSV i binarnego
	{
		CsvFile cf_in("test.csv", 'o');
		BinaryFile bf_in("test.bin", 'o');
		vector<Point> vp_c, vp_b;

		cout << R"(--- Odczyt pliku "test.csv" i "test.bin":)" << '\n';
		if (cf_in.read(vp_c) == FileError::Success) {
			cout << "Wektor pliku CSV:\n";
			cout << "\tsize: " << vp_c.size() << '\n';
			cout << "\tat(123): " << vp_c.at(123) << '\n';
		} else
			cout << "CSV, odczyt całego pliku nieudany!\n";

		if (bf_in.read(vp_b) == FileError::Success) {
			cout << "Wektor pliku binarnego:\n";
			cout << "\tsize: " << vp_b.size() << '\n';
			cout << "\tat(123): " << vp_b.at(123) << '\n';
		} else
			cout << "bin, odczyt całego pliku nieudany!\n";

		cout << '\n';
	}

	// Odczyt jednego punktu
	{
		CsvFile cf_in2("test.csv", 'o');
		BinaryFile bf_in2("test.bin", 'o');
		Point p_c, p_b;
		int idx = 42069;

		cout << R"(--- Odczyt jednego punktu:)" << '\n';
		if (cf_in2.read(p_c, idx) == FileError::Success)
			cout << "\tCSV, Punkt o idx " << idx << ": " << p_c << '\n';
		else
			cout << "\tCSV, Odczyt punktu o nr " << idx << " nieudany!\n";

		if (bf_in2.read(p_b, idx) == FileError::Success)
			cout << "\tbin, Punkt o idx " << idx << ": " << p_b << '\n';
		else
			cout << "\tbin, Odczyt punktu o nr " << idx << " nieudany!\n";

		cout << '\n';
	}

	// Utworzenie nowego pliku, i dodanie kilku punktów
	{
		CsvFile cf_out("out.csv", 'z');
		BinaryFile bf_out("out.bin", 'z');
		vector<Point> v;

		std::srand(std::time(0));
		for (int i = 0; i < 3; i++)
			v.push_back(Point(std::rand(), std::rand(), std::rand()));

		cout << R"(--- Zapis do pliku "out.csv" i "out.bin":)" << '\n';
		if (cf_out.write(v) == FileError::Success)
			cout << "\tCSV, zapis wektora udany!\n";
		else
			cout << "\tCSV, zapis wektora nieudany!\n";

		if (bf_out.write(v) == FileError::Success)
			cout << "\tbin, zapis wektora udany!\n";
		else
			cout << "\tbin, zapis wektora nieudany!\n";

		cout << '\n';
	}

	// Nadpisanie danych
	{
		CsvFile cf_app("out.csv", 'n');
		BinaryFile bf_app("out.bin", 'n');
		vector<Point> v {Point(-1, -2, -3)};

		cout << R"(--- Nadpisanie "out.csv" i "out.bin":)" << '\n';
		if (cf_app.write(v) == FileError::Success)
			cout << "\tCSV, nadpisanie danych udane!\n";
		else
			cout << "\tCSV, nadpisanie danych nieudane!\n";

		if (bf_app.write(v) == FileError::Success)
			cout << "\tbin, nadpisanie danych udane!\n";
		else
			cout << "\tbin, nadpisanie danych nieudane!\n";

		cout << '\n';
	}

	// Odczyt zapisanych danych
	{
		CsvFile cf_in("out.csv", 'o');
		BinaryFile bf_in("out.bin", 'o');
		vector<Point> vp_c, vp_b;

		cout << R"(--- Odczyt "out.csv" i "out.bin":)" << '\n';
		if (cf_in.read(vp_c) == FileError::Success) {
			cout << "Wektor pliku CSV:\n";
			for (auto x : vp_c)
				cout << '\t' << x << '\n';
		} else
			cout << "\tCSV, odczyt całego pliku nieudany!\n";

		if (bf_in.read(vp_b) == FileError::Success) {
			cout << "Wektor pliku bin:\n";
			for (auto x : vp_b)
				cout << '\t' << x << '\n';
			cout << '\n';
		} else
			cout << "\tbin, odczyt całego pliku nieudany!\n";

		cout << '\n';
	}

	return 0;
}

// Ułatwienie odczytu punktu
std::ostream & operator<<(std::ostream & os, const Point & p) {
	os << p.x << ", " << p.y << ", " << p.z;
	return os;
}
