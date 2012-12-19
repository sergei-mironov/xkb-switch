// X11Exception.h
// C++ exception that wraps X11 errors.
// Copyright (C) 2008 Jay Bromley <jbromley@gmail.com>

#ifndef X11EXCEPTION
#define X11EXCEPTION

#include <exception>
#include <stdexcept>

#define CHECK_MSG(x,msg) do{ \
		if(!(x)) { \
			std::ostringstream oss; \
			oss << __FILE__ << ":" << __LINE__ << ": Condition " << #x << " failed. " << msg; \
			throw std::runtime_error(oss.str()); \
		} \
	} while(0)

#define CHECK(x) CHECK_MSG(x,"")

class X11Exception : public std::exception
{
public:
    X11Exception() : _reason("unknown") {}
    X11Exception(const std::string& what) : _reason(what) {}
    virtual ~X11Exception() throw () {};
    virtual const char* what() const throw () { return _reason.c_str(); }

private:
    std::string _reason;
};

#endif
