#include "MyClass.h"

MyClass::MyClass() : num(INIT_VAL)
{

}

int MyClass::get() const
{
	return num;
}

void MyClass::set(int n)
{
	num = n;
}