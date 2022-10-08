#include <iostream>
// Program został napisany na Linuxie, dlatego do obsługi niebuforowanego
// wejścia używam nagłówka <ncurses.h> i odpowiednich funkcji
// takich jak: initscr(), noecho(), getch(), clear(), addstr(), endwin()
#include <ncurses.h>
#include <chrono>
#include <random>
#include <functional>
#include "producentdanych.h"

// Do obsługi literałów czasowych, np. 1s (1 sekunda)
using namespace std::chrono_literals;

int main() {
	// Inicjacja okna
	initscr();
	noecho();

	const char ENTER_KEY = '\n';
	const char ESC_KEY = 27;

	double calc_avg(const std::vector<double> & vd);
	ProducentDanych pd;
	pd.start_thread();

	while (true) {
		char key = getch();

		// Nie używam switch, by móc breakować pętlę główną
		if (key == ENTER_KEY) {
			auto avg = calc_avg(pd.get_data_copy());
			clear();
			addstr("--- --- ---\n");
			addstr("average: ");
			addstr(std::to_string(avg).c_str());
			addstr("\n");
		} else if (key == ESC_KEY)
			break;
	}

	pd.stop_thread();

	endwin();

	return 0;
}

// Najpierw tworzy generator liczb pseudolosowych (rozkład równomierny
// z zakresu [5.0, 50.0]), korzysta z domyślnego silnika. Funkcja bind,
// łączy to w jeden obiekt funkcyjny, którego możemy używać jak ze
// starej dobrej funkcji rand()
void generate_data(ProducentDanych & pd) {
	auto get_rand_val = std::bind(
		std::uniform_real_distribution<>(5.0, 50.0),
		std::default_random_engine()
	);

	while (pd.thread_is_running) {
		std::this_thread::sleep_for(100ms);
		pd.mx.lock();
		pd.data.push_back(get_rand_val());
		pd.mx.unlock();
	}
}

// Liczenie średniej z wektora liczb double
double calc_avg(const std::vector<double> & vd) {
	double sum = 0.0;
	for (const auto & x : vd)
		sum += x;
	return sum / vd.size();
}
