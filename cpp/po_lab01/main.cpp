#include <iostream>
#include <cmath>
#include "cmath.h"

using std::cout;
using std::cin;
using std::endl;

bool roots_quad_func(double a, double b, double c, double & x1, double & x2);

void zd2() {
	double a, b, c, r1, r2;
	a = b = c = r1 = r2 = 0.0;

	cout << "\nWprowadź parametry funkcji kwadratowej (a b c) <podaj literę, by zakończyć>:\n> ";
	while (cin >> a >> b >> c) {
		if (roots_quad_func(a, b, c, r1, r2)) {
			if (std::isnan(r1))
				cout << "Podana funkcja nie ma pierwiastków.\n";
			else
				cout << "Pierwiastki tej funkcji: " << r1 << ", " << r2 << endl;
		} else
			cout << "Błąd: wprowadzono funkcję liniową.\n";
		cout << "\nWprowadź parametry funkcji kwadratowej (a b c) <podaj literę, by zakończyć>:\n> ";
	}
}

void zd3() {
	double a, b;
	a = b = 0.0;
	char op = 0;
	CMath cm;

	cout << "\nWprowadź dwie liczby, następnie wybierz operację na nich <podaj literę, by zakończyć>.\n> ";
	while (cin >> a >> b) {
		cout << "Operacje: (+) dodawanie, (-) odejmowanie, (*) mnożenie, (/) dzielenie\n> ";
		cin >> op;
		switch (op) {
		case '+':
			cm.add(a, b);
			cout << "Wynik: " << cm.get_result() << '\n';
			break;
		case '-':
			cm.subtract(a, b);
			cout << "Wynik: " << cm.get_result() << '\n';
			break;
		case '*':
			cm.multiply(a, b);
			cout << "Wynik: " << cm.get_result() << '\n';
			break;
		case '/':
			cm.divide(a, b);
			cout << "Wynik: " << cm.get_result() << '\n';
			break;
		default:
			cout << "Brak wybranej opcji.\n";
			break;
		}
		cout << "\nWprowadź dwie liczby, następnie wybierz operację na nich <podaj literę, by zakończyć>.\n> ";
	}
}

int main() {
	cout << "Wybierz część programu.\n";
	cout << "\t(2) zadanie nr 2 -- Rozgrzewka, funkcja kwadratowa\n";
	cout << "\t(3) zadanie nr 3 -- Klasa CMath\n";

	cout << "> ";
	char choice = 0;
	cin >> choice;

	switch (choice) {
	case '2':
		zd2();
		break;
	case '3':
		zd3();
		break;
	default:
		cout << "Wprowadzono niepoprawną opcję. Kończę program.\n";
		break;
	}
	cout << "\nKoniec\n";

	return 0;
}

bool roots_quad_func(double a, double b, double c, double & x1, double & x2) {
	if (a == 0.0)
		return false;

	double delta = b*b - 4*a*c;
	x1 = (-b + std::sqrt(delta)) / (2*a);
	x2 = (-b - std::sqrt(delta)) / (2*a);
	return true;
}
