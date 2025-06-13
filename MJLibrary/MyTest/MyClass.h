#pragma once
#define INIT_VAL 5
class MyClass
{
public:
	MyClass();
	~MyClass() {};
	int get() const;
	void set(int n);
private:
	int num;
};

