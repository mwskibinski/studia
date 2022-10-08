#include <iostream>
#include "clista.h"
#include <ctime>
#include <cstdlib>
#include <vector>
#include <windows.h>
#include <algorithm>

void memory_leakage();
void vector_CLista_time_cmp();
double get_performance_counter();

int main() {
	// Wybrać (zakomentować/odkomentować) jedną funkcję.

	// memory_leakage();
	vector_CLista_time_cmp();

	return 0;
}

// Funkcja testująca wyciek pamięci.
// Na ten moment, do wycieku nie dochodzi. Jeśli chcemy zaobserwować wyciek należy zakomentować treść destruktora.
void memory_leakage() {
	using std::rand;
	std::srand(std::time(0));

	while (true) {
		CLista cl;
		int n = rand() % 10;
		for (int i = 0; i < n; i++)
			cl.push_back({rand() * 1.1, rand() * 0.9, rand() * 9.87});
	}
}

// Funkcja porównująca działanie wektora i klasy CLista
// Wykonane porównania:
// - push_back: umieszczenie elementu na końcu.
// - pop_back: usunięcie ostatniego elementu.
// - insert: umieszczenie elementu na początku.
// - find, remove: wyszukanie i usunięcie elementu (drugi element).
// - find, remove: wyszukanie i usunięcie elementu (element w środku).
// - find, remove: wyszukanie i usunięcie elementu (element trzeci od końca).
void vector_CLista_time_cmp() {
	using std::cout;

	double start = 0.0, finish = 0.0;
	std::vector<Point> vi;
	CLista cl;
	int lim = 10'000;

	// Umieszczanie elementu na końcu kolekcji.
	cout << "--- Metoda push_back:\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		vi.push_back({i * 1.2, i * -0.8, i * 4.3});
	}
	finish = get_performance_counter();
	cout << "\tvector: " << finish - start << " ms\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		cl.push_back({i * 1.2, i * -0.8, i * 4.3});
	}
	finish = get_performance_counter();
	cout << "\tCLista: " << finish - start << " ms\n";

	// Usuwanie elementu z końca.
	cout << "--- Metoda pop_back:\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		vi.pop_back();
	}
	finish = get_performance_counter();
	cout << "\tvector: " << finish - start << " ms\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		cl.pop_back();
	}
	finish = get_performance_counter();
	cout << "\tCLista: " << finish - start << " ms\n";

	// Umieszczanie elementu na samym początku kolekcji.
	cout << "--- Metoda insert (na początek):\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		vi.insert(vi.begin(), {i * 1.2, i * -0.8, i * 4.3});
	}
	finish = get_performance_counter();
	cout << "\tvector: " << finish - start << " ms\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		cl.insert({i * 1.2, i * -0.8, i * 4.3}, cl.get_head());
	}
	finish = get_performance_counter();
	cout << "\tCLista: " << finish - start << " ms\n";

	// Wyszukiwanie elementu drugiego, a następnie usunięcie go.
	cout << "--- Metody find (drugi element) i remove:\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		auto vi_it = std::find(vi.begin(), vi.end(), Point {(i+1) * 1.2, (i+1) * -0.8, (i+1) * 4.3});
		if (vi_it != vi.end())
			vi.erase(vi_it);
	}
	finish = get_performance_counter();
	cout << "\tvector: " << finish - start << " ms\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		auto cl_it = cl.find(Point {(i+1) * 1.2, (i+1) * -0.8, (i+1) * 4.3});
		if (cl_it != nullptr)
			cl.remove(cl_it);
	}
	finish = get_performance_counter();
	cout << "\tCLista: " << finish - start << " ms\n";

	// Wyszukiwanie elementu (w środku), a następnie usunięcie go
	cout << "--- Metody find (ok. środek) i remove:\n";

	// Ponowne wypełnienie kolekcji.
	for (int i = 0; i < lim; i++) {
		vi.push_back({i * 1.2, i * -0.8, i * 4.3});
	}
	for (int i = 0; i < lim; i++) {
		cl.push_back({i * 1.2, i * -0.8, i * 4.3});
	}

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		auto vi_it = std::find(vi.begin(), vi.end(), Point {(i + lim/2) * 1.2, (i + lim/2) * -0.8, (i + lim/2) * 4.3});
		if (vi_it != vi.end())
			vi.erase(vi_it);
	}
	finish = get_performance_counter();
	cout << "\tvector: " << finish - start << " ms\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		auto cl_it = cl.find(Point {(i + lim/2) * 1.2, (i + lim/2) * -0.8, (i + lim/2) * 4.3});
		if (cl_it != nullptr)
			cl.remove(cl_it);
	}
	finish = get_performance_counter();
	cout << "\tCLista: " << finish - start << " ms\n";

	// Wyszukiwanie elementu (ok. koniec), a następnie usunięcie go.
	cout << "--- Metody find (ok. koniec) i remove:\n";

	// Ponowne wypełnienie kolekcji.
	for (int i = 0; i < lim; i++) {
		vi.push_back({i * 1.2, i * -0.8, i * 4.3});
	}
	for (int i = 0; i < lim; i++) {
		cl.push_back({i * 1.2, i * -0.8, i * 4.3});
	}

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		auto vi_it = std::find(vi.begin(), vi.end(),
			Point {(lim - i - 3) * 1.2, (lim - i - 3) * -0.8, (lim - i - 3) * 4.3});
		if (vi_it != vi.end())
			vi.erase(vi_it);
	}
	finish = get_performance_counter();
	cout << "\tvector: " << finish - start << " ms\n";

	start = get_performance_counter();
	for (int i = 0; i < lim; i++) {
		auto cl_it = cl.find(Point {(lim - i - 3) * 1.2, (lim - i - 3) * -0.8, (lim - i - 3) * 4.3});
		if (cl_it != nullptr)
			cl.remove(cl_it);
	}
	finish = get_performance_counter();
	cout << "\tCLista: " << finish - start << " ms\n";
}

// Funkcja z kodu załączonego do ćwiczeń
double get_performance_counter() {
    LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	double freq = double(li.QuadPart) / 1000.0;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart) / freq;
}
