
#include "Navy.h"

Cell Space::u_fire;
Cell Space::r_fire;
CurrentState Space::u_state = Miss;
CurrentState Space::r_state = Miss;
int Space::step;
// ������� gap(n) ���������� ������ �� n ��������
std::string gap(int n) { return std::string(n, ' '); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����� Navy
Navy::Navy() : nLiveShip(10)
{
	// ��������� ������� ���� �������� "�����"
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
	// ��������� �������� ����������� ������ �������
	// � ������ �������������� "�����������" ������ �������
	// � ���������� ������ vetoSet
	while (1)
	{
		lt.row = rand() % (N + 1 - nDeck);
		lt.col = rb.col = rand() % N;
		rb.row = lt.row + nDeck - 1;
		if (!Rect(lt, rb).Intersect(vetoSet)) break;
	}
	// ��������� ������ � ����� �������
	ship[indShip] = Ship(nDeck, name, Rect(lt, rb));

	// ������� ����� ������� �� ������� ���� (������ DECK)
	// ��������� ��������������� �������� � ������� ����������
	for(i = lt.row; i <= rb.row; i++)
		for (j = lt.col; j <= rb.col; j++)
		{
			ownField[i][j] = DECK;
			shipMap[Cell(i, j)] = indShip;
		}
	// ��������� � ��������� vetoSet ������ ������ �������
	// ������ � ������������ ��������
	AddToVetoSet(Shell(Rect(lt, rb)));
}

void Navy::Show() const
{
	char rowName[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
	std::string colName("1 2 3 4 5 6 7 8 9 10");
	int i, j;
	std::cout << "-------------------------\n";
	std::cout << gap(3) << "��� ����" << gap(18) << "���� ����������" << std::endl;
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
	std::cout << step << ". " << "��� �������:     ";
	step++;
}

int Navy::GetInt() {
	int value;
	while (true) {
		std::cin >> value;
		if ('\n' == std::cin.peek()) { std::cin.get(); break; }
		else {
			std::cout << "��������� ���� ������� (��������� ����� �����):" << std::endl;
			std::cin.clear();
			while (std::cin.get() != '\n') {};
			return value;
		}
	}
	return value;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����� UserNavy::Allocation()
void UserNavy::Allocation()
{
	srand((unsigned)time(NULL));
	AllocShip(0, 4, "��������� '�����'");
	AllocShip(1, 3, "������ '�������'");
	AllocShip(2, 3, "������ '�������'");
	AllocShip(3, 2, "������� '������'");
	AllocShip(4, 2, "������� '������'");
	AllocShip(5, 2, "������� '������������'");
	AllocShip(6, 1, "��������� '�������'");
	AllocShip(7, 1, "���������'������������'");
	AllocShip(8, 1, "��������� '������'");
	AllocShip(9, 1, "��������� '�������'");
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
	unsigned char rowName;						// ����������� ���� (A, B, ... J)
	int colName;								// ����������� ������� (1, 2, ... 10)
	int row;									// ������ ���� (0, 1, 2, ... 9)
	int col;									// ������ ������� (0, 1, 2, ... 9)

	bool success = false;
	while (!success)
	{
		std::cin >> rowName;
		row = capital_letter.find(rowName);
		if (-1 == row) row = small_letter.find(rowName);
		if (-1 == row) { std::cout << "������. ��������� ����.\n"; continue; }
		colName = GetInt();
		col = colName - 1;
		if ((col < 0) || (col > 9))
		{
			std::cout << "������. ��������� ����.\n"; continue;
		}
		success = true;
	}
	u_fire = Cell(row, col);
}

void UserNavy::ResultAnalys()
{
	// r_state - ��������� ������ � ����������� ��������
	// ������������ �� ������ u_fire
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
		// ��������� "����������" ���������
		FillDeadZone(kill, enemyField);
		crushSet.clear();
	}
}

void UserNavy::GetFire()
{
	// ������� ������ - �� ������ r_fire
	std::string capital_letter = "ABCDEFGHIJ";
	char rowName = capital_letter[r_fire.row];
	int colName = r_fire.col + 1;
	std::cout << "\n������� ����������: " << rowName << colName << std::endl;
	if (DECK == ownField[r_fire.row][r_fire.col])
	{
		std::cout << "*** ���� ���������! ***";
		ownField[r_fire.row][r_fire.col] = DAMAGE;
		u_state = Damage;
		// ������ �������, ����������� ������ r_fire
		int ind = shipMap[r_fire];
		ship[ind].nLiveDeck--;

		if (!ship[ind].nLiveDeck)
		{
			u_state = Kill;
			std::cout << gap(6) << "� ����! ����� " << ship[ind].name << " !!!";
			nLiveShip--;
			Rect kill = ship[ind].place;
			FillDeadZone(kill, ownField);
		}
	}
	else
	{
		u_state = Miss;
		std::cout << "*** M���! ***";
		ownField[r_fire.row][r_fire.col] = MISS;
	}
	std::cout << std::endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����� RobotNavy
RobotNavy::RobotNavy() {
	Allocation();
	isCrushContinue = false;
	upEmpty = false;
}

void RobotNavy::Allocation()
{
	AllocShip(0, 4, "��������� '��������'");
	AllocShip(1, 3, "������ '��������'");
	AllocShip(2, 3, "������ '������'");
	AllocShip(3, 2, "������� '������'");
	AllocShip(4, 2, "������� '��������'");
	AllocShip(5, 2, "������� '����'");
	AllocShip(6, 1, "��������� '������'");
	AllocShip(7, 1, "���������'�������'");
	AllocShip(8, 1, "��������� '���������'");
	AllocShip(9, 1, "��������� '������'");
	vetoSet.clear();
}

void RobotNavy::FireOff() {
	Cell c, cUp;
	if (!isCrushContinue) {
		// ��������� ����� ��������� �������� 
		while (1) {
			c.row = rand() % N;
			c.col = rand() % N;
			if (!c.InSet(vetoSet)) break;
		}
	}
	else {
		// "������" �� ����������� ��������� 
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
	// u_state - ��������� ������������ � ����������
	// �������� ������ �� ������ r_fire
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
	// ������� ������������ - �� ������ u_fire
	if (DECK == ownField[u_fire.row][u_fire.col])
	{
		std::cout << "*** ���� ���������! ***";
		r_state = Damage;
		// ������ �������, ����������� ������ u_fire
		int ind = shipMap[u_fire];
		ship[ind].nLiveDeck--;

		if (!ship[ind].nLiveDeck)
		{
			r_state = Kill;
			std::cout << "��������� " << ship[ind].name << " !!!";
			nLiveShip--;
		}
	}
	else
	{
		r_state = Miss;
		std::cout << "*** ����! ***";
	}
	std::cout << std::endl;
}


