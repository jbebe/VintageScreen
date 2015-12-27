#pragma once

// from C++11 STL
#include <functional>

#define GEN_GUID() __COUNTER__

template <int GUID, typename T>
struct Callback;

template <int GUID, typename Ret, typename... Params>
struct Callback<GUID, Ret(Params...)> {
	
	template <typename... Args> 
	static Ret callback(Args... args) {                    
		func(args...);  
	}
	
	static std::function<Ret(Params...)> func; 
};

template <int GUID, typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<GUID, Ret(Params...)>::func;