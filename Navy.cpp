
#include "Navy.h"

Cell Space::u_fire;
Cell Space::r_fire;
CurrentState Space::u_state = Miss;
CurrentState Space::r_state = Miss;
int Space::step;
// Функция gap(n) возвращает строку из n пробелов
std::string gap(int n) { return std::string(n, ' '); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Класс Navy
Navy::Navy() : nLiveShip(10)
{
	// Заполняем игровые поля символом "точка"
	for(int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			ownField[i][j] = '.';
			enemyField[i][j] = '.';
		}
}

Rect Navy::Shell(Rect r) const
{
	Rect sh(r);
	sh.lt.row = (--sh.lt.row < 0) ? 0 : sh.lt.row;
	sh.lt.col = (--sh.lt.col < 0) ? 0 : sh.lt.col;
	sh.rb.row = (++sh.rb.row > (N - 1)) ? (N - 1) : sh.rb.row;
	sh.rb.col = (++sh.rb.col > (N - 1)) ? (N - 1) : sh.rb.col;
	return sh;
}

void Navy::AddToVetoSet(Rect r)
{
	for (int i = r.lt.row; i <= r.rb.row; i++)
		for (int j = r.lt.col; j <= r.rb.col; j++)
			vetoSet.insert(Cell(i, j));
}

void Navy::AllocShip(int indShip, int nDeck, std::string name)
{
	int i, j;
	Cell lt, rb;
	// Генерация случайно размещенной клетки корабля
	// с учётом недопустимости "пересечения" нового корабля
	// с множеством клеток vetoSet
	while (1)
	{
		lt.row = rand() % (N + 1 - nDeck);
		lt.col = rb.col = rand() % N;
		rb.row = lt.row + nDeck - 1;
		if (!Rect(lt, rb).Intersect(vetoSet)) break;
	}
	// Сохраняем данные о новом корабле
	ship[indShip] = Ship(nDeck, name, Rect(lt, rb));

	// Наносим новый корабля на игровое поле (символ DECK)
	// Добавляем соответствующие элементы в словарь ассоциаций
	for(i = lt.row; i <= rb.row; i++)
		for (j = lt.col; j <= rb.col; j++)
		{
			ownField[i][j] = DECK;
			shipMap[Cell(i, j)] = indShip;
		}
	// Добавляем в множество vetoSet клетки нового корабля
	// вместе с пограничными клетками
	AddToVetoSet(Shell(Rect(lt, rb)));
}

void Navy::Show() const
{
	char rowName[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
	std::string colName("1 2 3 4 5 6 7 8 9 10");
	int i, j;
	std::cout << "-------------------------\n";
	std::cout << gap(3) << "Мой флот" << gap(18) << "Флот неприятеля" << std::endl;
	std::cout << gap(3) << colName << gap(6) << colName << std::endl;

	for (i = 0; i < N; i++)
	{
		// Own
		std::string line = gap(1) + rowName[i];
		for (j = 0; j < N; j++)
			line += gap(1) + (char)ownField[i][j];
		// Enemy
		line += gap(5) + rowName[i];
		for (j = 0; j < N; j++)
			line += gap(1) + (char)enemyField[i][j];
		std::cout << line << std::endl;
	}
	std::cout << std::endl;
	std::cout << "==================================================================\n";
	std::cout << step << ". " << "Мой выстрел:     ";
	step++;
}

int Navy::GetInt() {
	int value;
	while (true) {
		std::cin >> value;
		if ('\n' == std::cin.peek()) { std::cin.get(); break; }
		else {
			std::cout << "Повторите ввод колонки (ожидается целое число):" << std::endl;
			std::cin.clear();
			while (std::cin.get() != '\n') {};
			return value;
		}
	}
	return value;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Класс UserNavy::Allocation()
void UserNavy::Allocation()
{
	srand((unsigned)time(NULL));
	AllocShip(0, 4, "Авианосец 'Варяг'");
	AllocShip(1, 3, "Линкор 'Муромец'");
	AllocShip(2, 3, "Линкор 'Никитич'");
	AllocShip(3, 2, "Крейсер 'Чудный'");
	AllocShip(4, 2, "Крейсер 'Добрый'");
	AllocShip(5, 2, "Крейсер 'Справедливый'");
	AllocShip(6, 1, "Миноносец 'Храбрый'");
	AllocShip(7, 1, "Миноносец'Неустрашимый'");
	AllocShip(8, 1, "Миноносец 'Смелый'");
	AllocShip(9, 1, "Миноносец 'Быстрый'");
	vetoSet.clear();
}

void UserNavy::FillDeadZone(Rect r, Field& field)
{
	int i, j;
	Rect sh = Shell(r);
	for (i = sh.lt.row, j = sh.lt.col; j <= sh.rb.col; j++)
		if (sh.lt.row < r.lt.row) field[i][j] = ' ';
	for (i = sh.rb.row, j = sh.lt.col; j <= sh.rb.col; j++)
		if (sh.rb.row > r.rb.row) field[i][j] = ' ';
	for (j = sh.lt.col, i = sh.lt.row; i <= sh.rb.row; i++)
		if (sh.lt.col < r.lt.col) field[i][j] = ' ';
	for (j = sh.rb.col, i = sh.lt.row; i <= sh.rb.row; i++)
		if (sh.rb.col > r.rb.col) field[i][j] = ' ';
}

void UserNavy::FireOff()
{
	std::string capital_letter = "ABCDEFGHIJ";
	std::string small_letter = "abcdefghij";
	unsigned char rowName;						// обозначение ряда (A, B, ... J)
	int colName;								// обозначение колонки (1, 2, ... 10)
	int row;									// индекс ряда (0, 1, 2, ... 9)
	int col;									// индекс колонки (0, 1, 2, ... 9)

	bool success = false;
	while (!success)
	{
		std::cin >> rowName;
		row = capital_letter.find(rowName);
		if (-1 == row) row = small_letter.find(rowName);
		if (-1 == row) { std::cout << "Ошибка. Повторите ввод.\n"; continue; }
		colName = GetInt();
		col = colName - 1;
		if ((col < 0) || (col > 9))
		{
			std::cout << "Ошибка. Повторите ввод.\n"; continue;
		}
		success = true;
	}
	u_fire = Cell(row, col);
}

void UserNavy::ResultAnalys()
{
	// r_state - сообщение робота о результатах выстрела
	// пользователя по клетке u_fire
	switch (r_state)
	{
	case Miss: enemyField[u_fire.row][u_fire.col] = MISS;
		break;
	case Damage:
		enemyField[u_fire.row][u_fire.col] = DAMAGE;
		crushSet.insert(u_fire);
		break;
	case Kill:
		enemyField[u_fire.row][u_fire.col] = DAMAGE;
		crushSet.insert(u_fire);
		Rect kill;
		kill.lt = *crushSet.begin();
		kill.rb = *(--crushSet.end());
		// заполняем "обрамление" пробелами
		FillDeadZone(kill, enemyField);
		crushSet.clear();
	}
}

void UserNavy::GetFire()
{
	// выстрел робота - по клетке r_fire
	std::string capital_letter = "ABCDEFGHIJ";
	char rowName = capital_letter[r_fire.row];
	int colName = r_fire.col + 1;
	std::cout << "\nВыстрел неприятеля: " << rowName << colName << std::endl;
	if (DECK == ownField[r_fire.row][r_fire.col])
	{
		std::cout << "*** Есть попадание! ***";
		ownField[r_fire.row][r_fire.col] = DAMAGE;
		u_state = Damage;
		// индекс корабля, занимающего клетку r_fire
		int ind = shipMap[r_fire];
		ship[ind].nLiveDeck--;

		if (!ship[ind].nLiveDeck)
		{
			u_state = Kill;
			std::cout << gap(6) << "О ужас! Погиб " << ship[ind].name << " !!!";
			nLiveShip--;
			Rect kill = ship[ind].place;
			FillDeadZone(kill, ownField);
		}
	}
	else
	{
		u_state = Miss;
		std::cout << "*** Mимо! ***";
		ownField[r_fire.row][r_fire.col] = MISS;
	}
	std::cout << std::endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Класс RobotNavy
RobotNavy::RobotNavy() {
	Allocation();
	isCrushContinue = false;
	upEmpty = false;
}

void RobotNavy::Allocation()
{
	AllocShip(0, 4, "Авианосец 'Алькаида'");
	AllocShip(1, 3, "Линкор 'БенЛаден'");
	AllocShip(2, 3, "Линкор 'Хусейн'");
	AllocShip(3, 2, "Крейсер 'Подлый'");
	AllocShip(4, 2, "Крейсер 'Коварный'");
	AllocShip(5, 2, "Крейсер 'Злой'");
	AllocShip(6, 1, "Миноносец 'Гадкий'");
	AllocShip(7, 1, "Миноносец'Мерзкий'");
	AllocShip(8, 1, "Миноносец 'Пакостный'");
	AllocShip(9, 1, "Миноносец 'Душный'");
	vetoSet.clear();
}

void RobotNavy::FireOff() {
	Cell c, cUp;
	if (!isCrushContinue) {
		// случайный выбор координат выстрела 
		while (1) {
			c.row = rand() % N;
			c.col = rand() % N;
			if (!c.InSet(vetoSet)) break;
		}
	}
	else {
		// "пляшем" от предыдущего попадания 
		c = cUp = r_fire;
		cUp.row--;
		if ((!upEmpty) && c.row && (!cUp.InSet(vetoSet)))
			c.row--;
		else
		{
			c = *(--crushSet.end());
			c.row++;
		}
	}
	r_fire = c;
	vetoSet.insert(r_fire);
}

void RobotNavy::ResultAnalys()
{
	// u_state - сообщение пользователя о результате
	// выстрела робота по клетке r_fire
	switch (u_state)
	{
	case Miss:
		if (isCrushContinue) upEmpty = true;
		break;
	case Kill:
		isCrushContinue = false;
		upEmpty = false;
		crushSet.insert(r_fire);
		Rect kill;
		kill.lt = *crushSet.begin();
		kill.rb = *(--crushSet.end());

		AddToVetoSet(Shell(kill));
		crushSet.clear();

	}
}

void RobotNavy::GetFire()
{
	// выстрел пользователя - по клетке u_fire
	if (DECK == ownField[u_fire.row][u_fire.col])
	{
		std::cout << "*** Есть попадание! ***";
		r_state = Damage;
		// индекс корабля, занимающего клетку u_fire
		int ind = shipMap[u_fire];
		ship[ind].nLiveDeck--;

		if (!ship[ind].nLiveDeck)
		{
			r_state = Kill;
			std::cout << "Уничтожен " << ship[ind].name << " !!!";
			nLiveShip--;
		}
	}
	else
	{
		r_state = Miss;
		std::cout << "*** Мимо! ***";
	}
	std::cout << std::endl;
}


