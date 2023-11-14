CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(ClienServerEcn)

FIND_PACKAGE(Boost 1.40 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

FIND_PACKAGE(GTest REQUIRED)

SET(CMAKE_CXX_STANDARD 17)

ADD_EXECUTABLE(Server Server.cpp Common.hpp json.hpp Balance.h Core.cpp Order.h)
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(Client Client.cpp Common.hpp json.hpp)
TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(GTest Gtest.cpp Common.hpp json.hpp Balance.h Core.cpp Order.h)
TARGET_LINK_LIBRARIES(GTest PRIVATE Threads::Threads ${Boost_LIBRARIES} ${GTEST_LIBRARIES})
