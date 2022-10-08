#include <iostream>
#include "clista.h"

// Funkcja main zawiera krótką prezentację możliwości
// płynących z przeciążonych operatorów.
int main() {
	using std::cout;
	using std::endl;
	using std::vector;

	cout << "--- Krótka prezentacja.\n\n"
		<< "Tworzymy przykładowy obiekt CLista cl.\n\n";

	CLista cl;
	cl.push_back({1, 2, 3});
	cl.push_back({4, 5, 6});
	cl.push_back({7, 8, 9});

	cout << "Użycie operatora << do wyświetlenia zawartości.\n";
	cout << "cout << cl << endl;\n";
	cout << cl << endl;

	cout << "Użycie konstruktora kopiującego.\n"
		<< "CLista cl_cp(cl);\n";
	CLista cl_cp(cl);

	cout << "cout << cl_cp << endl;\n";
	cout << cl_cp << endl;

	cout << "Użycie wektora do konstrukcji listy.\n";
	vector<Point> vp {
		{9.11, 69, 420}, {2.1, 3.7, 0}
	};

	cout << "Wektor vp:\n";
	for (auto x : vp)
		cout << x << endl;
	cout << '\n';

	cout << "CLista cl_v(vp);\n";
	CLista cl_v(vp);

	cout << "cout << cl_v << endl;\n";
	cout << cl_v << endl;

	cout << "Użycie operatora +=.\n";
	cout << "cl += cl_v;\n";
	cl += cl_v;
	cout << "cout << cl << endl;\n";
	cout << cl << endl;

	cout << "Użycie operatora [].\n";
	cout << "cout << cl[2] << endl;\n";
	cout << cl[2] << endl;
	cout << '\n';

	cout << "Operator -- przedrostkowy (usunięcie ostatniego elementu).\n";
	cout << "cout << --cl << endl;\n";
	cout << --cl << endl;

	cout << "Operator -- przyrostkowy (usunięcie ostatniego elementu).\n";
	cout << "cout << cl-- << endl;\n";
	cout << cl--;
	cout << "cout << cl << endl;\n";
	cout << cl << endl;

	cout << "Użycie operatora *.\n";
	cout << "cout << cl * 3 << endl;\n";
	cout << cl * 3 << endl;

	cout << "Użycie operatora +.\n";
	cout << "cout << cl + cl_v << endl;\n";
	cout << cl + cl_v << endl;

	cout << "Operator ++ (usunięcie wszystkich elementów).\n";
	cout << "cout << ++cl << endl;\n";
	cout << ++cl << endl;
	cout << "Pustka...\n";

	cout << "\n--- Koniec.\n";

	return 0;
}
