#pragma once
#include <iostream>

struct Point { double x, y, z; };

// Przeciążyłem operatory ==, != by kod szukający danych
// znajdujących się w liście był bardziej zwięzły.
// Dodatkowo, przeciążyłem << by wyświetlanie danych było proste
bool operator==(const Point & a, const Point & b);
bool operator!=(const Point & a, const Point & b);
std::ostream & operator<<(std::ostream & os, const Point & p);

// Klasa CLIsta:
//	- push_back umieszcza element na końcu listy.
//	- pop_back usuwa ostatni element.
//	- get_head zwraca wskaźnik na pierwszy element listy.
//	- find szuka pierwszego wystąpienia danego punktu. Druga
//		wersja rozpoczyna wyszukiwanie od konkretnego punktu.
//	- remove usuwa węzeł wskazany przez argument.
//	- remove_first usuwa pierwsze wystąpienie danego punktu.
//		Podobnie jak z find, metoda jest przeciążana i może
//		także zaczynać wyszukiwanie od danego elementu listy.
//	- remove_all usuwa wszystkie elementy o podanej wartości.
//	- insert umieszcza punkt przed wskazanym przez wskaźnik.
//	- print wyświetla zawartość całej listy.
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

	void print() const;
};
