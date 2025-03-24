#include "example.h"

#include "Platform/FileServer/FileServer.h"
#include "Platform/Path.h"

#include "Function/Message/Message.h"

#include <iostream>

#include "Function/Event/Delegate.h"

#include "glm/glm.hpp"

using namespace TinyRenderer;
using namespace std;

int func(int a, int b)
{
	cout << a << b << endl;
	return a;
}

static float s_func(float a) {
	cout << a << endl;
	return a;
}

class A
{
public:
	char C_Method()
	{
		cout << "���Ǹ���Ա����" << endl;
		return 'A';
	}
	static void S_Method()
	{
		cout << "���Ǹ���̬����" << endl;
	}

	virtual int V_Method(char b)
	{
		cout << "���Ǹ�A���麯��" << endl;
		return 0;
	}
};

class B : public A
{
public:
	int V_Method(char b) override
	{
		cout << "���Ǹ�B���麯��" << endl;
		return 0;
	}
};

void delegate_test()
{
	//delegate_test();
	// ʹ��ί�����͵���ȫ�ֺ���
	Delegate<int(int, int)> d1 = Delegate<int(int, int)>::Bind(func);
	d1(1, 5);
	// ���þ�̬����
	Delegate<float(float)> d2 = Delegate<float(float)>::Bind(s_func);
	d2(1.2);
	// ����lambda���ʽ
	int i_c = 5;
	Delegate<void(int)> d3 = Delegate<void(int)>::Bind([i_c](int a) {
		cout << a + i_c << endl;
		});
	d3(100);

	// ���ó�Ա����
	A a;
	auto d4 = Delegate<char()>::Bind(&a, &A::C_Method);
	cout << d4() << endl;

}

void Example::Main()
{

}