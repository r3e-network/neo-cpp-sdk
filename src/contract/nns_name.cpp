#include "neocpp/contract/nns_name.hpp"
#include "neocpp/neo_swift_error.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace neocpp {

NNSName::NNSName(const std::string& name) : name_(name) {
    if (!isValidNNSName(name, true)) {
        throw ContractError::invalidNeoName(name);
    }
}

const std::string& NNSName::getName() const {
    return name_;
}

Bytes NNSName::getBytes() const {
    Bytes result;
    result.reserve(name_.size());
    std::copy(name_.begin(), name_.end(), std::back_inserter(result));
    return result;
}

bool NNSName::isSecondLevelDomain() const {
    return isValidNNSName(name_, false);
}

bool NNSName::isValidNNSName(const std::string& name, bool allowMultipleFragments) {
    // Check name length
    if (name.length() < 3 || name.length() > 255) {
        return false;
    }
    
    // Split name into fragments
    std::vector<std::string> fragments;
    std::istringstream ss(name);
    std::string fragment;
    while (std::getline(ss, fragment, '.')) {
        fragments.push_back(fragment);
    }
    
    // Check fragment count
    if (fragments.size() < 2 || fragments.size() > 8) {
        return false;
    }
    
    // Check if multiple fragments are allowed
    if (fragments.size() > 2 && !allowMultipleFragments) {
        return false;
    }
    
    // Check each fragment
    for (size_t i = 0; i < fragments.size(); i++) {
        bool isRoot = (i == fragments.size() - 1);
        if (!checkFragment(fragments[i], isRoot)) {
            return false;
        }
    }
    
    return true;
}

bool NNSName::checkFragment(const std::string& fragment, bool isRoot) {
    const size_t maxLength = isRoot ? 16 : 63;
    
    // Check fragment length
    if (fragment.empty() || fragment.length() > maxLength) {
        return false;
    }
    
    // Check first character
    char firstChar = fragment[0];
    if (isRoot && !std::isalpha(firstChar)) {
        return false;
    } else if (!isRoot && !std::isalnum(firstChar)) {
        return false;
    }
    
    // If fragment is a single character, we're done
    if (fragment.length() == 1) {
        return true;
    }
    
    // Check middle characters
    for (size_t i = 1; i < fragment.length() - 1; i++) {
        char c = fragment[i];
        if (!std::isalnum(c) && c != '-') {
            return false;
        }
    }
    
    // Check last character
    char lastChar = fragment[fragment.length() - 1];
    return std::isalnum(lastChar);
}

// NNSRoot implementation
NNSName::NNSRoot::NNSRoot(const std::string& root) : root_(root) {
    if (!isValidNNSRoot(root)) {
        throw ContractError::invalidNNSRoot(root);
    }
}

const std::string& NNSName::NNSRoot::getRoot() const {
    return root_;
}

bool NNSName::NNSRoot::isValidNNSRoot(const std::string& root) {
    return NNSName::checkFragment(root, true);
}

} // namespace neocpp