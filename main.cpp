#include <iostream>
#include <Windows.h>
#include "Navy.h"


int main() {
	system("chcp 1251");
	system("cls");
	// ��������� �������
	UserNavy userNavy;
	RobotNavy robotNavy;
	userNavy.Show();

	while (userNavy.IsLive() && robotNavy.IsLive())
	{
		// ������� ������������
		if (Space::u_state != Miss)
		{
			std::cout << "������������...: <Enter>" << std::endl;
			std::cin.get();
		}
		else
		{
			userNavy.FireOff();
			robotNavy.GetFire();
			userNavy.ResultAnalys();
			if (!robotNavy.IsLive())
			{
				userNavy.Show();
				break;
			}
		}
		// ������� ������
		if (Space::r_state != Miss)
			std::cout << "\n������� ����������: ������������..." << std::endl;
		else
		{
			robotNavy.FireOff();
			userNavy.GetFire();
			robotNavy.ResultAnalys();
		}
		userNavy.Show();
	}
	if (userNavy.IsLive())
		std::cout << "\n:-))) ���! ������!!! :-)))" << std::endl;
	else
	{
		std::cout << "\n:-((( ���. ���������� �������� �������." << std::endl;
		std::cout << ":-((( �� ������, � ��������� ��� �� ��� �������!!!" << std::endl;
	}
	std::cin.get();
	return 0;
}
