#ifndef PRINTMEG_H_
#define PRINTMEG_H_

#include <iostream>

namespace PSG
{
inline void PrintMsg(std::string_view msg)
{
#ifndef NDEBUG
    std::cout << msg << std::endl;
#endif
}

inline void PrintMsg(std::string_view info, std::string_view msg)
{
#ifndef NDEBUG
    std::cout << info << ": " << msg << std::endl;
#endif
}

} // namespace PSG

#endif // !PRINTMEG_H_
