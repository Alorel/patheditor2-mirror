// Util.h
// 

#include <utility>

// Check if current process has admin access
// pair.first  indicates elevated admin
// pair.second indicates non-elevated admin
std::pair<bool, bool> GetAdminStatus( HANDLE hProcess);
