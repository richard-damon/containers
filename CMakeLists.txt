cmake_minimum_required(VERSION 3.30)
project(containers)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(containers
        AATreeIn.h
        AATreeIn.hpp
        BalTreeIn.h
        BalTreeIn.hpp
        Container.h
        DListIn.h
        DListIn.hpp
        ListIn.h
        ListIn.hpp
        ManyMany.h
        ManyMany.hpp
        SortDListIn.h
        SortDListIn.hpp
        SortListIn.h
        SortListIn.hpp
        TreeIn.h
        TreeIn.hpp)
