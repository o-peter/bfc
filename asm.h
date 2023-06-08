static const char asm_scaffold[] = R"(
extern exit                  ; We declare these functions as extern to
extern malloc                ; let nasm know that they are somewhere else.
                             ; The linker will do the job of finding them.

section .text
global _start

_start:
  xor rax, rax
  mov rdi, 65536            ; allocate 65KiB
  call malloc
  mov [rax], dword 1        ; malloc returns the address of the allocated
                            ; memory in RAX. We store a int32 in the first
                            ; position

  mov rbx, rax              
  xor rax, rax              ; value to initialize to - zero
  lea rdi, [rbx]            ; starting location of buffer
  mov rcx, 8192             ; sizeof buffer to initialize
                            ; we initialize 65kib 8 bytes at a time
							; hence 8192

  cld                       ; let's move in forward direction
  rep stosq                 ; initialize rcx*8 bytes in [rdi],
  							; using one 8-byte value at [rax]
  mov rbx, rdi              ; just to make sure buffer pointer is in rbx
)";

static const char asm_teardown[] = R"(
  xor rdi, rdi              ; set exit code
  call exit                 ; 
)";