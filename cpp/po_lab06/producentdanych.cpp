#include "producentdanych.h"

// Konstruktor
ProducentDanych::ProducentDanych()
	: thread_is_running(false)
{}

// Destruktor -- kończy wątek
ProducentDanych::~ProducentDanych() {
	stop_thread();
}

// Rozpoczyna wątek, pod warunkiem, że już nie został rozpoczęty
void ProducentDanych::start_thread() {
	if (!thread_is_running) {
		void generate_data(ProducentDanych &);
		thread_is_running = true;
		thr = std::thread(generate_data, std::ref(*this));
	}
}

// Kończy wątek, pod warunkiem, że jeszcze działa
void ProducentDanych::stop_thread() {
	if (thread_is_running) {
		thread_is_running = false;
		thr.join();
	}
}

// Kopia aktualnych danych. Funkcja zwraca wektor przez wartość
// by otrzymać obraz danych w chwili wywołania metody.
std::vector<double> ProducentDanych::get_data_copy() {
	mx.lock();
	auto copy = data;
	mx.unlock();
	return copy;
}
