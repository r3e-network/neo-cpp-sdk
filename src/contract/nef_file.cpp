#include "neocpp/contract/nef_file.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/utils/base64.hpp"
#include "neocpp/exceptions.hpp"
#include <sstream>

namespace neocpp {

NefFile::NefFile() : magic_("NEF3"), compiler_("NeoCpp"), version_("1.0.0"), source_(""), checksum_(4, 0) {
}

NefFile::NefFile(const Bytes& script, const std::string& compiler, const std::string& version, const std::string& source)
    : magic_("NEF3"), compiler_(compiler), version_(version), source_(source), script_(script), checksum_(4, 0) {
    updateChecksum();
}

void NefFile::setScript(const Bytes& script) {
    script_ = script;
    updateChecksum();
}

void NefFile::updateChecksum() {
    // Serialize without checksum
    std::stringstream ss;
    BinaryWriter writer(ss);
    writer.writeBytes(Bytes(magic_.begin(), magic_.end()));
    writer.writeVarString(compiler_);
    writer.writeVarString(version_);
    writer.writeVarBytes(script_);
    
    // Get serialized data
    std::string str = ss.str();
    Bytes data(str.begin(), str.end());
    
    // Calculate SHA256 checksum
    checksum_ = HashUtils::sha256(HashUtils::sha256(data));
    
    // Take first 4 bytes
    checksum_.resize(4);
}

bool NefFile::verifyChecksum() const {
    // Serialize without checksum
    std::stringstream ss;
    BinaryWriter writer(ss);
    writer.writeBytes(Bytes(magic_.begin(), magic_.end()));
    writer.writeVarString(compiler_);
    writer.writeVarString(version_);
    writer.writeVarBytes(script_);
    
    // Get serialized data
    std::string str = ss.str();
    Bytes data(str.begin(), str.end());
    
    // Calculate SHA256 checksum
    auto calculated = HashUtils::sha256(HashUtils::sha256(data));
    
    // Compare first 4 bytes
    if (checksum_.size() != 4) {
        return false;
    }
    
    for (size_t i = 0; i < 4; ++i) {
        if (checksum_[i] != calculated[i]) {
            return false;
        }
    }
    
    return true;
}

Bytes NefFile::toBytes() const {
    std::stringstream ss;
    BinaryWriter writer(ss);
    serialize(writer);
    std::string str = ss.str();
    return Bytes(str.begin(), str.end());
}

std::string NefFile::toBase64() const {
    return Base64::encode(toBytes());
}

NefFile NefFile::fromBase64(const std::string& base64) {
    auto data = Base64::decode(base64);
    BinaryReader reader(data);
    return deserialize(reader);
}

size_t NefFile::getSize() const {
    size_t size = 0;
    size += 4; // magic length
    size += compiler_.length() + 1; // varstring
    size += version_.length() + 1; // varstring
    size += 1 + script_.size(); // varbytes (assuming < 253 bytes)
    if (script_.size() >= 253) {
        size += 2; // Additional bytes for larger sizes
    }
    size += 4; // checksum
    return size;
}

void NefFile::serialize(BinaryWriter& writer) const {
    writer.writeBytes(Bytes(magic_.begin(), magic_.end()));
    writer.writeVarString(compiler_);
    writer.writeVarString(version_);
    writer.writeVarBytes(script_);
    writer.writeBytes(checksum_);
}

NefFile NefFile::deserialize(BinaryReader& reader) {
    NefFile nef;
    
    // Read magic
    Bytes magicBytes = reader.readBytes(4);
    nef.magic_ = std::string(magicBytes.begin(), magicBytes.end());
    if (nef.magic_ != "NEF3") {
        throw IllegalArgumentException("Invalid NEF magic: " + nef.magic_);
    }
    
    // Read compiler and version
    nef.compiler_ = reader.readVarString();
    nef.version_ = reader.readVarString();
    
    // Read script
    nef.script_ = reader.readVarBytes();
    
    // Read checksum
    nef.checksum_ = reader.readBytes(4);
    
    // Verify checksum
    if (!nef.verifyChecksum()) {
        throw IllegalArgumentException("Invalid NEF checksum");
    }
    
    return nef;
}

} // namespace neocpp