#pragma once

#include <cstdint>
#include <string>

namespace neocpp {

/// Neo VM operation codes - Neo N3 v3.9.1 compliant
/// Based on official Neo VM specification:
/// https://github.com/neo-project/neo-vm/blob/master/src/Neo.VM/OpCode.cs
enum class OpCode : uint8_t {
    // Constants (0x00-0x20)
    PUSHINT8 = 0x00,
    PUSHINT16 = 0x01,
    PUSHINT32 = 0x02,
    PUSHINT64 = 0x03,
    PUSHINT128 = 0x04,
    PUSHINT256 = 0x05,
    PUSHT = 0x08,
    PUSHF = 0x09,
    PUSHA = 0x0A,
    PUSHNULL = 0x0B,
    PUSHDATA1 = 0x12,
    PUSHDATA2 = 0x13,
    PUSHDATA4 = 0x14,
    PUSHM1 = 0x1F,
    PUSH0 = 0x20,
    PUSH1 = 0x21,
    PUSH2 = 0x22,
    PUSH3 = 0x23,
    PUSH4 = 0x24,
    PUSH5 = 0x25,
    PUSH6 = 0x26,
    PUSH7 = 0x27,
    PUSH8 = 0x28,
    PUSH9 = 0x29,
    PUSH10 = 0x2A,
    PUSH11 = 0x2B,
    PUSH12 = 0x2C,
    PUSH13 = 0x2D,
    PUSH14 = 0x2E,
    PUSH15 = 0x2F,
    PUSH16 = 0x30,

    // Flow control (0x40-0x5F)
    NOP = 0x61,
    JMP = 0x62,
    JMP_L = 0x63,
    JMPIF = 0x64,
    JMPIF_L = 0x65,
    JMPIFNOT = 0x66,
    JMPIFNOT_L = 0x67,
    JMPEQ = 0x68,
    JMPEQ_L = 0x69,
    JMPNE = 0x6A,
    JMPNE_L = 0x6B,
    JMPGT = 0x6C,
    JMPGT_L = 0x6D,
    JMPGE = 0x6E,
    JMPGE_L = 0x6F,
    JMPLT = 0x70,
    JMPLT_L = 0x71,
    JMPLE = 0x72,
    JMPLE_L = 0x73,
    CALL = 0x74,
    CALL_L = 0x75,
    CALLA = 0x76,
    CALLT = 0x77,
    ABORT = 0x78,
    ASSERT = 0x79,
    THROW = 0x7A,
    TRY = 0x7B,
    TRY_L = 0x7C,
    ENDTRY = 0x7D,
    ENDTRY_L = 0x7E,
    ENDFINALLY = 0x7F,
    RET = 0x80,
    SYSCALL = 0x81,

    // Stack (0x82-0x97)
    DEPTH = 0x82,
    DROP = 0x83,
    NIP = 0x84,
    XDROP = 0x85,
    CLEAR = 0x86,
    DUP = 0x87,
    OVER = 0x88,
    PICK = 0x89,
    TUCK = 0x8A,
    SWAP = 0x8B,
    ROT = 0x8C,
    ROLL = 0x8D,
    REVERSE3 = 0x8E,
    REVERSE4 = 0x8F,
    REVERSEN = 0x90,

