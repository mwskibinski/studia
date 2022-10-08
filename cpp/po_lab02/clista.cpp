#include "clista.h"
#include <iostream>


// Point
bool operator==(const Point & a, const Point & b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
bool operator!=(const Point & a, const Point & b) {
	return !(a == b);
}
std::ostream & operator<<(std::ostream & os, const Point & p) {
	os << p.x << ", " << p.y << ", " << p.z;
	return os;
}


// CList
CLista::CLista()
	: head(nullptr)
{}

// Destruktor: czyści całą listę.
CLista::~CLista() {
	Node * cursor = head, * tmp = nullptr;
	while (cursor != nullptr) {
		tmp = cursor->next;
		delete cursor;
		cursor = tmp;
	}
}

// push_back: umieszcza punkt na końcu listy.
// Metoda rozróżnia czy lista jest pusta czy nie. Jeśli nie jest, to szuka najpierw ostatniego elementu
void CLista::push_back(const Point & p) {
	if (head == nullptr) {
		head = new Node {.data = p, .next = nullptr};
	} else {
		Node * cursor = head;
		while (cursor->next != nullptr)
			cursor = cursor->next;

		cursor->next = new Node {.data = p, .next = nullptr};
	}
}

// pop_back: usuwa ostatni element na liście.
// Jeśli lista jest pusta, metoda kończy działanie.
// Jeśli jest tylko jeden element, należy usunąć dane pod wskaźnikiem head, a następnie go wyzerować,
// W przeciwnym wypadku kursor najpierw szuka końca listy.
void CLista::pop_back() {
	if (head == nullptr) {
		return;
	} else if (head->next == nullptr) {
		delete head;
		head = nullptr;
	} else {
		Node * cursor = head;
		while (cursor->next->next != nullptr)
			cursor = cursor->next;
		delete cursor->next;
		cursor->next = nullptr;
	}
}

// find: szuka elementu na liście i zwraca wskaźnik na odpowiedni węzeł.
// Pusta lista kończy działanie metody. Jeśli tak nie jest, funkcja zaczyna
// przeszukiwanie od węzła podanego jako drugi argument i zwraca poszukiwany węzeł lub wartość nullptr.
CLista::Node * CLista::find(const Point & p, Node * start) const {
	if (head == nullptr) {
		return nullptr;
	} else {
		Node * cursor = start;
		while (cursor != nullptr && cursor->data != p)
			cursor = cursor->next;
		return cursor;
	}
}

// Działanie takie samo jak metody powyżej, ale jako drugi argument podawany jest początek listy.
// Nie można tego zrealizować za pomocą mechanizmu domyślnych wartości argumentów.
CLista::Node * CLista::find(const Point & p) const {
	return find(p, head);
}

// get_head: zwraca wskaźnik na początek listy.
CLista::Node * CLista::get_head() const {
	return head;
}

// remove: usuwa węzeł wskazany przez argument.
void CLista::remove(Node * p) {
	if (head == nullptr) {
		return;
	} else if (head == p) {
		Node * tmp = head->next;
		delete head;
		head = tmp;
	} else {
		Node * prev = head;
		while (prev->next != nullptr && prev->next != p)
			prev = prev->next;
		if (prev->next == nullptr)
			return;
		prev->next = p->next;
	}
}

// remove_first: usuwa pierwsze wystąpienie wartości z listy.
// Lista pusta -- koniec.
// Jeśli wyszukiwanie należy zacząć od początku (start == head), i pierwszy element jest taki jak wskazany przez argument
// przypadek należy uwzględnić oddzielnie.
// W przeciwnym wypadku, kursor szuka elementu zaczynając od 'start'. Jeśli po wyszukiwaniu ma wartość nullptr - brak elementu na liście.
// Jeśli nie jest pusty, element jest usuwany.
void CLista::remove_first(const Point & p, Node * start) {
	if (head == nullptr) {
		return;
	} else if (start == head && head->data == p) {
		Node * tmp = head;
		head = head->next;
		delete tmp;
	} else {
		Node * cursor = start, * prev = nullptr;
		while (cursor != nullptr && cursor->data != p) {
			prev = cursor;
			cursor = cursor->next;
		}
		if (cursor == nullptr)
			return;
		prev->next = cursor->next;
		delete cursor;
	}
}

// Działanie tak jak powyżej, ale drugi argument jest ustawiany na początek listy.
// Podobnie jak przy find nie można tego wykonać poprzez domyślne wartości argumentów i trzeba przeciążyć funkcję.
void CLista::remove_first(const Point & p) {
	remove_first(p, head);
}

// remove_all: usuwa wszystkie elementy o wskazanej wartości z listy.
// Pierwsza pętla usuwa elementy występujące na samym początku listy.
// Jeśli po 1szej pętli lista nie jest pusta (cursor != nullptr) program przechodzi dalej do drugiej pętli,
// która zajmuje już się usuwanie w środku listy.
void CLista::remove_all(const Point & p) {
	Node * cursor = head;
	while (head != nullptr && head->data == p) {
		head = head->next;
		delete cursor;
		cursor = head;
	}
	if (cursor != nullptr) {
		Node * prev = cursor;
		cursor = cursor->next;
		while (cursor != nullptr) {
			if (cursor->data == p) {
				prev->next = cursor->next;
				delete cursor;
				cursor = prev->next;
			} else {
				prev = cursor;
				cursor = cursor->next;
			}
		}
	}
}

// print: pokazuje zawartość całej listy.
void CLista::print() const {
	std::cout << "--- list:\n";
	for (auto p = head; p != nullptr; p = p->next)
		std::cout << p->data << std::endl;
}

// insert: umieszcza element przed wskazanym przez drugi argument.
// Jeśli przed pierwszym elementem, należy to rozpatrzeć oddzielnie.
// Jeśli nie, kursor szuka pozycji przed węzłem 'before'.
// Jeśli go nie może znaleźć znaczy że podany został niepoprawny adres, i funkcja kończy działanie.
// Jeśli jest znaleziony to zostaje umieszczony nowy element na liście.
void CLista::insert(const Point & p, Node * before) {
	if (before == head) {
		head = new Node {.data = p, .next = before};
	} else {
		Node * cursor = head;
		while (cursor != nullptr && cursor->next != before)
			cursor = cursor->next;
		if (cursor->next != before)
			return;
		cursor->next = new Node {.data = p, .next = before};
	}
}
