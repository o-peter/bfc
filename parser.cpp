#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/bind.hpp>
#include <cstddef>
#include <string>
#include <iostream>
#include <optional>
#include <stack>
#include "parser.hpp"

void increment_data_value(std::string& generated_code)
{
	static const std::string code = 
R"(
  ; increment_data_value
  inc byte [rbx]
)";
	generated_code += code;
}

void decrement_data_value(std::string& generated_code)
{
	static const std::string code = 
R"(
  ; decrement_data_value
  dec byte [rbx]
)";
	generated_code += code;
}

void increment_data_pointer(std::string& generated_code)
{
	static const std::string code = 
R"(
  ; increment_data_pointer
  inc rbx
)";	
	generated_code += code;
}

void decrement_data_pointer(std::string& generated_code)
{
	static const std::string code = 
R"(
  ; decrement_data_pointer
  dec rbx
)";	
	generated_code += code;
}

void output_byte(std::string& generated_code)
{
	static const std::string code = 
R"(
  ; output_byte
  mov rax, 1        ; syscall number for 'write'
  mov rdi, 1        ; rdi <- 1 (stdout file descriptor)
  mov rsi, rbx      ; rsi <- address of the buffer. 
  mov rdx, 1        ; size of buffer to write
  syscall           ; execute write(1, rbx, 1)
)";	
	generated_code += code;
}

void input_byte(std::string& generated_code)
{
	static const std::string code = 
R"(
  ; input_byte
  mov rax, 0        ; syscall number for 'read'
  mov rdi, 0        ; rdi <- 0 (stdin file descriptor)
  mov rsi, rbx      ; rsi <- address of the buffer. 
  mov rdx, 1        ; size of buffer to write
  syscall           ; execute read(0, rbx, 1)
)";	
	generated_code += code;
}

static int loop_no = 0;
static std::stack<int> loop_stack;
void loop_begin(std::string& generated_code)
{	
	loop_stack.push(++loop_no);
	std::string code = 
"loop_" + std::to_string(loop_stack.top()) +
R"(:
  ; loop_begin
  mov al, byte [rbx]
  test al, al
  jz loop_)" + std::to_string(loop_stack.top()) + R"(_end
)";
	generated_code += code;
}

void loop_end(std::string& generated_code)
{	
	std::string code = 
R"(
  ; loop_end
  mov al, byte [rbx]
  test al, al
  )" + 
  std::string("jnz loop_") + std::to_string(loop_stack.top()) + R"(
)" + std::string("loop_") + std::to_string(loop_stack.top()) + R"(_end:
)";
	
	loop_stack.pop();
	generated_code += code;
}




template <typename Iterator>
bool parse_tokens(Iterator first, Iterator last, std::string& generated_code)
{
	namespace qi = boost::spirit::qi;
	namespace phoenix = boost::phoenix;
	using qi::lit;
	using qi::_1;
	using qi::space_type;
	using qi::space;
	using qi::char_;
	using qi::lexeme;

	// all characters except the 8 valid tokens are ignored
	auto ignored = char_ - lit('>') - lit('<') - lit('+') - lit('-') -
			lit('.') - lit(',') - lit('[') - lit(']');
	bool result = qi::phrase_parse(first, last, 
		// begin grammar
		(
			+(
				lit('>')[boost::bind(&increment_data_pointer, ref(generated_code))]
				| lit('<')[boost::bind(&decrement_data_pointer, ref(generated_code))]
				| lit('+')[boost::bind(&increment_data_value, ref(generated_code))]
				| lit('-')[boost::bind(&decrement_data_value, ref(generated_code))]
				| lit('.')[boost::bind(&output_byte, ref(generated_code))]
				| lit(',')[boost::bind(&input_byte, ref(generated_code))]
				| lit('[')[boost::bind(&loop_begin, ref(generated_code))]
				| lit(']')[boost::bind(&loop_end, ref(generated_code))]
			)
		),
		// end grammar
		ignored);
	if (!result || first != last)
		return false;
	
	return true;
}

std::optional<std::string> parser::code_to_asm(std::string& code)
{
	std::string generated_code;
	if(parse_tokens(code.begin(), code.end(), generated_code))
		return generated_code;

	return std::nullopt;
}
