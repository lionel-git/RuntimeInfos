#include <iostream>

extern int tester(void);
extern int tester2(void);

int main(int argc, char** argv)
{
	std::cout << "Type key" << std::endl;
	//tester();
	tester2();
	//int c = getchar();
	std::cout << "Fin" << std::endl;
}