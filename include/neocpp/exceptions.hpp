#pragma once

#include <string>
#include <stdexcept>

namespace neocpp {

/// Base exception class for Neo SDK
class NeoException : public std::runtime_error {
public:
    explicit NeoException(const std::string& message)
        : std::runtime_error(message) {}
};

/// Exception thrown when a method is not yet implemented
class NotImplementedException : public NeoException {
public:
    explicit NotImplementedException(const std::string& message)
        : NeoException("Not implemented: " + message) {}

    NotImplementedException()
        : NeoException("Method not yet implemented") {}
};

/// Exception for illegal arguments
class IllegalArgumentException : public NeoException {
public:
    explicit IllegalArgumentException(const std::string& message)
        : NeoException("Illegal argument: " + message) {}
};

/// Exception for illegal state
class IllegalStateException : public NeoException {
public:
    explicit IllegalStateException(const std::string& message)
        : NeoException("Illegal state: " + message) {}
};

/// Exception for runtime errors
class RuntimeException : public NeoException {
public:
    explicit RuntimeException(const std::string& message)
        : NeoException("Runtime error: " + message) {}
};

/// Exception for unsupported operations
class UnsupportedOperationException : public NeoException {
public:
    explicit UnsupportedOperationException(const std::string& message)
        : NeoException("Unsupported operation: " + message) {}
};

/// Exception for invalid format
class InvalidFormatException : public NeoException {
public:
    explicit InvalidFormatException(const std::string& message)
        : NeoException("Invalid format: " + message) {}
};

/// Exception for deserialization errors
class DeserializationException : public NeoException {
public:
    explicit DeserializationException(const std::string& message)
        : NeoException("Deserialization error: " + message) {}
};

/// Exception for serialization errors
class SerializationException : public NeoException {
public:
    explicit SerializationException(const std::string& message)
        : NeoException("Serialization error: " + message) {}
};

/// Exception for script errors
class ScriptException : public NeoException {
public:
    explicit ScriptException(const std::string& message)
        : NeoException("Script error: " + message) {}
};

/// Exception for transaction errors
class TransactionException : public NeoException {
public:
    explicit TransactionException(const std::string& message)
        : NeoException("Transaction error: " + message) {}
};

/// Exception for wallet errors
class WalletException : public NeoException {
public:
    explicit WalletException(const std::string& message)
        : NeoException("Wallet error: " + message) {}
};

/// Exception for network errors
class NetworkException : public NeoException {
public:
    explicit NetworkException(const std::string& message)
        : NeoException("Network error: " + message) {}
};

/// Exception for cryptographic errors
class CryptoException : public NeoException {
public:
    explicit CryptoException(const std::string& message)
        : NeoException("Crypto error: " + message) {}
};

/// Exception for NEP-2 operations
class NEP2Exception : public NeoException {
public:
    explicit NEP2Exception(const std::string& message)
        : NeoException("NEP-2 error: " + message) {}
};

/// Exception for signing operations
class SignException : public NeoException {
public:
    explicit SignException(const std::string& message)
        : NeoException("Sign error: " + message) {}
};

/// Exception for RPC operations
class RpcException : public NeoException {
public:
    explicit RpcException(const std::string& message)
        : NeoException("RPC error: " + message) {}
};

} // namespace neocpp
