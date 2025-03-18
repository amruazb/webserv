#pragma once
#include <exception>
#include <string>
#include "webserv.hpp"

class HttpException : public std::exception {
    private:
        std::string _code;    // HTTP error code (e.g., "400")
        std::string _message; // HTTP error message (e.g., "Bad Request")
    
    public:
        HttpException(const std::string& code, const std::string& message)
            : _code(code), _message(message) {}

        // Destructor with throw() for C++98 compatibility
        virtual ~HttpException() throw() {}

        // Return the error code
        const char* what() const throw() {
            return _code.c_str();
        }

        // Return the error message
        const std::string& getMessage() const {
            return _message;
        }
};

class ErrorPage : public HttpException {
    private:
        const ServerTraits& _conf; // Server configuration for error pages
    
    public:
        // Constructor for ErrorPage
        ErrorPage(const ServerTraits& conf, const std::string& code, const std::string& message)
            : HttpException(code, message), _conf(conf) {}

        // Destructor with throw() for C++98 compatibility
        virtual ~ErrorPage() throw() {}

        // Getter for the server configuration
        const ServerTraits& getConf() const {
            return _conf;
        }

        // Explicitly declare getMessage() to ensure visibility in C++98
        const std::string& getMessage() const {
            return HttpException::getMessage();
        }
};