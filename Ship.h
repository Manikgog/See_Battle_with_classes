#ifndef _SHIP_H_
#define _SHIP_H_

#include <string>
#include <algorithm>
#include <iostream>
#include <set>
#include <map>
#include <string>

#define N 10		// размер поля для размещения флота (N * N клеток)

struct Cell;

typedef std::set<Cell> CellSet;		// множество клеток

// Клетка (ячейка) на игровом поле
struct Cell
{
	Cell(int _r = 0, int _c = 0) : row(_r), col(_c) {}
	bool InSet(const CellSet&) const;					// определяет принадлежность клетки множеству типа CellSet
	bool operator<(const Cell&) const;
	int row;
	int col;
};

// Прямоугольная область (размещение кораблей и их "оболочек")
struct Rect
{
	Rect() {}
	Rect(Cell _lt, Cell _rb) : lt(_lt), rb(_rb) { FillCset(); }
	void FillCset();											// наполнить cset клетками данной области
	bool Intersect(const CellSet& cs) const;					// определить наличие непустого пересечения прямоугольника с множеством cs
	Cell lt;
	Cell rb;
	CellSet cset;												// множество клеток, принадлежащих прямоугольнику
};

// Класс Ship (для представления корабля)
class Ship
{
	friend class UserNavy;
	friend class RobotNavy;
public:
	Ship() : nDeck(0), nLiveDeck(0) {}
	Ship(int, std::string, Rect);
protected:
	Rect place;							// координаты размещения
	std::string name;					// имя корабля
	int nDeck;							// количество палуб
	int nLiveDeck;						// количество неповреждённых палуб
};


#endif