#include"LogStream.h"
#include<assert.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<algorithm>
#include<limits>

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

template<typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;
    do
    {
        int lsd = static_cast<int> (i%10);
        i/=10;
        *p++ = zero[lsd];
    }while(i!=0);

    if(value<0)
        *p++ = '-';
    *p='\0';
    std::reverse(buf,p);
    return p-buf;
}

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

template <typename T>
void LogStream::formatInteger(T v)
{
    if(buffer_.avail() >= kMaxNumericSize)
    {
        size_t len = convert(buffer_.current(),v);
        buffer_.add(len);
    }
}

LogStream& LogStream::operator<<(bool v)
{
    buffer_.append(v ? "1":"0",1);
    return *this;
}

LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream& LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if(buffer_.avail() >= kMaxNumericSize)
    {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double v)
{
    if(buffer_.avail() >= kMaxNumericSize)
    {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
        buffer_.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(char v)
{
    buffer_.append(&v,1);
    return *this;
}
LogStream& LogStream::operator<<(const char* str)
{
    if(str)
        buffer_.append(str, strlen(str));
    else
        buffer_.append("(NULL)",6);
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char* str)
{
    return operator<<(reinterpret_cast<const char*>(str));
}

LogStream& LogStream::operator<<(const std::string& v)
{
    buffer_.append(v.c_str(), v.size());
    return *this;
}
