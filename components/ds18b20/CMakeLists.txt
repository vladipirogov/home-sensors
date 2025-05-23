cmake_minimum_required(VERSION 3.5)

idf_component_register(
    SRCS "ds18b20.c"
    INCLUDE_DIRS "."
    REQUIRES driver
)

project(ds18b20)