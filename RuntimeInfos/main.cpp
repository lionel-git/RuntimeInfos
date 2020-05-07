#include <iostream>

extern int tester(void);
extern int tester2(void);

int main(int argc, char** argv)
{
#ifdef _DLL
	std::cout << "/MD used" << std::endl;
#else
	std::cout << "/MT used" << std::endl;
#endif

	//tester();
	tester2();
	std::cout << "Fin" << std::endl;
	std::cout << "Type key" << std::endl;
	int c = getchar();
}