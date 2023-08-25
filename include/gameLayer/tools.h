/////////////////////////////////////////////
//tools.h
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////
#pragma once

///define FORCE_LOG
///to controll the behavour of the log or just 
///change from debug to releasee

///define ERRORS_ONLY
///to only display error messages

//todo define an assert

//#define FORCE_LOG

#include <iostream>
#include <Windows.h>
#include <sstream>

#ifdef _DEBUG
#define FORCE_LOG
#endif

#ifdef ERRORS_ONLY
#undef FORCE_LOG
#endif // ERRORS_ONLY

#ifdef FORCE_LOG
#define MAIN main()
#else
#define MAIN WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR lpCmdLine, int nCmdShow)
#endif

#ifdef FORCE_LOG
inline void llog()
{
	std::cout << "\n";
}

template<class F, class ...T>
inline void llog(F f, T ...args)
{
	std::cout << f << " ";
	llog(args...);
}
#else
template<class F, class ...T>
inline void llog(F f, T ...args)
{

}
#endif

///warning log
#ifdef FORCE_LOG
inline void wlog()
{
	std::cout << "\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

template<class F, class ...T>
inline void wlog(F f, T ...args)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	std::cout << f << " ";
	wlog(args...);
}
#else
template<class F, class ...T>
inline void wlog(F f, T ...args)
{

}
#endif

///important log
#ifdef FORCE_LOG
inline void ilog()
{
	std::cout << "\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

template<class F, class ...T>
inline void ilog(F f, T ...args)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
	std::cout << f << " ";
	ilog(args...);
}
#else
template<class F, class ...T>
inline void ilog(F f, T ...args)
{

}
#endif

///good log
#ifdef FORCE_LOG
inline void glog()
{
	std::cout << "\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

template<class F, class ...T>
inline void glog(F f, T ...args)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	std::cout << f << " ";
	glog(args...);
}
#else
template<class F, class ...T>
inline void glog(F f, T ...args)
{

}
#endif

///error log
#ifdef FORCE_LOG
inline void elog()
{
	std::cout << "\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

template<class F, class ...T>
inline void elog(F f, T ...args)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	std::cout << f << " ";
	elog(args...);
}
#else

#ifdef ERRORS_ONLY

inline void elog(std::stringstream &&stream)
{
	errorMessage(stream.str().c_str());
}

template<class F, class ...T>
inline void elog(std::stringstream &&stream, F &&f, T &&...args)
{
	stream << std::forward<F>(f) << " ";

	elog(std::move(stream), args...);
}

template<class F, class ...T>
inline void elog(F &&f, T &&...args)
{
	std::stringstream stream;

	stream << std::forward<F>(f) << " ";

	elog(std::move(stream), args...);
}


#else
template<class F, class ...T>
inline void elog(F f, T ...args)
{

}
#endif

#endif

