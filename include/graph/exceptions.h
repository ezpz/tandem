#ifndef EXCEPTIONS_H__
#define EXCEPTIONS_H__

#include <exception>
#include <string>
#include <cstdio>
#include <graph/types.h>
#include <graph/range.h>

struct GeneralException : public std::exception {
    std::string msg_;
    GeneralException (const char *msg, const char *file, int line) :
        std::exception() {
        char buff[512] = {0};
        int n = snprintf (buff, 512, "%s:%d:%s\n", file, line, msg);
        if (n >= 512) {
            buff[511] = '\0';
        }
        msg_ = std::string(buff);
    }
    virtual ~GeneralException () throw () {}
    virtual const char *what () const throw () { return msg_.c_str (); }

};

struct InvalidRange : public std::exception {
    std::string msg_;
    InvalidRange (const FloatType& x, const FloatType& y) : 
        std::exception() {
        char buff[512] = {0};
        snprintf (buff, 512,
                "Invalid Range - x:%0.6f y:%0.6f", x, y);
        msg_ = std::string(buff);
    }
    virtual ~InvalidRange () throw () {}
    virtual const char *what () const throw () { return msg_.c_str (); }

};

struct NotInRange : public std::exception {
    std::string msg_;
    NotInRange (const FloatType& point, const Range& range) : 
        std::exception() {
        char buff[512] = {0};
        snprintf (buff, 512,
                "Point %0.6f not in range [%0.6f,%0.6f] ", 
                 point, range.X (), range.Y ());
        msg_ = std::string(buff);
    }
    virtual ~NotInRange () throw () {}
    virtual const char *what () const throw () { return msg_.c_str (); }

};


#endif /* EXCEPTIONS_H__ */
