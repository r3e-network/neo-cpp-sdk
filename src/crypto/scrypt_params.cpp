
#include "neocpp/crypto/scrypt_params.hpp"
#include "neocpp/exceptions.hpp"

namespace neocpp {

ScryptParams::ScryptParams(int n, int r, int p, int dkLen)
    : n_(n), r_(r), p_(p), dkLen_(dkLen) {
    if (!isValid()) {
        throw IllegalArgumentException("Invalid scrypt parameters");
    }
} // namespace neocpp
ScryptParams ScryptParams::getDefault() {
    return ScryptParams(16384, 8, 8, 64);
} // namespace neocpp
ScryptParams ScryptParams::getLight() {
    return ScryptParams(256, 1, 1, 64);
} // namespace neocpp
bool ScryptParams::isValid() const {
    // Check N is power of 2
    if (n_ <= 0 || (n_ & (n_ - 1)) != 0) {
        return false;
    }

    // Check other parameters are positive
    if (r_ <= 0 || p_ <= 0 || dkLen_ <= 0) {
        return false;
    }

    // Check memory requirements won't overflow
    // Scrypt requires approximately 128 * N * r bytes
    uint64_t memoryRequired = static_cast<uint64_t>(128) * n_ * r_;
    if (memoryRequired > 1024 * 1024 * 1024) { // Limit to 1GB
        return false;
    }

    return true;
} // namespace neocpp
} // namespace neocpp
