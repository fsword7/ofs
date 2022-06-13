// debug.h - Debug tools package
//
// Author:  Tim Stark
// Date:    Jun 13, 2022

#pragma once

template <typename T>
inline void displayMatrix4(cstr_t &name, Eigen::Matrix<T, 4, 4> m)
{
    Logger::getLogger()->debug("{} contents:\n", name);
    Logger::getLogger()->debug("{:.6f} {:6f} {:6f} {:6f}\n",
        m(0, 0), m(0, 1), m(0, 2), m(0, 3));
    Logger::getLogger()->debug("{:.6f} {:6f} {:6f} {:6f}\n",
        m(1, 0), m(1, 1), m(1, 2), m(1, 3));
    Logger::getLogger()->debug("{:.6f} {:6f} {:6f} {:6f}\n",
        m(2, 0), m(2, 1), m(2, 2), m(2, 3));
    Logger::getLogger()->debug("{:.6f} {:6f} {:6f} {:6f}\n",
        m(3, 0), m(3, 1), m(3, 2), m(3, 3));
}
