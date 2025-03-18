#pragma once
#include <functional>

template<typename F>
class Delegate;

template<typename R, typename... Args>
class Delegate<R(Args...)>
{
public:
	Delegate() = default;

	// 绑定普通函数 静态成员函数
	static Delegate Bind(R(*func_ptr)(Args...))
	{
		Delegate d;
		d.m_func = func_ptr;
		return d;
	}

	// 绑定成员函数
	template<class C>
	static Delegate Bind(C* obj, R(C::*cfunc_ptr)(Args...))
	{
		Delegate d;
		d.m_func = [obj, cfunc_ptr](Args... args)->R {
			return ((obj->*cfunc_ptr)(std::forward<Args>(args)...));
		};
		return d;
	}

	template<typename F>
	static Delegate Bind(F&& func)
	{
		Delegate d;
		d.m_func = [func](Args... args)->R {
			return (func(std::forward<Args>(args)...));
		};
		return d;
	}

	R operator()(Args&&... args)
	{
		return m_func(std::forward<Args>(args)...);
	}

	// 判断是否绑定有效
	explicit operator bool() const {
		return static_cast<bool>(m_func);
	}
private:
	using FunctionType = std::function<R(Args...)>;
	FunctionType m_func;
};