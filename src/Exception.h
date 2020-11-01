#ifndef SIMPLE_SERVER_EXCEPTION_H
#define SIMPLE_SERVER_EXCEPTION_H

#include <cstring>
#include <exception>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <cstdio>

class Exception : public std::runtime_error {
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the char* lies
     *                 with the caller. 
     */
    explicit Exception(const char *message = "") noexcept: msg(message), std::runtime_error(message) {}

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit Exception(const std::string &message) noexcept: msg(message), std::runtime_error(message) {}

    Exception() noexcept: std::runtime_error("") {}

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~Exception() noexcept override = default;

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    const char *what() const noexcept override {
        return msg.c_str();
    }

protected:
    /** Error message.
     */
    std::string msg;
};

#endif // SIMPLE_SERVER_EXCEPTION_H