    // Slot (0x91-0xC2)
    INITSSLOT = 0x91,
    INITSLOT = 0x92,
    LDSFLD0 = 0x93,
    LDSFLD1 = 0x94,
    LDSFLD2 = 0x95,
    LDSFLD3 = 0x96,
    LDSFLD4 = 0x97,
    LDSFLD5 = 0x98,
    LDSFLD6 = 0x99,
    LDSFLD = 0x9A,
    STSFLD0 = 0x9B,
    STSFLD1 = 0x9C,
    STSFLD2 = 0x9D,
    STSFLD3 = 0x9E,
    STSFLD4 = 0x9F,
    STSFLD5 = 0xA0,
    STSFLD6 = 0xA1,
    STSFLD = 0xA2,
    LDLOC0 = 0xA3,
    LDLOC1 = 0xA4,
    LDLOC2 = 0xA5,
    LDLOC3 = 0xA6,
    LDLOC4 = 0xA7,
    LDLOC5 = 0xA8,
    LDLOC6 = 0xA9,
    LDLOC = 0xAA,
    STLOC0 = 0xAB,
    STLOC1 = 0xAC,
    STLOC2 = 0xAD,
    STLOC3 = 0xAE,
    STLOC4 = 0xAF,
    STLOC5 = 0xB0,
    STLOC6 = 0xB1,
    STLOC = 0xB2,
    LDARG0 = 0xB3,
    LDARG1 = 0xB4,
    LDARG2 = 0xB5,
    LDARG3 = 0xB6,
    LDARG4 = 0xB7,
    LDARG5 = 0xB8,
    LDARG6 = 0xB9,
    LDARG = 0xBA,
    STARG0 = 0xBB,
    STARG1 = 0xBC,
    STARG2 = 0xBD,
    STARG3 = 0xBE,
    STARG4 = 0xBF,
    STARG5 = 0xC0,
    STARG6 = 0xC1,
    STARG = 0xC2,

    // Compound (0xC3-0xC8)
    NEWBUFFER = 0xC3,
    MEMCPY = 0xC4,
    CAT = 0xC5,
    SUBSTR = 0xC6,
    LEFT = 0xC7,
    RIGHT = 0xC8,

    // Arithmetic (0xD0-0xEF)
    INVERT = 0xD0,
    AND = 0xD1,
    OR = 0xD2,
    XOR = 0xD3,
    EQUAL = 0xD4,
    NOTEQUAL = 0xD5,
    SIGN = 0xD6,
    ABS = 0xD7,
    NEGATE = 0xD8,
    INC = 0xD9,
    DEC = 0xDA,
    ADD = 0xDB,
    SUB = 0xDC,
    MUL = 0xDD,
    DIV = 0xDE,
    MOD = 0xDF,
    POW = 0xE0,
    SQRT = 0xE1,
    MODMUL = 0xE2,
    MODPOW = 0xE3,
    SHL = 0xE8,
    SHR = 0xE9,
    NOT = 0xEA,
    BOOLAND = 0xEB,
    BOOLOR = 0xEC,
    NZ = 0xF0,
    NUMEQUAL = 0xF1,
    NUMNOTEQUAL = 0xF2,
    LT = 0xF3,
    LE = 0xF4,
    GT = 0xF5,
    GE = 0xF6,
    MIN = 0xF7,
    MAX = 0xF8,
    WITHIN = 0xF9,

    // Array/Map operations
    PACK = 0xBE,
    UNPACK = 0xBF,
    NEWARRAY0 = 0xC0,
    NEWARRAY = 0xC1,
    NEWARRAY_T = 0xC2,
    NEWSTRUCT0 = 0xC5,
    NEWSTRUCT = 0xC6,
    NEWMAP = 0xC8,
    SIZE = 0xCA,
    HASKEY = 0xCB,
    KEYS = 0xCC,
    VALUES = 0xCD,
    PICKITEM = 0xCE,
    APPEND = 0xCF,
    SETITEM = 0xD0,
    REVERSEITEMS = 0xD1,
    REMOVE = 0xD2,
    CLEARITEMS = 0xD3,
    POPITEM = 0xD4,
    ISNULL = 0xD8,
    ISTYPE = 0xD9,
    CONVERT = 0xDB,
    PACKMAP = 0xDF
};

/// Helper class for OpCode operations
class OpCodeHelper {
public:
    /// Convert OpCode to byte value
    static uint8_t toByte(OpCode opcode) {
        return static_cast<uint8_t>(opcode);
    }

    /// Convert byte value to OpCode
    static OpCode fromByte(uint8_t value) {
        return static_cast<OpCode>(value);
    }

    /// Get the name of an OpCode
    static std::string getName(OpCode opcode);

    /// Get the size of the OpCode operand
    static int getOperandSize(OpCode opcode);

    /// Check if OpCode is a push operation
    static bool isPush(OpCode opcode);

    /// Get the push value for simple push opcodes (PUSH0-PUSH16)
    static int getPushValue(OpCode opcode);
};

} // namespace neocpp
