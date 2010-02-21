
file(GLOB yaml_sources Utils/yaml/[a-z]*.cpp)

# Create the extension library
ADD_LIBRARY(Extensions_PropertyTree
  Utils/PropertyTree.h
  Utils/PropertyTree.cpp
  ${yaml_sources}

)
