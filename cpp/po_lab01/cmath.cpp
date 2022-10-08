#include "cmath.h"

CMath::CMath() : result(0.0) {}
CMath::~CMath() {}

double CMath::get_result() const {
	return result;
}
void CMath::add(double a, double b) {
	result = a + b;
}
void CMath::subtract(double a, double b) {
	result = a - b;
}
void CMath::multiply(double a, double b) {
	result = a * b;
}
void CMath::divide(double a, double b) {
	result = a / b;
}
