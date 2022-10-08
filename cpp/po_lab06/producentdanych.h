#pragma once

#include <thread>
#include <mutex>
#include <vector>

// Klasa producenta danych
class ProducentDanych {
private:
	std::thread thr;
	bool thread_is_running;
	std::vector<double> data;
	std::mutex mx;
public:
	ProducentDanych();
	~ProducentDanych();

	void start_thread();
	void stop_thread();
	std::vector<double> get_data_copy();
	
	friend void generate_data(ProducentDanych &);
};
