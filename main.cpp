#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <optional>
#include "parser.hpp"
#include "asm.h"


std::optional<std::string> readFile(std::filesystem::path path)
{
    namespace fs = std::filesystem;
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if(!f)
      return std::nullopt;
    
    const auto sz = fs::file_size(path);
    std::string result(sz, '\0');
    f.read(result.data(), sz);
    return result;
}

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    std::cout << "No input files\n";
    return 1;
  }
  auto input_file = readFile(argv[1]);
  if(!input_file)
  {
    std::cout << "Input file doesn't exist\n";
    return 1;
  }
  
  auto input_file_contents = std::move(input_file.value());
  auto generated_assembly = parser::code_to_asm(input_file_contents);
  if(!generated_assembly)
  {
    std::cout << "Invalid syntax\n";
    return 1;
  }

  std::string assembly;
  assembly += std::string(asm_scaffold);
  assembly += std::move(generated_assembly.value());
  assembly += std::string(asm_teardown);

	std::cout << assembly << '\n';
	return 0;
}
