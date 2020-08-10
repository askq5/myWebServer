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

#define MAXLINEBUFFERSIZE 512

enum ProcessState
{
    STATE_PARSE_URL = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};

const char *ok_200_title = "OK";
const char *error_400_title = "Bad Request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.\n";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file form this server.\n";
const char *error_404_title = "Not Found";
const char *error_404_form = "The requested file was not found on this server.\n";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the request file.\n";

enum AnalysisState
{
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR
};

enum HttpCode
{
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURCE,
    FORBIDDEN_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

enum ChectState
{
    CHECK_STATE_REQUESTLINE = 0,
    CHECK_STATE_HEADER,
    CHECK_STATE_CONTENT,
    CHECK_STATE_FINISH
};

enum HttpMethod
{
    METHOD_POST = 1,
    METHOD_GET,
    METHOD_HEAD,
    METHOD_PUT,
    METHOD_TRACE,
    METHOD_DELETE,
    METHOD_PATH,
    METHOD_CONNECT,
    METHOD_OPTION
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
    
    
    string  inBuffer_ ;
    std::string  outBuffer_;
    bool error_;
   
    int nowReadPos_;
    HttpMethod method_;
    HttpVersion httpVersion_;
    std::string fileName_;
    
    ChectState checkState_;
    HttpCode reqState_;
    ProcessState state_;
    
    bool keepAlive_;
    string host_;
    int contentLength_;
    std::map<std::string, std::string> headers_;
    //std::weak_ptr<TimerNode> timer_;

    //void handleWrite();
    //void handleConn();
    bool addResponse(const char *format, ...);
    bool addStatusLine(int status,const char * str);
    bool addHeaders();
    bool addContentLength();
    bool addContentType();
    bool addLinger();
    bool addBlankLine();
    bool addContent(string & str);
    HttpCode  fillOutBufer();
    HttpCode parseRequest();
    void handleError(int errNum, std::string &&short_msg);
    HttpCode parseURL();
    HttpCode parseHeaders();
    HttpCode parseContent();
    HttpCode analysisRequest();
};

#endif
