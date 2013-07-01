// Util.h
// 

#include <windows.h>
#include <utility>

template<typename T, typename R>
class CAutoFree
{
public:
	typedef R (__stdcall *FreeFunctionPtr)(T);
private:
	T ptr_;
	FreeFunctionPtr freeFunction_;
public:
	CAutoFree( T ptr, FreeFunctionPtr freeFunction)
		: ptr_(ptr), freeFunction_(freeFunction)
	{
	}

	~CAutoFree()
	{
		if(ptr_)
			freeFunction_(ptr_);
	}
};

// Check if current process has admin access
// pair.first  indicates elevated admin
// pair.second indicates non-elevated admin
std::pair<bool, bool> GetAdminStatus( HANDLE hProcess);
