#include <iostream>
 
using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct Mem 
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise()
    {
        for(u32 i =0 ; i<MAX_MEM;i++){
            Data[i]=0x00;
        }
    }

    //read 1 byte
    Byte operator[](u32 Address) const
    {
        return Data[Address];
    }

    //write 1 byte
    Byte& operator[](u32 Address)
    {
        return Data[Address];
    }
};

struct CPU
{

    Word PC;    //program counter
    Word SP;    //stack pointer

    Byte A, X, Y;   //registers

    Byte C:1;
    Byte Z:1;
    Byte I:1;
    Byte D:1;
    Byte B:1;
    Byte V:1;
    Byte N:1;


    void Reset(Mem& memory)
    {
        PC = 0xFFFC;
        SP = 0X0100;

        A=0;
        X=0;
        Y=0;

        C=0;
        Z=0;
        I=0;
        D=0;
        B=0;
        V=0;
        N=0;

        memory.Initialise();
    }

    Byte FetchByte(u32& Cycles, Mem& memory)
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Word FetchWord(u32& Cycles, Mem& memory)
    {
        //6502 is little endian
        Byte Data = memory[PC];
        PC++;
    
        Data |= (memory[PC]<<8);
        PC++;
        return Data;
    }

    //reading 1 byte from memory
    Byte ReadByte(u32& Cycles,u32& Address, Mem& memory)
    {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    static constexpr Byte 
            INS_LDA_IM  = 0xA9,
            INS_LDA_ZP  = 0xA5,
            INS_LDA_ZPX = 0xB5;

    void LDASetStatuc()
    {
            Z = (A == 0);
            N = (A & 0b10000000) > 0;
    }

    void Execute(u32 Cycles, Mem& memory)
    {
        while( Cycles < 0 )
        {
            Byte Ins = FetchByte(Cycles, memory );
            switch (Ins)
            {
            case INS_LDA_IM:{
                Byte Value =
                    FetchByte( Cycles, memory);
                    A = Value;
                    LDASetStatuc();
            }
            break;
            case INS_LDA_ZP:{
                Byte ZeroPageAddr =
                    FetchByte( Cycles, memory);
                    A = ReadByte(ZeroPageAddr, mem);
                    LDASetStatuc();
            }
            break;
            case INS_LDA_ZPX:{
                Byte ZeroPageAddr =
                    FetchByte( Cycles, memory);
                    ZeroPageAddr += x;
                    Cycles--;
                    A = ReadByte(Cycles,ZeroPageAddr, mem);
                    LDASetStatuc();
            }
            break;
            default:
            {
                std::cout << "Instruction not handled " << Ins;
            }
            break;
                
            }
        }
    }
};

int main()
{
    std::cout<<"Welcome!";
    Mem mem;
    CPU cpu;
    cpu.Reset(mem);

    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x42;
    mem[ 0x42 ] = 0x84;

    cpu.Execute(2 ,mem );

    return 0;

}