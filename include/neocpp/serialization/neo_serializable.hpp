#pragma once

#include <cstddef>
#include "neocpp/types/types.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"

namespace neocpp {

/// Interface for Neo serializable objects
class NeoSerializable {
public:
    virtual ~NeoSerializable() = default;

    /// Get the size of the serialized data in bytes
    /// @return The size in bytes
    virtual size_t getSize() const = 0;

    /// Serialize the object to a binary writer
    /// @param writer The binary writer to serialize to
    virtual void serialize(BinaryWriter& writer) const = 0;

    /// Convert the object to a byte array
    /// @return The serialized bytes
    Bytes toArray() const;
};

/// Deserialize a NeoSerializable object from a BinaryReader
/// @tparam T The type of object to deserialize
/// @param reader The BinaryReader to read from
/// @return The deserialized object
/// @throws NeoSwiftError if deserialization fails
// This is a template function because C++ doesn't support static virtual methods
// Each derived class should have a matching deserializeImpl method
// Example: static T deserializeImpl(BinaryReader& reader)

template<typename T>
T deserialize(BinaryReader& reader) {
    return T::deserializeImpl(reader);
} // namespace neocpp
/// Deserialize a NeoSerializable object from a byte array
/// @tparam T The type of object to deserialize
/// @param bytes The byte array to read from
/// @return The deserialized object
/// @throws NeoSwiftError if deserialization fails
template<typename T>
T fromBytes(const Bytes& bytes) {
    BinaryReader reader(bytes);
    return deserialize<T>(reader);
} // namespace neocpp
} // namespace neocpp
