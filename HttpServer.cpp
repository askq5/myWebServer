/*************************************************************************
	> File Name: HttpServer.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 07:58:31 PM CST
 ************************************************************************/
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <mysql/mysql.h>
#include "HttpServer.h"
#include "SqlConnectionPool.h"

using namespace std;

// pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
// std::unordered_map<std::string, std::string> MimeType::mime;
// const int DEFAULT_EXPIRED_TIME = 2000;             // ms
// const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000; // ms

const char * ok_200_title = "OK";
const char * error_400_title = "Bad Request";
const char * error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.\n";
const char * error_403_title = "Forbidden";
const char * error_403_form = "You do not have permission to get file form this server.\n";
const char * error_404_title = "Not Found";
const char * error_404_form = "The requested file was not found on this server.\n";
const char * error_500_title = "Internal Error";
const char * error_500_form = "There was an unusual problem serving the request file.\n";


void MimeType::init()
{
    mime[".html"] = "text/html";
    mime[".avi"] = "video/x-msvideo";
    mime[".bmp"] = "image/bmp";
    mime[".c"] = "text/plain";
    mime[".doc"] = "application/msword";
    mime[".gif"] = "image/gif";
    mime[".gz"] = "application/x-gzip";
    mime[".htm"] = "text/html";
    mime[".ico"] = "image/x-icon";
    mime[".jpg"] = "image/jpeg";
    mime[".png"] = "image/png";
    mime[".txt"] = "text/plain";
    mime[".mp3"] = "audio/mp3";
    mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string &suffix)
{
    pthread_once(&once_control, MimeType::init);
    if (mime.find(suffix) == mime.end())
        return mime["default"];
    else
        return mime[suffix];
}

SqlConnectionPool * HttpServer::sqlConnPool = SqlConnectionPool::GetInstance();

HttpServer::HttpServer()
    : error_(false),
      method_(METHOD_GET),
      httpVersion_(HTTP_11),
      checkState_(CHECK_STATE_REQUESTLINE),
      reqState_(GET_REQUEST),
      keepAlive_(false)
{
    
    //fileName_ = PATHSOURCE;
    char  buf[500];
    getcwd(buf,500);
    fileName_ = buf;
    pthread_mutex_init(&mutex_,NULL);
    // loop_->queueInLoop(bind(&HttpServer::setHandlers, this));
    //   channel_->setReadHandler(bind(&HttpServer::handleRead, this));
    //   channel_->setWriteHandler(bind(&HttpServer::handleWrite, this));
    //   channel_->setConnHandler(bind(&HttpServer::handleConn, this));
}

 HttpServer::~HttpServer() 
 { pthread_mutex_destroy(&mutex_);}

void HttpServer::initSqlConnPool(string user, string passWord, string DBName, int maxConn, 
                int closeLog,string url,int port)
{
    sqlConnPool->init(user,passWord,DBName,maxConn,closeLog,url,port);
}

void HttpServer::initMySql()
{
    ConnectionRAII mysqlcon(&mySql_, sqlConnPool);

    //在user表中检索username，passwd数据，浏览器端输入
    if (mysql_query(mySql_, "SELECT username,passwd FROM user"))
    {
        //LOG_ERROR("SELECT error:%s\n", mysql_error(mysql));
    }

    //从表中检索完整的结果集
    MYSQL_RES *result = mysql_store_result(mySql_);

    //返回结果集中的列数
    //int num_fields = mysql_num_fields(result);

    //返回所有字段结构的数组
    //MYSQL_FIELD *fields = mysql_fetch_fields(result);

    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    while (MYSQL_ROW row = mysql_fetch_row(result))
    {
        string temp1(row[0]);
        string temp2(row[1]);
        users_[temp1] = temp2;
    }

    return ;
}

int HttpServer::handleMessage(std::string & inBuffer, std::string & outBuffer)
{
    inBuffer_ = inBuffer;
    parseRequest();
    swap(outBuffer_,outBuffer);
    return 0;
}

