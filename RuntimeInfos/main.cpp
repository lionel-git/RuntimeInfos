#include <iostream>

extern int tester(void);
extern int tester2(void);
extern void check_runtime();

int main(int argc, char** argv)
{
	check_runtime(); return 1;

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