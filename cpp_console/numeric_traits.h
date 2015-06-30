#ifndef NUMERIC_TRAITS_H
#define NUMERIC_TRAITS_H

namespace pgr {

template<typename T>
struct numeric_traits
{
    //max_length: max length of numeric literals
    //name      : name of numeric type
    //fmt_str   : format string that can be pass to functions such as sprintf
};

template<>
struct numeric_traits<int>
{
    static const int max_length = 32;
    static const char* name() { return "int"; }
    static const char* fmt_str() { return "%d"; }
};

template<>
struct numeric_traits<long>
{
    static const int max_length = 32;
    static const char* name() { return "long"; }
    static const char* fmt_str() { return "%ld"; }
};

template<>
struct numeric_traits<long long>
{
    static const int max_length = 32;
    static const char* name() { return "long long"; }
    static const char* fmt_str()
    {
#ifndef WIN32
        return "%lld";
#else
        return "%I64d";
#endif
    }
};


template<>
struct numeric_traits<unsigned>
{
    static const int max_length = 32;
    static const char* name() { return "unsigned"; }
    static const char* fmt_str() { return "%u"; }
};


template<>
struct numeric_traits<unsigned long>
{
    static const int max_length = 32;
    static const char* name() { return "unsigned long"; }
    static const char* fmt_str() { return "%lu"; }
};

template<>
struct numeric_traits<unsigned long long>
{
    static const int max_length = 32;
    static const char* name() { return "unsigned long long"; }
    static const char* fmt_str()
    {
#ifndef WIN32
        return "%llu";
#else
        return "%I64u";
#endif
    }
};

template<>
struct numeric_traits<float>
{
    static const int max_length = 512;
    static const char* name() { return "float"; }
    static const char* fmt_str() { return "%f"; }
};

template<>
struct numeric_traits<double>
{
    static const int max_length = 512;
    static const char* name() { return "double"; }
    static const char* fmt_str() { return "%f"; }
};

template<>
struct numeric_traits<long double>
{
    static const int max_length = 512;
    static const char* name() { return "long double"; }
    static const char* fmt_str() { return "%Lf"; }
};

} //namespace pgr

#endif // NUMERIC_TRAITS_H