void HttpServer::reset()
{
    // inBuffer_.clear();
    fileName_ = PATHSOURCE;
    headers_.clear();
    // keepAlive_ = false;
    // if (timer_.lock())
    // {
    //     shared_ptr<TimerNode> my_timer(timer_.lock());
    //     //my_timer->clearReq();
    //     timer_.reset();
    // }
}

// void HttpServer::seperateTimer()
// {
//     // cout << "seperateTimer" << endl;
//     if (timer_.lock())
//     {
//         shared_ptr<TimerNode> my_timer(timer_.lock());
//         my_timer->clearReq();
//         timer_.reset();
//     }
// }

HttpCode HttpServer::parseRequest()
{
    //__uint32_t &events_ = channel_->getEvents();
    
    HttpCode ret = NO_REQUEST;
    checkState_ = CHECK_STATE_REQUESTLINE;

    while(checkState_ != CHECK_STATE_FINISH)
    {
        //LOG_INFO("%s", text);
        switch(CHECK_STATE_REQUESTLINE)
        {
            case CHECK_STATE_REQUESTLINE:
            {
                ret = parseURL();
                if (ret == BAD_REQUEST)
                    return BAD_REQUEST;
                else
                    checkState_ = CHECK_STATE_HEADER;
                break;
            }
            case CHECK_STATE_HEADER:
            {
                ret = parseHeaders();
                if (ret == BAD_REQUEST)
                    return BAD_REQUEST;
                else if (method_ ==  METHOD_GET && ret == GET_REQUEST)
                {
                    ret = analysisRequest();
                    checkState_ = CHECK_STATE_FINISH;
                }
                else if(method_ ==  METHOD_GET && ret == GET_REQUEST)
                {
                    checkState_ = CHECK_STATE_CONTENT;
                }
                break;
            }
            case CHECK_STATE_CONTENT:
            {
                ret = parseContent();
                if (ret == GET_REQUEST)
                {
                    ret = analysisRequest();
                    checkState_ = CHECK_STATE_FINISH;
                }
                break;
            }
            default:
                return INTERNAL_ERROR;
        }
    }
    

    // if (!error_)
    // {
    //     if (outBuffer_.size() > 0)
    //     {
    //         return ;
    //         //handleWrite();
    //         // events_ |= EPOLLOUT;
    //     }
    //     // error_ may change
    //     if (!error_ && state_ == STATE_FINISH)
    //     {
    //         this->reset();
    //         if (inBuffer_.size() > 0)
    //         {
    //             if (connectionState_ != H_DISCONNECTING)
    //                 parseRequest();
    //         }
    //     }
    //     else if (!error_ && connectionState_ != H_DISCONNECTED)
    //     {
    //         //重新读？
    //         //events_ |= EPOLLIN;
    //     }
    
    return ret;
}

HttpCode HttpServer::parseURL()
{
    
    string cop = inBuffer_;
    // 读到完整的请求行再开始解析请求
    size_t pos = inBuffer_.find('\r', 0);
    if (pos < 0)
    {
        return BAD_REQUEST;
    }
    // 去掉请求行所占的空间，节省空间
    string requestLine = inBuffer_.substr(0, pos);
    if (inBuffer_.size() > pos + 1)
        inBuffer_ = inBuffer_.substr(pos + 1);
    else
        inBuffer_.clear();
    // Method
    int posGet = requestLine.find("GET");
    int posPost = requestLine.find("POST");
    int posHead = requestLine.find("HEAD");

    if (posGet >= 0)
    {
        pos = posGet;
        method_ = METHOD_GET;
    }
    else if (posPost >= 0)
    {
        pos = posPost;
        method_ = METHOD_POST;
    }
    else if (posHead >= 0)
    {
        pos = posHead;
        method_ = METHOD_HEAD;
    }
    else
    {
        return BAD_REQUEST;
    }

    
    pos = requestLine.find("/", pos);
    int endPos = requestLine.find(" ",pos);
    url_ = requestLine.substr(pos,endPos-pos);
    pos = requestLine.find("HTTP/1.1",endPos);
    if(pos >= 0)
        httpVersion_ = HTTP_11;
    else
        httpVersion_ = HTTP_10;
    
    return GET_REQUEST;
}

