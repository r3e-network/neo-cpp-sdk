#include "neocpp/serialization/neo_serializable.hpp"
#include "neocpp/serialization/binary_writer.hpp"

namespace neocpp {

Bytes NeoSerializable::toArray() const {
    BinaryWriter writer;
    serialize(writer);
    return writer.toArray();
}

} // namespace neocpp