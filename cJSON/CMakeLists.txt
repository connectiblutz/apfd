cmake_minimum_required(VERSION 3.13)


add_library(cJSON
  cJSON.c
  cJSON.h
)

target_include_directories(cJSON PUBLIC
  .
)
