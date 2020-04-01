#include <gtest/gtest.h>
#include "http/Http1xCodec.h"
#include <fstream>
#include "util/easylogging++.h"
#include "boost/filesystem.hpp"

using namespace http;

std::string ReadHttpRequestLineFromFile(const std::string& requestLineFullFileName)
{
    auto filePath = boost::filesystem::path{requestLineFullFileName};
    if(!boost::filesystem::exists(filePath))
    {
        LOG(ERROR) << "headerFile: " << requestLineFullFileName << " does not exist...";
        return "";
    }
    std::string retMessageString;
    std::fstream fs{requestLineFullFileName, std::ios_base::in};
    char* buf = (char*)::calloc(1, 4096);
    while(fs.getline(buf, 4096))
    {
        retMessageString.append(buf, strlen(buf));
        retMessageString.append("\n");
        memset(buf, 0, 4096);
    }
    free(buf);
    fs.close();
    return retMessageString;
}

std::string ReadHttpHeadersFromFile(const std::string& headerFileFullName)
{
    auto filePath = boost::filesystem::path{headerFileFullName};
    if(!boost::filesystem::exists(filePath))
    {
        LOG(ERROR) << "headerFile: " << headerFileFullName << " does not exist...";
        return "";
    }
    std::string retMessageString;
    std::fstream fs{headerFileFullName, std::ios_base::in};
    char* buf = (char*)::calloc(1, 4096);
    while(fs.getline(buf, 4096))
    {
        retMessageString.append(buf, strlen(buf));
        retMessageString.append("\n");
        memset(buf, 0, 4096);
    }
    retMessageString.append("\n");
    free(buf);
    fs.close();
    return retMessageString;
}

std::string ReadHttpBodyFromFile(const std::string& bodyFileFullName)
{
    auto filePath = boost::filesystem::path{bodyFileFullName};
    if(!boost::filesystem::exists(filePath))
    {
        LOG(ERROR) << "bodyFile: " << bodyFileFullName << " does not exist...";
        return "";
    }
    std::string retMessageBody;
    std::fstream fs{bodyFileFullName, std::ios_base::binary | std::ios_base::in};
    char* buf = (char*)::calloc(1, 4096);
    while(fs.get(buf, 4096))
    {
        retMessageBody.append(buf, strlen(buf));
        memset(buf, 0, 4096);
    }
    char eof[2];
    eof[0] = -1;
    eof[1] = '\0';
    retMessageBody.append(eof);
    free(buf);
    fs.close();
    return retMessageBody;
}


TEST(Http1xCodec, normal_downstream)
{
    Http1xCodec codec{HttpDirection::DOWNSTREAM};
    std::string requestLineData = ReadHttpRequestLineFromFile("./http/HttpResponseRequestLineExmp.txt");
    std::string headerData = ReadHttpHeadersFromFile("./http/HttpResponseHeadersExmp.txt");
    std::string bodyData = ReadHttpBodyFromFile("./http/HttpResponseBodyExmp.txt");
    std::string message{};
    message.append(requestLineData).append(headerData).append(bodyData);
    auto ret = codec.onIngress(message);
    if(ret <= 0) FAIL();
    ASSERT_EQ(codec.state_, http::Http1xCodec::CodecState::ON_MESSAGE_COMPLETE);
}

TEST(Http1xCodec, downstream_with_steps)
{
    Http1xCodec codec{HttpDirection::DOWNSTREAM};
    std::string requestLineData = ReadHttpRequestLineFromFile("./http/HttpResponseRequestLineExmp.txt");
    std::string headerData = ReadHttpHeadersFromFile("./http/HttpResponseHeadersExmp.txt");
    std::string bodyData = ReadHttpBodyFromFile("./http/HttpResponseBodyExmp.txt");

    auto ret = codec.onIngress(requestLineData);
    ASSERT_EQ(codec.status(), 200);
    if(ret <= 0) FAIL();
    ASSERT_EQ(codec.state_, http::Http1xCodec::CodecState::ON_STATUS);

    ret = codec.onIngress(headerData);
    if(ret <= 0) FAIL();
    ASSERT_EQ(codec.state_, http::Http1xCodec::CodecState::ON_HEADERS_COMPLETE);

    ret = codec.onIngress(bodyData);
    if(ret <= 0) FAIL();
    ASSERT_EQ(codec.state_, http::Http1xCodec::CodecState::ON_MESSAGE_COMPLETE);
}

TEST(Http1xCodec, downStream_pause_Parser)
{
    Http1xCodec codec{HttpDirection::DOWNSTREAM};
    std::string requestLineData = ReadHttpRequestLineFromFile("./http/HttpResponseRequestLineExmp.txt");
    std::string headerData = ReadHttpHeadersFromFile("./http/HttpResponseHeadersExmp.txt");
    std::string bodyData = ReadHttpBodyFromFile("./http/HttpResponseBodyExmp.txt");

    auto ret = codec.onIngress(requestLineData);
    auto ispaused = codec.pause(1);
    ASSERT_EQ(ispaused, 0);
    ASSERT_EQ(codec.state_, http::Http1xCodec::CodecState::PAUSED);

//continue to parse after paused, onIngress return 0;
    ret = codec.onIngress(headerData);
    ASSERT_EQ(ret, 0);

    //unpause the codec
    ispaused = codec.pause(0);
    ASSERT_EQ(ispaused, 0);
    ASSERT_EQ(codec.state_, http::Http1xCodec::CodecState::ON_STATUS);

    ret = codec.onIngress(headerData);
    ASSERT_TRUE(ret > 0);
    ASSERT_EQ(codec.state_, http::Http1xCodec::CodecState::ON_HEADERS_COMPLETE);
}

TEST(Http1xCodec, normal_uptream)
{
    Http1xCodec codec{HttpDirection::UPSTREAM};
    string_piece::const_string_piece requestData = "\
GET /favicon.ico HTTP/1.1\n\
Host: www.baidu.com\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:69.0) Gecko/20100101 Firefox/69.0\n\
Accept: image/webp,*/*\n\
Accept-Language: en-US,en;q=0.5\n\
Accept-Encoding: gzip, deflate, br\n\
Connection: keep-alive\n\
Cookie: BAIDUID=5B9A613886836BF21ED0C6B44D6ED1C6:FG=1; BIDUPSID=5B9A613886836BF21ED0C6B44D6ED1C6; PSTM=1566466421; BD_UPN=13314752; COOKIE_SESSION=3_0_8_1_5_4_0_0_8_2_0_0_2142_0_10_0_1568768701_0_1568768691%7C9%231222234_1_1568016187%7C2; sug=3; sugstore=1; ORIGIN=0; bdime=0; delPer=0; BD_HOME=0; H_PS_PSSID=1450_21107_29523_29721_29568_29220_26350; BD_CK_SAM=1; PSINO=7; BDORZ=B490B5EBF6F3CD402E515D22BCDA1598\n\
Pragma: no-cache\n\
Cache-Control: no-cache\n\n";

    codec.onIngress(requestData);

    ASSERT_EQ(codec.state_, Http1xCodec::CodecState::ON_MESSAGE_COMPLETE);
    
}
