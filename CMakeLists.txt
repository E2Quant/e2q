# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.8)
project(e2q)

add_definitions('-g')
add_definitions('-Wall')
add_definitions('-std=c++20')


# Set project version
set(E2Q_VERSION_MAJOR "0")
set(E2Q_VERSION_MINOR "1")
set(E2Q_VERSION_PATCH "0")
set(E2Q_VERSION_EXTRA "")
set(E2Q_VERSION "${E2Q_VERSION_MAJOR}.${E2Q_VERSION_MINOR}")
set(E2Q_VERSION_FULL "${E2Q_VERSION_MAJOR}.${E2Q_VERSION_MINOR}.${E2Q_VERSION_PATCH}${E2Q_VERSION_EXTRA}")

# number deci for expression
add_definitions(-DNUMBER_DECI=10000)

# DEBUG
option(DEBUG "build application with debug" OFF) # OFF is the default
if(DEBUG)
    # for code  #ifdef DEBUG ... #endif
    add_compile_definitions( DEBUG )
endif(DEBUG) 

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    )

file(GLOB SOURCES

    "${PROJECT_SOURCE_DIR}/src/Toolkit/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/ControlPack/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/FeedPack/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/FusionPack/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/libs/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/libs/bprinter/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/libs/kafka/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/libs/kafka/protocol/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/libs/DB/*.cpp"

    "${PROJECT_SOURCE_DIR}/src/PlunkPack/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/MessagePack/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/StrategyPack/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/E2LScript/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/TradePack/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/Coordinate/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/OMSPack/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/OMSPack/Quote/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/OMSPack/OrderBook/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/OMSPack/Matcher/*.cpp"

    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    )

# Kafka LOG
option(KAFKALOG "build application with kafka log" OFF) # OFF is the default
if(KAFKALOG)
    # for code  #ifdef KAFKALOG ... #endif
    add_compile_definitions( KAFKALOG )
endif(KAFKALOG) 

# Configure header
configure_file(
    ${PROJECT_SOURCE_DIR}/include/Toolkit/version.h.in
    ${PROJECT_SOURCE_DIR}/include/Toolkit/version.h
    )

EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

set(LIB_EXT "so")
if( ${ARCHITECTURE} STREQUAL "x86_64" )
    message( STATUS "X86 Architecture: ${ARCHITECTURE}" )
    set(LIB_EXT "so")
else()
    # 以后再优化吧
    message( STATUS "ARM Architecture: ${ARCHITECTURE}" )
    set(LIB_EXT "dylib")
endif()

#---------------------------
# librdkafka library
#---------------------------
if (DEFINED ENV{LIBRDKAFKA_INCLUDE_DIR})
    set(LIBRDKAFKA_INCLUDE_DIR $ENV{LIBRDKAFKA_INCLUDE_DIR})
else ()
    find_file(LIBRDKAFKA_HEADER
        NAMES rdkafka.h
        HINTS /usr/include/librdkafka /usr/local/include/librdkafka /opt/homebrew/include/librdkafka)

    cmake_path(GET LIBRDKAFKA_HEADER PARENT_PATH LIBRDKAFKA_INCLUDE_DIR)
    cmake_path(GET LIBRDKAFKA_INCLUDE_DIR PARENT_PATH LIBRDKAFKA_INCLUDE_DIR)
endif ()

if (DEFINED ENV{LIBRDKAFKA_LIBRARY_DIR})
    set(LIBRDKAFKA_LIBRARY_DIR $ENV{LIBRDKAFKA_LIBRARY_DIR})
else ()
    find_library(LIBRDKAFKA_LIB
        NAMES rdkafka
        HINTS /usr/lib /usr/local/lib)

    cmake_path(GET LIBRDKAFKA_LIB PARENT_PATH LIBRDKAFKA_LIBRARY_DIR)
endif ()

if (EXISTS "${LIBRDKAFKA_INCLUDE_DIR}/librdkafka/rdkafka.h")
    message(STATUS "librdkafka include directory: ${LIBRDKAFKA_INCLUDE_DIR}")
else ()
    message(FATAL_ERROR "Could not find headers for librdkafka!")
endif ()

if (EXISTS "${LIBRDKAFKA_LIBRARY_DIR}/librdkafka.a" OR EXISTS "${LIBRDKAFKA_LIBRARY_DIR}/librdkafka.${LIB_EXT}" OR EXISTS "${LIBRDKAFKA_LIBRARY_DIR}/rdkafka.lib" )
    message(STATUS "librdkafka library directory: ${LIBRDKAFKA_LIBRARY_DIR}")
else ()
    message(FATAL_ERROR "Could not find library for librdkafka! apt-get install -y librdkafka++1 librdkafka-dev librdkafka1 ")
endif ()

#---------------------------
# libquickfix library
#---------------------------
#find_package(quickfix CONFIG REQUIRED)
find_library(FIX_LIBRARY NAMES  libquickfix.${LIB_EXT} HINTS "/usr/lib")

if(NOT FIX_LIBRARY)
    message (FATAL_ERROR "Cannot find Quickfix")
else()
    message ("Looking Quickfix found")
endif()



#---------------------------
# thread library
#---------------------------
find_package(Threads REQUIRED)

