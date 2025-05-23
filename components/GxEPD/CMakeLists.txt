# GxEPD Library
# https://github.com/ZinggJM/GxEPD
# BSD License

cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE GxEPD_SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

idf_component_register(SRCS "GxEPD.cpp" 
                            "GxIO/GxIO_SPI/GxIO_SPI.cpp" 
                            "GxIO/GxIO.cpp" 
                            "GxGDEH0154D67/GxGDEH0154D67.cpp"
                            "GxFont_GFX.cpp"
                       INCLUDE_DIRS "." "GxIO" "GxIO/GxIO_SPI" "GxGDEH0154D67"
                       REQUIRES Adafruit-GFX-Library)

project(GxEPD)