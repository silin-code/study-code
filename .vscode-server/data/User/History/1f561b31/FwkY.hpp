#pragma once

#include <iostream>

class Request
{
public:
    Request() {}
    Request(int x, int y, char o) : _data_x(x), _data_y(y), _oper(o)
    {
    }
    ~Request()
    {
    }

public:
    bool Serialize(std::string *out)
    {
        
    }

    bool Deserialize(std::string &in)
    {

    }

private:
    int _data_x;
    int _data_y;
    char _oper;
};

class Response
{
public:
    Response(int result, int code) : _result(result), _code(code)
    {
    }

    ~Response()
    {
    }

private:
    int _result;
    int _code;
};