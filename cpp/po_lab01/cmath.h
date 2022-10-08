#pragma once

class CMath {
private:
	double result;
public:
	CMath();
	~CMath();

	double get_result() const;
	void add(double, double);
	void subtract(double, double);
	void multiply(double, double);
	void divide(double, double);
};