HttpCode HttpServer::parseHeaders()
{
    string pattern = "\r\n\r\n";
    size_t pos = inBuffer_.find(pattern);
    if(pos == inBuffer_.npos)
        return BAD_REQUEST;
    string str = inBuffer_.substr(0,pos+2);
    inBuffer_ = inBuffer_.substr(pos+2);
    string connState = "Connection: ";
    string contLen = "Content-length: ";
    string hostName = "Host: ";
    while(!str.empty())
    {
        pos = str.find("\r\n",0);
        if(pos == str.npos) 
            break;
        string line = str.substr(0,pos);
        str = str.substr(pos+2);
        size_t posConn = line.find(connState);
        size_t posContent = line.find(contLen);
        size_t posHost = line.find(hostName);
        if(posConn  != line.npos)
        {
            if(line.find("keep-alive"))
                keepAlive_ = true;
        }
        else if(posContent != line.npos)
        {
            line = line.substr(posContent+contLen.size());
            contentLength_ = atoi(line.c_str());
        }
        else if(posHost != line.npos)
        {
            line = line.substr(posContent+hostName.size());
            host_ = line;
        }
        else
        {
            string unKnowStr = "oop!unkown header: ";
            unKnowStr += line;
        }

    }

    return GET_REQUEST;
}

HttpCode HttpServer::parseContent()
{
    size_t namePos = inBuffer_.find("name=");
    if(namePos == inBuffer_.npos)
        return BAD_REQUEST;
    size_t passWord = inBuffer_.find("passwd=");
    if(passWord == inBuffer_.npos)
        return BAD_REQUEST;
    return GET_REQUEST;
}

HttpCode HttpServer::analysisRequest()
{
    if (method_ == METHOD_POST)
    {
        size_t namePos = inBuffer_.find("name=");
        size_t passWdPos = inBuffer_.find("passwd=");
        size_t finPos = inBuffer_.find("&");
        std::string name = inBuffer_.substr(namePos+5,finPos-namePos-5);
        std::string passWd = inBuffer_.substr(passWdPos+7);

        if(url_.find("3") != url_.npos)
        {
            //如果是注册，先检测数据库中是否有重名的
            //没有重名的，进行增加数据
            char * sql_insert = (char *)malloc(sizeof(char) * 200);
            strcpy(sql_insert, "INSERT INTO user(username, passwd) VALUES(");
            strcat(sql_insert, "'");
            strcat(sql_insert, name.c_str());
            strcat(sql_insert, "', '");
            strcat(sql_insert, passWd.c_str());
            strcat(sql_insert, "')");

            if (users_.find(name) == users_.end())
            {
                pthread_mutex_lock(&mutex_);
                int res = mysql_query(mySql_, sql_insert);
                users_.insert(pair<string, string>(name, passWd));
                pthread_mutex_unlock(&mutex_);
                if (!res)
                    fileName_ += "log.html";
                else
                    fileName_ += "/registerError.html";
            }
            else
                fileName_ += "registerError.html";
        }
        //如果是登录，直接判断
        //若浏览器端输入的用户名和密码在表中可以查找到，返回1，否则返回0
        else if (url_.find("2") != url_.npos)
        {
            if (users_.find(name) != users_.end() && users_[name] == passWd)
                fileName_ += "/welcome.html";
            else
                fileName_ += "/logError.html";
        }
        
        struct stat fileState;
        if (stat(fileName_.c_str(), &fileState) < 0)
            reqState_ = NO_RESOURCE;

        if (!(fileState.st_mode & S_IROTH))
            reqState_ = FORBIDDEN_REQUEST;

        if (S_ISDIR(fileState.st_mode))
            reqState_ = BAD_REQUEST;

    }
    else if (method_ == METHOD_GET || method_ == METHOD_HEAD)
    {
        
        if(url_ == "/")
        {
            fileName_ += "judge.html";
        }
        else if(url_.find("0") != url_.npos)
        {
            fileName_ += "/register.html";
        }
        else if (url_.find("1") != url_.npos)
        {
            fileName_ += "/log.html";
        }
        else if (url_.find("5") != url_.npos)
        {
            fileName_ += "/picture.html";
        }
        else if(url_.find("6") != url_.npos)
        {
            fileName_ += "/video.html";
        }
        else if(url_.find("7") != url_.npos)
        {
            fileName_ += "/fans.html";
        }
        else
            reqState_ = BAD_REQUEST;

        struct stat fileState;
        if (stat(fileName_.c_str(), &fileState) < 0)
            reqState_ = NO_RESOURCE;

        if (!(fileState.st_mode & S_IROTH))
            reqState_ = FORBIDDEN_REQUEST;

        if (S_ISDIR(fileState.st_mode))
            reqState_ = BAD_REQUEST;

        
    }

    return fillOutBufer();

}

