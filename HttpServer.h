/*************************************************************************
	> File Name: HttpServer.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 07:58:31 PM CST
 ************************************************************************/

#ifndef _HTTPSERVER_H
#define _HTTPSERVER_H

#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
//#include "Timer.h"

enum ProcessState
{
    STATE_PARSE_URL = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};

enum URLState
{
    PARSE_URL_AGAIN = 1,
    PARSE_URL_ERROR,
    PARSE_URL_SUCCESS,
};

enum HeaderState
{
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
};

enum AnalysisState
{
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR
};

enum ParseState
{
    H_START = 0,
    H_KEY,
    H_COLON,
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF
};

enum ConnectionState
{
    H_CONNECTED = 0,
    H_DISCONNECTING,
    H_DISCONNECTED
};

enum HttpMethod
{
    METHOD_POST = 1,
    METHOD_GET,
    METHOD_HEAD
};

enum HttpVersion
{
    HTTP_10 = 1,
    HTTP_11
};

class MimeType
{
private:
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    MimeType();
    MimeType(const MimeType &m);

public:
    static std::string getMime(const std::string &suffix);

private:
    static pthread_once_t once_control;
};

class HttpServer: public std::enable_shared_from_this<HttpServer>
{
public:
    HttpServer();
    ~HttpServer() {}
    void reset();
    void seperateTimer();
    //   void linkTimer(std::shared_ptr<TimerNode> mtimer) {
    //     // shared_ptr重载了bool, 但weak_ptr没有
    //     timer_ = mtimer;
    //   }

    //void handleClose();
    //void newEvent();

    int handleMessage(std::string &inBuffer, std::string &outBuffer);

private:
    std::string url_;
    
    std::string  inBuffer_ ;
    std::string  outBuffer_;
    bool error_;
    ConnectionState connectionState_;

    HttpMethod method_;
    HttpVersion HTTPVersion_;
    std::string fileName_;
    std::string path_;
    int nowReadPos_;
    ProcessState state_;
    ParseState hState_;
    bool keepAlive_;
    std::map<std::string, std::string> headers_;
    //std::weak_ptr<TimerNode> timer_;

    //void handleWrite();
    //void handleConn();
    void parseRequest();
    void handleError(int errNum, std::string &&short_msg);
    URLState parseURL();
    HeaderState parseHeaders();
    AnalysisState analysisRequest();
};

#endif
