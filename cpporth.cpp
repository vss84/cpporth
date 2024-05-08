#include <cassert>
#include <stack>
#include <vector>
#include <fstream>
#include <charconv>
#include <iostream>


enum Opcode
{
	OP_PUSH,
	OP_PLUS,
	OP_MINUS,
	OP_DUMP,
	COUNT_OPS
};

struct Operation
{
	Opcode opcode;
	int value;
};

Operation push(int x)
{
	return { OP_PUSH, x };
}
Operation plus()
{
	return { OP_PLUS };
}
Operation minus()
{
	return { OP_MINUS };
}
Operation dump()
{
	return { OP_DUMP };
}

void simulate_program(std::vector<Operation> program)
{
	std::stack<int> Stack;
	auto size = program.size();
	for (int i = 0; i < size; i++)
	{
		assert(COUNT_OPS == 4 && "Exhaustive op handling in simulate_program");
		switch (program[i].opcode)
		{
			case OP_PUSH:
			{
				Stack.push(program[i].value);
				break;
			}
			case OP_PLUS:
			{
				int a = Stack.top();
				Stack.pop();
				int b = Stack.top();
				Stack.pop();
				Stack.push(a + b);
				break;
			}
			case OP_MINUS:
			{
				int a = Stack.top();
				Stack.pop();
				int b = Stack.top();
				Stack.pop();
				Stack.push(b - a);
				break;
			}
			case OP_DUMP:
			{
				int a = Stack.top();
				Stack.pop();	
				printf("%d\n", a);
				break;
			}
			default:
				assert(false && "unreachable\n");
				break;
		}
	}
}

void compile_program(std::vector<Operation> program, std::string out_file_path)
{
	std::fstream output; 
	auto size = program.size();
	output.open("output.asm", std::ios::out);
	output << "extern _write: PROC"												<< "\n";
	output << ".data"															<< "\n";
	output << ".code"															<< "\n";
	output << "buffer$ EQU 32"													<< "\n";
	output << "x$ EQU 80"														<< "\n";
	output << "dump    PROC		; COMDAT"										<< "\n";
	output << "$LN10:"															<< "\n";
	output << "    sub     rsp, 72		; 00000048H"							<< "\n";
	output << "    mov     r8d, 1"												<< "\n";
	output << "    mov     BYTE PTR buffer$[rsp + 31], 10"						<< "\n";
	output << "    lea     r10, QWORD PTR buffer$[rsp + 30]"					<< "\n";
	output << "    mov     r11, -3689348814741910323	; cccccccccccccccdH"	<< "\n";
	output << "    align   2"													<< "\n";
	output << "$LL4@dump:"														<< "\n";
	output << "    mov     rax, r11"											<< "\n";
	output << "    lea     r10, QWORD PTR[r10 - 1]"								<< "\n";
	output << "    mul     rcx"													<< "\n";
	output << "    inc     r8"													<< "\n";
	output << "    shr     rdx, 3"												<< "\n";
	output << "    movzx   eax, dl"												<< "\n";
	output << "    shl     al, 2"												<< "\n";
	output << "    lea     r9d, DWORD PTR[rax + rdx]"							<< "\n";
	output << "    add     r9b, r9b"											<< "\n";
	output << "    sub     cl, r9b"												<< "\n";
	output << "    add     cl, 48		; 00000030H"							<< "\n";
	output << "    mov     BYTE PTR[r10 + 1], cl"								<< "\n";
	output << "    mov     rcx, rdx"											<< "\n";
	output << "    test    rdx, rdx"											<< "\n";
	output << "    jne     SHORT $LL4@dump"										<< "\n";
	output << "    lea     rdx, QWORD PTR buffer$[rsp + 32]"					<< "\n";
	output << "    mov     ecx, 1"												<< "\n";
	output << "    sub     rdx, r8"												<< "\n";
	output << "    call    _write"												<< "\n";
	output << "    add     rsp, 72		; 00000048H"							<< "\n";
	output << "    ret     0"													<< "\n";
	output << "dump    ENDP"													<< "\n";
	output << "public _start"													<< "\n";
	output << "_start:"															<< "\n";
	for (int i = 0; i < size; i++)
	{
		switch (program[i].opcode)
		{
			case OP_PUSH:
			{
				output << "    ; -- push -- "	<< program[i].value << "\n";
				output << "    push "			<< program[i].value << "\n";
				break;
			}
			case OP_PLUS:
			{
				output << "    ; -- plus -- "						<< "\n";
				output << "    pop rax"								<< "\n";
				output << "    pop rbx"								<< "\n";
				output << "    add rax, rbx"						<< "\n";
				output << "    push rax"							<< "\n";
				break;
			}
			case OP_MINUS:
			{
				output << "    ; -- minus -- "						<< "\n";
				output << "    pop rax"								<< "\n";
				output << "    pop rbx"								<< "\n";
				output << "    sub rbx, rax"						<< "\n";
				output << "    push rbx"							<< "\n";
				break;
			}
			case OP_DUMP:
			{
				output << "    ; -- dump -- "						<< "\n";
				output << "    pop rcx"								<< "\n";
				output << "    call dump"							<< "\n";
				break;
			}
			default:
				assert(false && "unreachable");
		}
	}
	output << "    ret" << "\n";
	output << "end"		<< "\n";
	output.close();
}

