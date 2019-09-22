#pragma once


namespace http
{

enum class HttpDirection
{
    DOWNSTREAM,  // toward the client
    UPSTREAM     // toward the origin application or data
};
    
} // namespace http