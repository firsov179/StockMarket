cmake_minimum_required(VERSION 3.5)
PROJECT(ClienServerEcn)

FIND_PACKAGE(Boost 1.40 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

FIND_PACKAGE(GTest REQUIRED)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Charts REQUIRED)

ADD_EXECUTABLE(Gui gui.cpp stockmarket.cpp stockmarket.h stockmarket.ui json.hpp Common.hpp ClientUnion.h img.qrc)
TARGET_LINK_LIBRARIES(Gui Qt::Core Qt::Gui Qt::Widgets Qt::Charts)

ADD_EXECUTABLE(Server Server.cpp Common.hpp json.hpp Balance.h Core.cpp Order.h)
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(Client Client.cpp Common.hpp json.hpp ClientUnion.h)
TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(GTest Gtest.cpp Common.hpp json.hpp Balance.h Core.cpp Order.h)
TARGET_LINK_LIBRARIES(GTest PRIVATE Threads::Threads ${Boost_LIBRARIES} ${GTEST_LIBRARIES})