Operation parse_token(std::string token)
{
	assert(COUNT_OPS == 4 && "Exhaustive op handling in parse_tokens");
	if (token == "+") { return plus(); }
	else if (token == "-") { return minus(); }
	else if (token == ".") { return dump(); }
	else 
	{ 
		// Attempt to convert token to an integer
		int value;
		auto [p, ec] = std::from_chars(token.data(), token.data() + token.size(), value);
		if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
			// Conversion failed (invalid input)
			// Handle the error appropriately (e.g., return an error operation)
			std::cerr << "[ERROR] Invalid input encountered: " << token << std::endl;
			std::exit(EXIT_FAILURE);
		}
		return push(value);
	}
}

std::vector<Operation> load_program_from_file(char* file_path)
{
	std::ifstream my_file;
	std::vector<Operation> tokens;
	my_file.open(file_path, std::ios::in);
	if (!my_file.is_open())
	{
		printf("[ERROR] Unable to open file %s\n", file_path);
	}

	std::string token;
	while (my_file >> token)
	{
		tokens.push_back(parse_token(token));
	}
	my_file.close();
	return tokens;
}

void usage(char* program_name)
{
	printf("Usage: %s <SUBCOMMAND> [ARGS]\n", program_name);
	printf("SUBCOMMANDS:\n");
	printf("    sim <file>    Simulate the program\n");
	printf("    com <file>    Compile the program\n");
}

void call_cmd(std::string cmd)
{
	printf("[INFO] %s\n", cmd.c_str());
	std::system(cmd.c_str());
}

char* shift(int* argc, char** argv[])
{
	assert(*argc > 0);
	char* result = **argv;
	*argv += 1;
	*argc -= 1;
	return result;
}

int main(int argc, char* argv[])
{	
	char* program_name;
	program_name = shift(&argc, &argv);
	if (argc < 1)
	{
		usage(program_name);
		printf("[ERROR] No subcommand is provided\n");
		return 1;
	}
	const std::string subcommand = shift(&argc, &argv);
	if (subcommand == "sim") 
	{ 
		if (argc < 1)
		{
			usage(program_name);
			printf("[ERROR] No input file is provided for the simulation\n");
			return 1;
		}
		char* program_path = shift(&argc, &argv);
		std::vector<Operation> program = load_program_from_file(program_path);
		simulate_program(program);
	}
	else if (subcommand == "com") 
	{ 
		if (argc < 1)
		{
			usage(program_name);
			printf("[ERROR] No input file is provided for the compilation\n");
			return 1;
		}
		char* program_path = shift(&argc, &argv);
		std::vector<Operation> program = load_program_from_file(program_path);
		compile_program(program, "output.asm");
		call_cmd("ml64 -nologo /c /Cx output.asm");
		call_cmd("link -nologo /SUBSYSTEM:CONSOLE /NODEFAULTLIB /entry:_start output.obj ucrt.lib");
	}
	else 
	{
		usage(program_name);
		printf("[ERROR] Unknown subcommand %s\n", subcommand.c_str());
	}
}