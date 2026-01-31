#pragma once

#include <cstdint>

namespace neocpp {

/// Scrypt parameters for key derivation
class ScryptParams {
private:
    int n_;          // CPU/memory cost parameter
    int r_;          // Block size parameter
    int p_;          // Parallelization parameter
    int dkLen_;      // Derived key length

public:
    /// Constructor
    /// @param n CPU/memory cost parameter (must be power of 2)
    /// @param r Block size parameter
    /// @param p Parallelization parameter
    /// @param dkLen Derived key length in bytes
    ScryptParams(int n, int r, int p, int dkLen = 64);

    /// Get default NEP-2 scrypt parameters
    /// @return Default parameters (N=16384, r=8, p=8, dkLen=64)
    static ScryptParams getDefault();

    /// Get light scrypt parameters (for testing)
    /// @return Light parameters (N=256, r=1, p=1, dkLen=64)
    static ScryptParams getLight();

    /// Get N parameter
    [[nodiscard]] int getN() const { return n_; }

    /// Get R parameter
    [[nodiscard]] int getR() const { return r_; }

    /// Get P parameter
    [[nodiscard]] int getP() const { return p_; }

    /// Get derived key length
    [[nodiscard]] int getDkLen() const { return dkLen_; }

    /// Validate parameters
    /// @return True if parameters are valid
    bool isValid() const;
};

} // namespace neocpp
