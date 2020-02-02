#include <iostream>

#include "SomeClass.h"

int main()
{
	SomeNamespace::SomeClass					someClass;
	SomeNamespace::SomeClass::SomeNestedClass	someNestedClass;

	std::cout << someClass.get_someChar()				<< std::endl;
	std::cout << someClass.get_someFloat()				<< std::endl;
	std::cout << someClass.get_someInt()				<< std::endl;
	std::cout << someClass.get_someUnsignedLongLong()	<< std::endl;
	std::cout << someClass.get_someString()				<< std::endl;
	someClass.get_someNestedClass();

	someClass.set_someFloat(640.45f);
	someClass.set_someInt(46);
	someClass.set_someUnsignedLongLong(123u);
	someClass.set_someString("This is another test");
	someClass.set_someNestedClass(&someNestedClass);

	std::cout << someClass.get_someChar()				<< std::endl;
	std::cout << someClass.get_someFloat()				<< std::endl;
	std::cout << someClass.get_someInt()				<< std::endl;
	std::cout << someClass.get_someUnsignedLongLong()	<< std::endl;
	std::cout << someClass.get_someString()				<< std::endl;

	return EXIT_SUCCESS;
}