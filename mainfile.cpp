/*
CURRENTLY WIP PROJECT
A simplified custom assembly language
Written by Anato.
*/

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <cstdlib>
#include <sstream>
using namespace std;
/*
TODO: (top is easiest, bottom is hardest.)
- Update all opcodes to update the flags
- Implement the remaining machine operations
- Create a helper function for handling RAM, Register, and Immediate values, both writing and reading.
- Implement RAM addressing via '@'
- Update RAM so each address is 1 byte
- Update opcodes to support the 1 byte limit
- Implement try/catch handling for errors
- Write an assembler for binary opcode
- Switch to a binary opcode system rather than keywords
- Instead of if...else if statements, use a switch statement
- Get SDL2 support working

-- DOCS --
Instructions:
+ NOTE: All mentions of R(x) can be interchanged with immediate values as well.
* Arithmetic *
Add R1, R2, R3 : R1 = X, R2 = Y, R3 = result
Sub R1, R2, R3 : R1 = X, R2 = Y, R3 = result
Mul R1, R2, R3 : R1 = X, R2 = Y, R3 = result
Div R1, R2, R3 : R1 = X, R2 = Y, R3 = result
Mod R1, R2, R3 : R1 = X, R2 = Y, R3 = result

* Branching *
Jmp R1 : R1 = destination
Jeq R1 : R1 = destination
Jlt R1 : R1 = destination
Jgt R1 : R1 = destination

* Comparison *
Cmp R1, R2 : R1 = X, R2 = Y

* Machine control *
Gdi R1 : R1 = register/ram address. Takes user input, places them in R1.
Sdl : enables SDL2 system. (Note: REFER TO RAM LOCATIONS FOR CONFIGS.)
Ens : Enables output to a save file.
Hlt : Halts the CPU.
Mmc R1, R2 : R1 = address, R2 = value. Modifies RAM directly.
Read R1 : R1 = address
Mov R1, R2 : R1 = Value, R2 = target (ram address or register address)

* Registers and flags *
- Flags -
ZF : Zero flag - true or false, determines whether last operation resulted in zero or not.
CF : Carry flag - true or false, determines if the last operation has become larger than 65535
SF : Sign Flag - true or false, determines if the last operstion resulted in a negative or positive number. True for Negative, False for Positive.

- Registers -
R0 to R31 are available for usage. This totals up to 32 registers, BUT NOTE THAT R32 IS NOT AVAILABLE.

RAM Locations:
* SDL2 *
SDL2 has it's own dedicated section of RAM initialized, called VRAM. VRAM has 518,000 addresses available, 720x720. 1 pixel per address.
*/

class Cpu{
  private:
    bool Zero = false;
    bool Carry = false;
    bool Sign = false;
    int PC = 0;
    uint32_t reg[32] = {0};
    vector<string> RAM{65536, "0"};
  public:
    // Making the save data variables public to make them adjustable outside of this class.
    bool save_output = false;
    string save_filename = "halt_finish.txt";
    
    // _ signifies a helper function.
    uint32_t _get_val(const string& input) {
      if (input[0] == 'R') {return reg[stoi(input.substr(1))];}
      else {return (uint32_t)stoi(input);}
    }
    
    uint32_t _imm_or_reg(const string& input){
      if (input[0] == 'R'){return stoi(input.substr(1));}
      else {return (uint32_t)stoi(input);}
    }
    
    void add(const string& x, const string& y, const string& result){
      int a = _get_val(x);
      int b = _get_val(y);
      
      int res = _imm_or_reg(result);
      reg[res] = a+b;
    }
    
    void sub(const string& x, const string& y, const string& result){
      int a = _get_val(x);
      int b = _get_val(y);
      int res = _imm_or_reg(result);
      reg[res] = a-b;
    }
    
    void mul(const string& x, const string& y, const string& result){
      int a = _get_val(x);
      int b = _get_val(y);
      int res = _imm_or_reg(result);
      reg[res] = a*b;
    }
    
    void div(const string& x, const string& y, const string& result){
      int a = _get_val(x);
      int b = _get_val(y);
      int res = _imm_or_reg(result);
      reg[res] = a/b;
    }
    
    void mod(const string& x, const string& y, const string& result){
      int a = _get_val(x);
      int b = _get_val(y);
      int res = _imm_or_reg(result);
      reg[res] = a%b;
    }
    
    void jmp(const string& dest){
      PC = _get_val(dest)-1;
    }
    
    void jeq(const string& dest){
      if (Zero == true){
        PC = _get_val(dest)-1;
      }
    }
    
    void jlt(const string& dest){
      if (Sign == true){
        PC = _get_val(dest)-1;
      }
    }
    
    void jgt(const string& dest){
      if (Sign == false && Zero == false){
        PC = _get_val(dest)-1;
      }
    }
    
    void cmp(const string& x, const string& y){
      int a = _get_val(x);
      int b = _get_val(y);
      int result = a-b;

      if (result < 0){Zero = false; Carry = false; Sign = true;}
      else if (result > 0){Zero = false; Carry = (result > 2147483647LL) ? true : false; Sign = false;}
      else if (result == 0){Zero = true; Carry = false; Sign = false;}
      
    }
    
