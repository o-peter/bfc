#ifndef bfc_parser_h
#define bfc_parser_h
#include <string>
#include <optional>

namespace parser
{
std::optional<std::string> code_to_asm(std::string& code);
} // namespace parser

#endif // bfc_parser_h
