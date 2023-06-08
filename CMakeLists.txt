cmake_minimum_required(VERSION 3.26.3)
project(brainfuck-compiler)

# find_package(keystone REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem locale)
# don't need to put asio here, as it's header-only

add_executable(brainfuck-compiler
	main.cpp
	parser.hpp 
	parser.cpp 
)
target_compile_features(brainfuck-compiler PUBLIC cxx_std_23)
target_include_directories(brainfuck-compiler PUBLIC
	${keystone_INCLUDE_DIRS} 
	${Boost_INCLUDE_DIRS}
)
target_link_libraries(brainfuck-compiler PRIVATE
	${keystone_LIBRARIES}  
	${Boost_LIBRARIES}
)
