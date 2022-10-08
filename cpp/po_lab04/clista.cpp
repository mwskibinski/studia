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


// CLista
CLista::CLista()
	: head(nullptr)
{}

// Konstruktor kopiujący.
// By nie dzielić problemu na kilka przypadków (head == nullptr, head->next == nullptr, else)
// użyłem wskaźnika na wskaźnik (Node **). Kod wygląda bardziej "egzotycznie", ale dzięki temu
// jest także krótszy. Podobnie podszedłem do tego w kilku innych metodach.
CLista::CLista(const CLista & rhs) {
	Node ** ptr = &head;
	Node * rhs_cursor = rhs.head;

	while (rhs_cursor != nullptr) {
		*ptr = new Node;
		(*ptr)->data = rhs_cursor->data;
		ptr = &(*ptr)->next;
		rhs_cursor = rhs_cursor->next;
	}
	*ptr = nullptr;
}

// Konstrukcja przy pomocy wektora punktów.
CLista::CLista(const std::vector<Point> & vp) {
	Node ** ptr = &head;

	for (auto & x : vp) {
		*ptr = new Node;
		(*ptr)->data = x;
		ptr = &(*ptr)->next;
	}
	*ptr = nullptr;
}

CLista::~CLista() {
	Node * cursor = head, * tmp = nullptr;
	while (cursor != nullptr) {
		tmp = cursor->next;
		delete cursor;
		cursor = tmp;
	}
}

// Metoda push_back została przerobiona względem poprzedniej wersji.
// Teraz wykorzystuje wskaźnik Node **.
void CLista::push_back(const Point & p) {
	Node ** ptr = &head;
	while (*ptr != nullptr)
		ptr = &(*ptr)->next;
	*ptr = new Node {.data = p, .next = nullptr};
}

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

CLista::Node * CLista::find(const Point & p, Node * start) const {
	Node * cursor = start;
	while (cursor != nullptr && cursor->data != p)
		cursor = cursor->next;
	return cursor;
}

CLista::Node * CLista::find(const Point & p) const {
	return find(p, head);
}

CLista::Node * CLista::get_head() const {
	return head;
}

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

void CLista::remove_first(const Point & p) {
	remove_first(p, head);
}

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

// Konkatenacja list.
// Najpierw kursor idzie na koniec listy, następnie dochodzi do kopiowania.
void CLista::cat(const CLista & rhs) {
	Node ** ptr = &head;
	while (*ptr != nullptr)
		ptr = &(*ptr)->next;

	Node * rhs_cursor = rhs.head;
	while (rhs_cursor != nullptr) {
		*ptr = new Node;
		(*ptr)->data = rhs_cursor->data;
		ptr = &(*ptr)->next;
		rhs_cursor = rhs_cursor->next;
	}
	*ptr = nullptr;
}

// Tworzenie nowej lokalnej listy, na której koniec umieszczana jest "ta" lista
// odpowiednią ilość razy.
CLista CLista::mult(int x) const {
	CLista ret;
	while (x--)
		ret.cat(*this);
	return ret;
}

void CLista::print() const {
	std::cout << "--- list:\n";
	for (auto p = head; p != nullptr; p = p->next)
		std::cout << p->data << std::endl;
}

// Usunięcie ostatniego elementu
CLista & CLista::operator--() {
	pop_back();
	return *this;
}

// Usunięcie ostatniego elementu, operator przyrostkowy
CLista CLista::operator--(int) {
	CLista tmp(*this);
	operator--();
	return tmp;

}

// Konkatenacja, operatorem +=
CLista & CLista::operator+=(const CLista & rhs) {
	cat(rhs);
	return *this;
}

// Tworzenie lokalnego obiektu budowanego z "tego" i podanego przez argument.
CLista CLista::operator+(const CLista & rhs) const {
	CLista ret(*this);
	ret.cat(rhs);
	return ret;
}

// Użycie funkcji mult operatorem *.
CLista CLista::operator*(int x) const {
	return mult(x);
}

// Wydobycie odpowiedniego elementu z listy.
// (Może lepiej byłoby zwracać 'Node *' zamiast Point? Pozostałem przy Point.)
// Brak zabezpieczeń przed podaniem zbyt dużego indeksu (wyjściu poza listę)
// podobnie jak przy zwykłych tablicach.
Point & CLista::operator[](int idx) {
	Node * cursor = head;
	while (idx--)
		cursor = cursor->next;
	return cursor->data;
}

// Wersja dla obiektu niemodyfikowalnego.
const Point & CLista::operator[](int idx) const {
	Node * cursor = head;
	while (idx--)
		cursor = cursor->next;
	return cursor->data;
}

// Usunięcie całej listy.
CLista & CLista::operator++() {
	Node * cursor = head, * tmp = nullptr;
	head = nullptr;
	while (cursor != nullptr) {
		tmp = cursor->next;
		delete cursor;
		cursor = tmp;
	}
	return *this;
}

// Usunięcie całej listy, operator przyrostkowy.
CLista CLista::operator++(int) {
	CLista tmp(*this);
	operator++();
	return tmp;
}

// Przeciążenie operatora << do wyświetlania całej listy.
std::ostream & operator<<(std::ostream & os, const CLista & cl) {
	for (auto p = cl.head; p != nullptr; p = p->next)
		os << p->data << '\n';
	return os;
}
