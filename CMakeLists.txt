# Define the minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Set the project name and specify C++ standard
project(restAPI LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(cpprestsdk REQUIRED)

# Define compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wwrite-strings -Wsign-compare -Wextra -g")
include_directories(include /usr/include/mysql /usr/include/cppconn)

# Define source files[-Wwrite-strings]
file(GLOB SRC_FILES "src/*.cpp")

# Define the executable target
add_executable(${PROJECT_NAME} ${SRC_FILES})

option(USE_PODOFO "Use PoDoFo Library for PDF forms" ON)

if(USE_PODOFO)
    # Versuche PoDoFo über pkg-config zu finden
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PODOFO libpodofo)
    
    if(PODOFO_FOUND)
        target_compile_definitions(restAPI PRIVATE PODOFO_AVAILABLE)
        target_include_directories(restAPI PRIVATE ${PODOFO_INCLUDE_DIRS})
        target_link_libraries(restAPI PRIVATE ${PODOFO_LIBRARIES})
        target_link_directories(restAPI PRIVATE ${PODOFO_LIBRARY_DIRS})
        target_compile_options(restAPI PRIVATE ${PODOFO_CFLAGS_OTHER})
        
        message(STATUS "Using PoDoFo Library for PDF forms")
    else()
        # Alternative: Manuelle Suche nach PoDoFo
        find_path(PODOFO_INCLUDE_DIR
            NAMES podofo/podofo.h
            PATHS /usr/include
                  /usr/local/include
                  /opt/local/include)
        
        find_library(PODOFO_LIBRARY
            NAMES podofo
            PATHS /usr/lib
                  /usr/local/lib
                  /opt/local/lib)
        
        if(PODOFO_INCLUDE_DIR AND PODOFO_LIBRARY)
            target_compile_definitions(pdf_form_filler PRIVATE PODOFO_AVAILABLE)
            target_include_directories(pdf_form_filler PRIVATE ${PODOFO_INCLUDE_DIR})
            target_link_libraries(pdf_form_filler ${PODOFO_LIBRARY})
            
            # PoDoFo Abhängigkeiten
            find_package(PkgConfig REQUIRED)
            pkg_check_modules(FONTCONFIG fontconfig)
            pkg_check_modules(FREETYPE freetype2)
            
            if(FONTCONFIG_FOUND)
                target_link_libraries(pdf_form_filler ${FONTCONFIG_LIBRARIES})
                target_include_directories(pdf_form_filler PRIVATE ${FONTCONFIG_INCLUDE_DIRS})
            endif()
            
            if(FREETYPE_FOUND)
                target_link_libraries(pdf_form_filler ${FREETYPE_LIBRARIES})
                target_include_directories(pdf_form_filler PRIVATE ${FREETYPE_INCLUDE_DIRS})
            endif()
            
            # Weitere PoDoFo Abhängigkeiten
            find_package(JPEG REQUIRED)
            find_package(PNG REQUIRED)
            find_package(ZLIB REQUIRED)
            find_package(OpenSSL REQUIRED)
            
            target_link_libraries(pdf_form_filler 
                JPEG::JPEG 
                PNG::PNG 
                ZLIB::ZLIB 
                OpenSSL::SSL 
                OpenSSL::Crypto
            )
            
            message(STATUS "Using PoDoFo Library (manual find)")
        else()
            message(WARNING "PoDoFo Library not found")
            set(USE_PODOFO OFF)
        endif()
    endif()
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE mysqlcppconn ${Boost_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto curl cpprestsdk::cpprest )

# Set the output directories for build artifacts   / ${CMAKE_BINARY_DIR}
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

# Add a custom clean target
add_custom_target(app
    COMMAND ${CMAKE_COMMAND} -E rm -f $<TARGET_OBJECTS:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E rm -f ${PROJECT_NAME}
)