if(Threads_FOUND)
    message ("Looking Threads found")
else()
    message (FATAL_ERROR "Cannot find Threads")
endif()


#---------------------------
# libe2 e2language library
#---------------------------
#set(CMAKE_PREFIX_PATH /usr/local/lib)
find_library(E2_LIBRARY NAMES e2 libe2.${LIB_EXT} HINTS "/usr/local/lib")

if(NOT E2_LIBRARY)
    message (FATAL_ERROR "Cannot find E2")
else()
    message("Looking E2 library")
endif()

message(${E2_LIBRARY})
#---------------------------
# CCACHE library
#---------------------------
option(USE_CCACHE "" OFF)
set(CCACHE_OPTIONS "" CACHE STRING "options for ccache")
# or cmake -DDEBUG=ON -DCMAKE_CXX_COMPILER_LAUNCHER=ccache  ../
if (USE_CCACHE)

    find_program(CCACHE_PROGRAM ccache)

    if(CCACHE_PROGRAM)
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")

        message("Looking ccache library")
    endif()
endif()

set(BIN2H_HEADER_FILE  ${PROJECT_SOURCE_DIR}/include/E2LScript/BIN2E_HEAD_FILE.hpp )
set(BIN2H_SOURCE_FILE  ${PROJECT_SOURCE_DIR}/src/E2LScript/BIN2E_HEAD_FILE.cpp )






#---------------------------
# PostgreSQL library
#---------------------------

find_package(PostgreSQL REQUIRED)
if(PostgreSQL_FOUND)

    message ("Looking PostgreSQL found")
else()
    message (FATAL_ERROR "Cannot find PostgreSQL: sudo apt-get install -y libpq-dev libpq5")
endif()
#---------------------------
# pkg-config library
#---------------------------
find_package(PkgConfig REQUIRED)
if(PkgConfig_FOUND)
    message ("Looking PkgConfig found")
else()
    message (FATAL_ERROR "Cannot find PkgConfig: sudo apt-get install pkg-config")
endif()

#---------------------------
# uuid library
#---------------------------
pkg_search_module(UUID REQUIRED uuid)
if(NOT UUID_LIBRARIES)
    message("error : apt install uuid uuid-dev !!!")
endif()


add_executable(${PROJECT_NAME} ${SOURCES})

#--------------------------
#  hub config
#--------------------------

get_cmake_property(HUB_VARS VARIABLES)
set(HUB_LIBS "")


message("incs ${PROJECT_SOURCE_DIR}/hub")
include_directories(${PROJECT_SOURCE_DIR}/hub)

foreach(HUB_VAR ${HUB_VARS})
    string(FIND "${HUB_VAR}" "HUB" hublib)

    if(${hublib} EQUAL 0)
        string(SUBSTRING "${HUB_VAR}" 4 -1 hub_lib)
        message("CMAKE_ARGS: ${HUB_VAR} ${hub_lib}") 

        set(HUB_LIBS ${HUB_LIBS} ${hub_lib})
       

        # file(GLOB HUB_SRC
        #     "${PROJECT_SOURCE_DIR}/hub/${hub_lib}/*.cpp"
        #     )

        set(CallHublibfun "PLUGIN_HEADER\n\n#include \"${hub_lib}/${hub_lib}.hpp\" \n")

        file(READ "${BIN2H_HEADER_FILE}" HUBLIB_HEADER)
        string(FIND "${HUBLIB_HEADER}"  ${hub_lib}  matchre_hub)
        if(${matchre_hub} EQUAL -1)
            message("No match found ${hub_lib} in header, and append code")

            string(REPLACE "PLUGIN_HEADER" "${CallHublibfun}" HUBLIB_HEADER "${HUBLIB_HEADER}")
            file(WRITE "${BIN2H_HEADER_FILE}" "${HUBLIB_HEADER}") 
        endif ()

        set(CallCPP "PLUGIN_FUN\n\n Call${hub_lib}(funList);")
        file(READ "${BIN2H_SOURCE_FILE}" HUBLIB_TEXT)
        string(FIND "${HUBLIB_TEXT}" ${hub_lib} matchres)

        if(${matchres} EQUAL -1)
            message("No match found ${hub_lib}, and append code")
            string(REPLACE "PLUGIN_FUN" "${CallCPP}" HUBLIB_TEXT "${HUBLIB_TEXT}")
            file(WRITE "${BIN2H_SOURCE_FILE}" "${HUBLIB_TEXT}") 

        endif ()

        add_subdirectory(${PROJECT_SOURCE_DIR}/hub/${hub_lib})
    endif()
    
endforeach()

target_include_directories(${PROJECT_NAME}
    PUBLIC 
    ${PROJECT_SOURCE_DIR}/include

    /usr/local/include/e2
    ${UUID_INCLUDE_DIRS}
    ${PostgreSQL_INCLUDE_DIRS}

    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC

    pthread

    ${E2_LIBRARY}
    ${UUID_LIBRARIES}
    ${FIX_LIBRARY}
    ${LIBRDKAFKA_LIB}
    ${PostgreSQL_LIBRARIES}
 
    rdkafka++

    ${HUB_LIBS}
    )
