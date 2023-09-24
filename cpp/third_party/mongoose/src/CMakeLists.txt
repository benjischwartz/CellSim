add_library(mongoose mongoose.c)
target_include_directories(mongoose PUBLIC ${PROJECT_SOURCE_DIR}/third_party/mongoose/include)
