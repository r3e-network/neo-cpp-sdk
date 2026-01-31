#pragma once

#include <string>
#include <vector>
#include "neocpp/types/types.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

namespace neocpp {

// Forward declarations
class BinaryWriter;
class BinaryReader;

/// Neo Executable Format (NEF) file
/// NEF3 Format: Magic(4) + Compiler(var) + Version(var) + Source(var) + Script(var) + Checksum(4)
class NefFile : public NeoSerializable {
private:
    std::string magic_;      // "NEF3"
    std::string compiler_;   // Compiler name
    std::string version_;    // Compiler version (semantic)
    std::string source_;     // Source code hash/link (can be empty)
    Bytes script_;          // Contract script
    Bytes checksum_;        // 4-byte CRC32 checksum

public:
    /// Constructor
    NefFile();

    /// Constructor with script
    /// @param script The contract script
    /// @param compiler The compiler name
    /// @param version The version
    /// @param source The source code hash/link (optional)
    NefFile(const Bytes& script, const std::string& compiler = "NeoCpp",
            const std::string& version = "1.0.0", const std::string& source = "");

    /// Destructor
    ~NefFile() = default;

    // Getters
    const std::string& getMagic() const { return magic_; }
    const std::string& getCompiler() const { return compiler_; }
    const std::string& getVersion() const { return version_; }
    const std::string& getSource() const { return source_; }
    const Bytes& getScript() const { return script_; }
    const Bytes& getChecksum() const { return checksum_; }

    // Setters
    void setCompiler(const std::string& compiler) { compiler_ = compiler; updateChecksum(); }
    void setVersion(const std::string& version) { version_ = version; updateChecksum(); }
    void setSource(const std::string& source) { source_ = source; updateChecksum(); }
    void setScript(const Bytes& script);

    /// Calculate and update checksum
    void updateChecksum();

    /// Verify checksum
    /// @return True if checksum is valid
    bool verifyChecksum() const;

    /// Convert to Base64 string
    /// @return The Base64 encoded NEF
    std::string toBase64() const;

    /// Parse from Base64 string
    /// @param base64 The Base64 encoded NEF
    /// @return The NEF file
    static NefFile fromBase64(const std::string& base64);

    /// Convert to bytes
    /// @return The serialized NEF bytes
    Bytes toBytes() const;

    // NeoSerializable interface
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    static NefFile deserialize(BinaryReader& reader);
};

} // namespace neocpp
