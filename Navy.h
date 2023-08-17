#ifndef _NAVY_H_
#define _NAVY_H_
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include "Ship.h"
#define DECK 176	// исправная клетка-палуба
#define DAMAGE 'X'	// разрушенная клетка-палуба
#define MISS 'o'	// пустая клетка, в которую упал снаряд

typedef unsigned char Field[N][N];	// игровое поле
typedef std::map<Cell, int> ShipMap;	// словарь ассоциаций "клетка - индекс корабля"
enum CurrentState {Miss, Damage, Kill};	// результат попадания в цель

// Класс Space - информационное пространство для обмена информацией между игроками
struct Space
{
	static Cell u_fire;			// огонь от пользователя
	static Cell r_fire;			// огонь от робота (компьютера)
	static CurrentState u_state;	// состояние пользователя
	static CurrentState r_state;	// состояние робота
	static int step;
};

// Базовый класс Navy
class Navy : public Space
{
public:
	Navy();
	void AllocShip(int, int, std::string);		// разместить корабль
	void Show() const;							// показать поля ownField и enemyField
	int GetInt();								// ввод целого числа
	bool IsLive() { return (nLiveShip > 0); }	// мы ещё живы?
	Rect Shell(Rect) const;						// вернуть "оболочку" для заданного прямоугольника (сам прямоугольник плюс пограничные клетки)
	void AddToVetoSet(Rect);					// добавить клетки прямоугольника в множество vetoSet

protected:
	Ship ship[10];								// корабли флота
	Field ownField;								// моё игровое поле
	Field enemyField;							// игровое поле неприятеля
	ShipMap shipMap;							// словарь ассоциаций "клетка - индекс корабля"
	CellSet vetoSet;							// множество "запрещённых" клеток
	CellSet crushSet;							// множество "уничтоженных" клеток
	int nLiveShip;								// количество боеспособных кораблей
};

// Класс UserNavy
class UserNavy : public Navy
{
public:
	UserNavy() { Allocation(); }
	void Allocation();
	void FireOff();					// выстрел по неприятелю
	void ResultAnalys();			// анализ результатов выстрела
	void GetFire();					// "приём" огня противника
	void FillDeadZone(Rect r, Field&);	// заполнить пробелами програничные клетки для r
};

// Класс RobotNavy
class RobotNavy : public Navy
{
public:
	RobotNavy();
	void Allocation();
	void FireOff();					// выстрел по неприятелю
	void ResultAnalys();			// анализ результатов выстрела
	void GetFire();					// "приём" огня противника
private:
	bool isCrushContinue;			// предыдущий выстрел был успешным
	bool upEmpty;					// у поврежденного корабля противника нет "живых" клеток в верхнем направлении
};


#endif // !_NAVY_H_
