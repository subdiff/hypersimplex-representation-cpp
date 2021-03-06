cmake_minimum_required (VERSION 3.1.0)

project (hypersimplex-representer)
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
              Concurrent
              3DCore
              3DExtras
              3DRender
              3DInput
            )

set( ALL_SRCS
     main.cpp
     hypersimplex.cpp
     vertex.cpp
     autgroup.cpp
     gimatrix.cpp
     backend.cpp
     schlegel.cpp
     view3d/root3dwrapper.cpp
     view3d/root3dentity.cpp
     view3d/vertex3dentity.cpp
     view3d/edge3dentity.cpp
     )

qt5_add_resources( ALL_SRCS
                   resources.qrc
                   )

add_executable( hypersimplex-representer ${ALL_SRCS} )

target_link_libraries( hypersimplex-representer
                       Qt5::Core
                       Qt5::Gui
                       Qt5::Quick
                       Qt5::3DCore
                       Qt5::3DExtras
                       Qt5::3DRender
                       Qt5::3DInput
                       )
