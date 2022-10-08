#pragma once

#include <iostream>
#include <vector>

struct Point { double x, y, z; };

bool operator==(const Point & a, const Point & b);
bool operator!=(const Point & a, const Point & b);
std::ostream & operator<<(std::ostream & os, const Point & p);

// Wszystkie dodane składowe zachowują się według danych z instrukcji.
// Poprzednie komentarze (te z lab 2) zostały w większości usunięte.
class CLista {
public:
	struct Node {
		Point data;
		Node * next;
	};
private:
	Node * head;
public:
	CLista();
	CLista(const CLista & rhs);
	CLista(const std::vector<Point> & vp);
	~CLista();

	void push_back(const Point & p);
	void pop_back();
	Node * get_head() const;
	Node * find(const Point & p) const;
	Node * find(const Point & p, Node * start) const;
	void remove(Node * p);
	void remove_first(const Point & p);
	void remove_first(const Point & p, Node * start);
	void remove_all(const Point & p);
	void insert(const Point & p, Node * before);
	void cat(const CLista & rhs);
	CLista mult(int x) const;

	CLista & operator--();
	CLista operator--(int);
	CLista & operator+=(const CLista & rhs);
	CLista operator+(const CLista & rhs) const;
	CLista operator*(int x) const;
	Point & operator[](int idx);
	const Point & operator[](int idx) const;
	CLista & operator++();
	CLista operator++(int);

	void print() const;

	friend std::ostream & operator<<(std::ostream & os, const CLista & cl);
};
