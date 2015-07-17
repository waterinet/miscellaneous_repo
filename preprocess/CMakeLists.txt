project(Preprocess)

include_directories("/home/linlt/repos/miscellaneous_repo/cpp_console")
	
set(SRC_LIST 
  Main.cc
  SrcFile.cc
  SrcLine.cc
  SrcIter.cc
  SrcCIter.cc)

add_executable(prep ${SRC_LIST})