    void gdi(const string& dest){
      string userin = "";
      getline(cin, userin);
      if (dest[0] == 'R' || dest[0] == 'r'){
        int index = stoi(dest.substr(1));
        uint32_t val = (uint32_t)stoi(userin);
        reg[index] = val;
      }
      
      else{
        int addr = _imm_or_reg(dest);
        RAM[addr] = userin;
      }
    }
    
    void sdl(){
    }
    
    void halt(){
      if (save_output){
        // Declare file
        fstream file(save_filename, ios::app);
        
        if (file.is_open()){
          // Formatting for the file output
          file  << "RAM (0 - 65535): [";
          
          // Iterate through items in RAM and append them one by one.
          for (const auto& str : RAM){file << str << ",";}
          file << "]\nPC: " << PC << "\nRegisters (R0 - R31): [";
          
          // Do the same for registers as well
          for (const auto& str : reg){file << str << ",";}
          file << "]";
          file.close();
        }
        
        cout << "Program has finished. Saving data to " << save_filename << endl;
        exit(EXIT_SUCCESS);
        }
        
        else{
          cout << "Program has finished." << endl;
          exit(EXIT_SUCCESS);
        }
    }
    
    void ens(){
      save_output = true;
    }
    
    void mmc(string addr, string value){
      int data = _get_val(value);
      int index = stoi(addr.substr(1));
      RAM[index] = data;
    }
    
    // P.S : This read functionality will be temporary. I'll make it more complicated in the future.
    void read(string addr, string mode){
      int res = _imm_or_reg(addr);
      if (mode == "'register'"){cout << reg[res] << endl;}
      else{cout << RAM[res] << endl;}
    }
    
    void mov(string value, string value2){
      int val = _get_val(value);
      if (value2[0] == 'R' || value[0] == 'r'){
        reg[stoi(value2.substr(1))] = val;
      }
      
      else if(value2[0] == '@'){
        RAM[stoi(value2.substr(1))] = val;
      }
      
      else{
        cout << "Invalid value: not register or RAM address. (Received immediate value for second parameter.)" << endl;
        halt();
      }
    }
    
    void run(const vector<string>& PRG){
      bool running = true;
      while (running){
        // Declare variables needed to fetch-decode-execute
        stringstream inst_full(PRG.at(PC));
        string inst_segment;
        vector<string> decomp_inst;
        
        // Separate instructions and data
        while(getline(inst_full, inst_segment, ' ')){
          size_t pos=0;
          string target = ",";
          string replacement = "";
          
          // Remove commas from the instruction
          while((pos=inst_segment.find(",",pos)) != string::npos){
            inst_segment.replace(pos,target.length()," ");
            pos+=replacement.length();}
          decomp_inst.push_back(inst_segment);
        }
        
        // Actual instruction and data
        // For readability sakes.
        string inst = decomp_inst.at(0);
        vector<string> data;
        for (size_t i = 1; i < decomp_inst.size(); i++){data.push_back(decomp_inst.at(i));}
        PC++;
        
        // P.S If anyone ever suggests I use a dispatch table. I will bash their heads in :3
        // I have TRIED to use one.
        // It was how I implemented
        // it in python (a key-value dispatch table).
        // However, apparently
        // C++ is too rigid for that.
        // All values must follow the
        // format of the value defined at
        // the definition of the map.
        
        if (inst == "add"){add(data.at(0),data.at(1),data.at(2));}
        else if (inst == "sub"){sub(data.at(0),data.at(1),data.at(2));}
        else if (inst == "mul"){mul(data.at(0),data.at(1),data.at(2));}
        else if (inst == "div"){div(data.at(0),data.at(1),data.at(2));}
        else if (inst == "mod"){mod(data.at(0),data.at(1),data.at(2));}
        
        else if (inst == "jmp"){jmp(data.at(0));}
        else if (inst == "jeq"){jeq(data.at(0));}
        else if (inst == "jlt"){jlt(data.at(0));}
        else if (inst == "jgt"){jgt(data.at(0));}
        
        else if (inst == "cmp"){cmp(data.at(0), data.at(1));}
        
        else if (inst == "gdi"){gdi(data.at(0));}
        else if (inst == "sdl"){sdl();}
        else if (inst == "ens"){ens();}
        else if (inst == "hlt"){halt();running = false;}
        else if (inst == "mmc"){mmc(data.at(0),data.at(1));}
        else if (inst == "read"){read(data.at(0),data.at(1));}
        else if (inst == "mov"){mov(data.at(0),data.at(1));}
        else {cout << "Invalid opcode!";}
      }
      
    }
};

int main(){
  Cpu computer;
  // vector<string> PRG = {"ens","add 1, 1, 0","read 0 'register'","sub 1, 1, 1","read 1 'register'","mul 2, 2, 2","read 2 'register'","div 2, 2, 3","read 3 'register'","hlt"}; // 2 0 4 1 - my first ever program to work.
  // vector<string> PRG = {"ens", "gdi 0", "read 0, 'ram'", "jmp 1", "hlt"}; // First program written with GDI support, and branching!
  // vector<string> PRG = {"ens", "gdi R0", "gdi R1", "add R0, R1, R2", "read 2, 'register'", "jmp 1", "hlt"}; // First program with add and gdi interacting.
  // vector<string> PRG = {"ens", "read 0, 'register'", "mov 50, R0", "read 0, 'register'", "hlt"}; // First program to use MOV after it was implemented
  computer.run(PRG);
  return 0;
}