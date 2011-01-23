
file(GLOB yaml_sources Utils/yaml/[a-z]*.cpp)

# Create the extension library
ADD_LIBRARY(Extensions_PropertyTree
  Utils/PropertyTree.h
  Utils/PropertyTree.cpp
  Utils/PropertyTreeNode.h
  Utils/PropertyTreeNode.cpp
  ${yaml_sources}

)