HttpCode   HttpServer::fillOutBufer()
{
    switch(reqState_)
    {
        case INTERNAL_ERROR:
        {
            addStatusLine(500, error_500_title);
            addHeaders();
            handleError(500, error_500_form);
            break;
        }
        case NO_REQUEST:
        {
            addStatusLine(404, error_404_title);
            addHeaders();
            handleError(404, error_404_form);
            break;
        }
        case GET_REQUEST:
        {
            break;
        }
        case BAD_REQUEST:
        {
            addStatusLine(404, error_404_title);
            addHeaders();
            handleError(404, error_404_form);
            break;
        }
        case FORBIDDEN_REQUEST:
        {
            addStatusLine(404, error_404_title);
            addHeaders();
            handleError(404, error_404_form);
            break;
        }
        case FILE_REQUEST:
        {
            //outBuffer_ += "HTTP/1.1 200 OK\r\n";
            addStatusLine(200, ok_200_title);
            ifstream fin(fileName_,ios::in);
            istreambuf_iterator<char> beg(fin),end;
            string str(beg,end);
            contentLength_ = str.size();
            addHeaders();
            outBuffer_ += str;
            //outBuffer_ += "?";
            fin.close();
            break;
        }
        default :
            return BAD_REQUEST;

    }

    return GET_REQUEST;
}

bool HttpServer::addResponse(const char *format, ...)
{
    va_list arg_list;
    va_start(arg_list, format);
    char writeBufer[MAXLINEBUFFERSIZE];
    int len = vsnprintf(writeBufer, MAXLINEBUFFERSIZE, format, arg_list);
    if (len >= MAXLINEBUFFERSIZE)
    {
        va_end(arg_list);
        return false;
    }
    va_end(arg_list);
    outBuffer_ += writeBufer;
    //LOG_INFO("request:%s", m_write_buf);

    return true;

}

bool HttpServer::addStatusLine(int status,const char * title)
{
    std::string httpVer;
    if(httpVersion_ == HTTP_11)
        httpVer = "HTTP/1.1";
    else if(httpVersion_ == HTTP_10)
        httpVer = "HTTP/1.0";
    else
        return false;

    return addResponse("%s %d %s\r\n",httpVer.c_str(),status,title);
}
bool HttpServer::addHeaders()
{
    return addContentLength() && addContentType() && addLinger() && addBlankLine();
}
bool HttpServer::addContentLength()
{
    return addResponse("Content-Length:%d\r\n", contentLength_);
}
bool HttpServer::addContentType()
{
    return addResponse("Content-Type:%s\r\n", "text/html");
}
bool HttpServer::addLinger()
{
    return addResponse("Connection:%s\r\n", (keepAlive_ == true) ? "keep-alive" : "close");
}
bool HttpServer::addBlankLine()
{
    return addResponse("%s", "\r\n");
}
bool HttpServer::addContent(string & str)
{
    outBuffer_ += str;
    return true;
}
void HttpServer::handleError(int errNum, const char * shortMsg)
{
    
    //char send_buff[4096];
    string bodyBuffer;
    bodyBuffer += "<html><title>哎~出错了</title>";
    bodyBuffer += "<body bgcolor=\"ffffff\">";
    bodyBuffer += to_string(errNum) + shortMsg;
    bodyBuffer += "<hr><em> nemo's Web Server</em>\n</body></html>";
    outBuffer_ += bodyBuffer;
    
    return ;
}



