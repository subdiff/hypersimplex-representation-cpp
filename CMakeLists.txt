cmake_minimum_required (VERSION 3.1.0)

project (hypersimplex-representation)
#set(PROJECT_VERSION "1.0")
#set(PROJECT_VERSION_MAJOR 1)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set (CMAKE_CXX_STANDARD 11)

# Find Qt libraries
find_package( Qt5 REQUIRED
              Core
              Gui
              Qml
              Quick
            )

include_directories(/home/roman/Studium/Masterarbeit/Software/libgap/install/include)    #TODOX
link_directories(/home/roman/Studium/Masterarbeit/Software/libgap/install/lib)

# find_library(LIB_GAP gap REQUIRED)
find_library(LIB_GAP NAMES gap PATHS /home/roman/Studium/Masterarbeit/Software/libgap/install/lib REQUIRED)

MESSAGE( STATUS "variable is now: " ${LIB_GAP} ) # TODOX

# find_library(LIB_POLYMAKE polymake REQUIRED) # TODOX

# MESSAGE( STATUS "LIB_POLYMAKE variable is now: " ${LIB_POLYMAKE} ) # TODOX

set( ALL_SRCS
     main.cpp
     hypersimplex.cpp
     autgroup.cpp
     gimatrix.cpp
     backend.cpp
     )

qt5_add_resources( ALL_SRCS
                   resources.qrc
                   )

add_executable( hypersimplex-representation ${ALL_SRCS} )

target_link_libraries( hypersimplex-representation
                       ${LIB_GAP}
                       )

target_link_libraries( hypersimplex-representation
                       Qt5::Core
                       Qt5::Gui
                       Qt5::Quick
                       )
