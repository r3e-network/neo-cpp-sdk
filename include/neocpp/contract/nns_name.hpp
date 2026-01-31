#pragma once

#include <string>
#include <vector>
#include "../types/types.hpp"
#include "../neo_swift_error.hpp"

namespace neocpp {

/**
 * Represents a NeoNameService domain name
 * Equivalent to NNSName in the Swift SDK
 */
class NNSName {
public:
    /**
     * Creates a NNS name and checks its validity
     * @param name The domain name
     * @throws ContractError if the name is invalid
     */
    explicit NNSName(const std::string& name);

    /**
     * Get the name
     * @return The domain name
     */
    const std::string& getName() const;

    /**
     * Get the name as a byte vector
     * @return The name as a byte vector
     */
    [[nodiscard]] Bytes getBytes() const;

    /**
     * Check if the name is a second-level domain
     * @return true if the name is a second-level domain, false otherwise
     */
    bool isSecondLevelDomain() const;

    /**
     * Check if a name is a valid NNS name
     * @param name The domain name
     * @param allowMultipleFragments Whether to allow multiple fragments
     * @return true if the name is valid, false otherwise
     */
    static bool isValidNNSName(const std::string& name, bool allowMultipleFragments);

    /**
     * Check if a fragment is valid
     * @param fragment The fragment to check
     * @param isRoot Whether the fragment is a root
     * @return true if the fragment is valid, false otherwise
     */
    static bool checkFragment(const std::string& fragment, bool isRoot);

    /**
     * Represents a NeoNameService root
     */
    class NNSRoot {
    public:
        /**
         * Creates a NNS root and checks its validity
         * @param root The root
         * @throws ContractError if the root is invalid
         */
        explicit NNSRoot(const std::string& root);

        /**
         * Get the root
         * @return The root
         */
        const std::string& getRoot() const;

        /**
         * Check if a root is valid
         * @param root The root to check
         * @return true if the root is valid, false otherwise
         */
        static bool isValidNNSRoot(const std::string& root);

    private:
        std::string root_;
    };

private:
    std::string name_;
};

} // namespace neocpp
