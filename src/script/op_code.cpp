
#include "neocpp/script/op_code.hpp"
#include <unordered_map>

namespace neocpp {

std::string OpCodeHelper::getName(OpCode opcode) {
    static const std::unordered_map<OpCode, std::string> names = {
        {OpCode::PUSHINT8, "PUSHINT8"},
        {OpCode::PUSHINT16, "PUSHINT16"},
        {OpCode::PUSHINT32, "PUSHINT32"},
        {OpCode::PUSHINT64, "PUSHINT64"},
        {OpCode::PUSHINT128, "PUSHINT128"},
        {OpCode::PUSHINT256, "PUSHINT256"},
        {OpCode::PUSHA, "PUSHA"},
        {OpCode::PUSHNULL, "PUSHNULL"},
        {OpCode::PUSHDATA1, "PUSHDATA1"},
        {OpCode::PUSHDATA2, "PUSHDATA2"},
        {OpCode::PUSHDATA4, "PUSHDATA4"},
        {OpCode::PUSHM1, "PUSHM1"},
        {OpCode::PUSH0, "PUSH0"},
        {OpCode::PUSH1, "PUSH1"},
        {OpCode::PUSH2, "PUSH2"},
        {OpCode::PUSH3, "PUSH3"},
        {OpCode::PUSH4, "PUSH4"},
        {OpCode::PUSH5, "PUSH5"},
        {OpCode::PUSH6, "PUSH6"},
        {OpCode::PUSH7, "PUSH7"},
        {OpCode::PUSH8, "PUSH8"},
        {OpCode::PUSH9, "PUSH9"},
        {OpCode::PUSH10, "PUSH10"},
        {OpCode::PUSH11, "PUSH11"},
        {OpCode::PUSH12, "PUSH12"},
        {OpCode::PUSH13, "PUSH13"},
        {OpCode::PUSH14, "PUSH14"},
        {OpCode::PUSH15, "PUSH15"},
        {OpCode::PUSH16, "PUSH16"},
        {OpCode::NOP, "NOP"},
        {OpCode::JMP, "JMP"},
        {OpCode::JMPIF, "JMPIF"},
        {OpCode::JMPIFNOT, "JMPIFNOT"},
        {OpCode::CALL, "CALL"},
        {OpCode::RET, "RET"},
        {OpCode::SYSCALL, "SYSCALL"},
        {OpCode::DUP, "DUP"},
        {OpCode::DROP, "DROP"},
        {OpCode::SWAP, "SWAP"},
        {OpCode::ROT, "ROT"},
        {OpCode::ROLL, "ROLL"},
        {OpCode::EQUAL, "EQUAL"},
        {OpCode::ADD, "ADD"},
        {OpCode::SUB, "SUB"},
        {OpCode::MUL, "MUL"},
        {OpCode::DIV, "DIV"},
        {OpCode::MOD, "MOD"},
        {OpCode::LT, "LT"},
        {OpCode::LE, "LE"},
        {OpCode::GT, "GT"},
        {OpCode::GE, "GE"},
        // Add more as needed
    };

    auto it = names.find(opcode);
    if (it != names.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

int OpCodeHelper::getOperandSize(OpCode opcode) {
    switch (opcode) {
        case OpCode::PUSHINT8:
        case OpCode::JMP:
        case OpCode::JMPIF:
        case OpCode::JMPIFNOT:
        case OpCode::JMPEQ:
        case OpCode::JMPNE:
        case OpCode::JMPGT:
        case OpCode::JMPGE:
        case OpCode::JMPLT:
        case OpCode::JMPLE:
        case OpCode::CALL:
        case OpCode::PUSHDATA1:
            return 1;

        case OpCode::PUSHINT16:
        case OpCode::PUSHDATA2:
            return 2;

        case OpCode::PUSHINT32:
        case OpCode::PUSHA:
        case OpCode::TRY:
        case OpCode::JMP_L:
        case OpCode::JMPIF_L:
        case OpCode::JMPIFNOT_L:
        case OpCode::JMPEQ_L:
        case OpCode::JMPNE_L:
        case OpCode::JMPGT_L:
        case OpCode::JMPGE_L:
        case OpCode::JMPLT_L:
        case OpCode::JMPLE_L:
        case OpCode::CALL_L:
        case OpCode::PUSHDATA4:
        case OpCode::SYSCALL:
            return 4;

        case OpCode::PUSHINT64:
        case OpCode::TRY_L:
            return 8;

        case OpCode::PUSHINT128:
            return 16;

        case OpCode::PUSHINT256:
            return 32;

        default:
            return 0;
    }
}

bool OpCodeHelper::isPush(OpCode opcode) {
    uint8_t value = toByte(opcode);
    // Neo N3 push opcodes:
    // 0x00-0x05: PUSHINT8, PUSHINT16, PUSHINT32, PUSHINT64, PUSHINT128, PUSHINT256
    // 0x08-0x0B: PUSHT, PUSHF, PUSHA, PUSHNULL
    // 0x12-0x14: PUSHDATA1, PUSHDATA2, PUSHDATA4
    // 0x1F: PUSHM1
    // 0x20-0x30: PUSH0-PUSH16
    return (value >= 0x00 && value <= 0x05) || (value >= 0x08 && value <= 0x0B) ||
           (value >= 0x12 && value <= 0x14) || value == 0x1F ||
           (value >= 0x20 && value <= 0x30);
}

int OpCodeHelper::getPushValue(OpCode opcode) {
    if (opcode == OpCode::PUSHM1) return -1;
    if (opcode >= OpCode::PUSH0 && opcode <= OpCode::PUSH16) {
        return static_cast<int>(opcode) - static_cast<int>(OpCode::PUSH0);
    }
    return -1;
}

} // namespace neocpp
