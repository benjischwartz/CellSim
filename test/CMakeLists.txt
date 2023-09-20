add_executable(loadlibcellml loadlibcellml.cpp)
target_link_libraries(loadlibcellml ${LIBCELLML_LIBS})
add_test(loadlibcellml_test loadlibcellml)
