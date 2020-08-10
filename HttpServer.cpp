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
#include "HttpServer.h"

using namespace std;

pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;

const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;
const int DEFAULT_EXPIRED_TIME = 2000;             // ms
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000; // ms

char favicon[555] = {
    '\x89', 'P',    'N',    'G',    '\xD',  '\xA',  '\x1A', '\xA',  '\x0',
    '\x0',  '\x0',  '\xD',  'I',    'H',    'D',    'R',    '\x0',  '\x0',
    '\x0',  '\x10', '\x0',  '\x0',  '\x0',  '\x10', '\x8',  '\x6',  '\x0',
    '\x0',  '\x0',  '\x1F', '\xF3', '\xFF', 'a',    '\x0',  '\x0',  '\x0',
    '\x19', 't',    'E',    'X',    't',    'S',    'o',    'f',    't',
    'w',    'a',    'r',    'e',    '\x0',  'A',    'd',    'o',    'b',
    'e',    '\x20', 'I',    'm',    'a',    'g',    'e',    'R',    'e',
    'a',    'd',    'y',    'q',    '\xC9', 'e',    '\x3C', '\x0',  '\x0',
    '\x1',  '\xCD', 'I',    'D',    'A',    'T',    'x',    '\xDA', '\x94',
    '\x93', '9',    'H',    '\x3',  'A',    '\x14', '\x86', '\xFF', '\x5D',
    'b',    '\xA7', '\x4',  'R',    '\xC4', 'm',    '\x22', '\x1E', '\xA0',
    'F',    '\x24', '\x8',  '\x16', '\x16', 'v',    '\xA',  '6',    '\xBA',
    'J',    '\x9A', '\x80', '\x8',  'A',    '\xB4', 'q',    '\x85', 'X',
    '\x89', 'G',    '\xB0', 'I',    '\xA9', 'Q',    '\x24', '\xCD', '\xA6',
    '\x8',  '\xA4', 'H',    'c',    '\x91', 'B',    '\xB',  '\xAF', 'V',
    '\xC1', 'F',    '\xB4', '\x15', '\xCF', '\x22', 'X',    '\x98', '\xB',
    'T',    'H',    '\x8A', 'd',    '\x93', '\x8D', '\xFB', 'F',    'g',
    '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f',    'v',    'f',    '\xDF',
    '\x7C', '\xEF', '\xE7', 'g',    'F',    '\xA8', '\xD5', 'j',    'H',
    '\x24', '\x12', '\x2A', '\x0',  '\x5',  '\xBF', 'G',    '\xD4', '\xEF',
    '\xF7', '\x2F', '6',    '\xEC', '\x12', '\x20', '\x1E', '\x8F', '\xD7',
    '\xAA', '\xD5', '\xEA', '\xAF', 'I',    '5',    'F',    '\xAA', 'T',
    '\x5F', '\x9F', '\x22', 'A',    '\x2A', '\x95', '\xA',  '\x83', '\xE5',
    'r',    '9',    'd',    '\xB3', 'Y',    '\x96', '\x99', 'L',    '\x6',
    '\xE9', 't',    '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',    '\xA7',
    '\xC4', 'b',    '1',    '\xB5', '\x5E', '\x0',  '\x3',  'h',    '\x9A',
    '\xC6', '\x16', '\x82', '\x20', 'X',    'R',    '\x14', 'E',    '6',
    'S',    '\x94', '\xCB', 'e',    'x',    '\xBD', '\x5E', '\xAA', 'U',
    'T',    '\x23', 'L',    '\xC0', '\xE0', '\xE2', '\xC1', '\x8F', '\x0',
    '\x9E', '\xBC', '\x9',  'A',    '\x7C', '\x3E', '\x1F', '\x83', 'D',
    '\x22', '\x11', '\xD5', 'T',    '\x40', '\x3F', '8',    '\x80', 'w',
    '\xE5', '3',    '\x7',  '\xB8', '\x5C', '\x2E', 'H',    '\x92', '\x4',
    '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g',    '\x98', '\xE9',
    '6',    '\x1A', '\xA6', 'g',    '\x15', '\x4',  '\xE3', '\xD7', '\xC8',
    '\xBD', '\x15', '\xE1', 'i',    '\xB7', 'C',    '\xAB', '\xEA', 'x',
    '\x2F', 'j',    'X',    '\x92', '\xBB', '\x18', '\x20', '\x9F', '\xCF',
    '3',    '\xC3', '\xB8', '\xE9', 'N',    '\xA7', '\xD3', 'l',    'J',
    '\x0',  'i',    '6',    '\x7C', '\x8E', '\xE1', '\xFE', 'V',    '\x84',
    '\xE7', '\x3C', '\x9F', 'r',    '\x2B', '\x3A', 'B',    '\x7B', '7',
    'f',    'w',    '\xAE', '\x8E', '\xE',  '\xF3', '\xBD', 'R',    '\xA9',
    'd',    '\x2',  'B',    '\xAF', '\x85', '2',    'f',    'F',    '\xBA',
    '\xC',  '\xD9', '\x9F', '\x1D', '\x9A', 'l',    '\x22', '\xE6', '\xC7',
    '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15', '\x90', '\x7',  '\x93',
    '\xA2', '\x28', '\xA0', 'S',    'j',    '\xB1', '\xB8', '\xDF', '\x29',
    '5',    'C',    '\xE',  '\x3F', 'X',    '\xFC', '\x98', '\xDA', 'y',
    'j',    'P',    '\x40', '\x0',  '\x87', '\xAE', '\x1B', '\x17', 'B',
    '\xB4', '\x3A', '\x3F', '\xBE', 'y',    '\xC7', '\xA',  '\x26', '\xB6',
    '\xEE', '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
    '\xA',  '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X',    '\x0',  '\x27',
    '\xEB', 'n',    'V',    'p',    '\xBC', '\xD6', '\xCB', '\xD6', 'G',
    '\xAB', '\x3D', 'l',    '\x7D', '\xB8', '\xD2', '\xDD', '\xA0', '\x60',
    '\x83', '\xBA', '\xEF', '\x5F', '\xA4', '\xEA', '\xCC', '\x2',  'N',
    '\xAE', '\x5E', 'p',    '\x1A', '\xEC', '\xB3', '\x40', '9',    '\xAC',
    '\xFE', '\xF2', '\x91', '\x89', 'g',    '\x91', '\x85', '\x21', '\xA8',
    '\x87', '\xB7', 'X',    '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N',
    '\x7F', '\x9',  '2',    '\x95', '\xBF', '\x1F', '\xDB', '\xD7', 'c',
    'N',    'b',    't',    '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
    '\xEC', '\x86', '\x2',  'H',    '\x26', '\x93', '\xD0', 'u',    '\x1D',
    '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF', '\x22', 'J',    '\xC3',
    '\x87', '\x0',  '\x3',  '\x0',  'K',    '\xBB', '\xF8', '\xD6', '\x2A',
    'v',    '\x98', 'I',    '\x0',  '\x0',  '\x0',  '\x0',  'I',    'E',
    'N',    'D',    '\xAE', 'B',    '\x60', '\x82',
};

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
    
    fileName_ = PATHSOURCE;
    pthread_mutex_init(&mutex_,NULL);
    // loop_->queueInLoop(bind(&HttpServer::setHandlers, this));
    //   channel_->setReadHandler(bind(&HttpServer::handleRead, this));
    //   channel_->setWriteHandler(bind(&HttpServer::handleWrite, this));
    //   channel_->setConnHandler(bind(&HttpServer::handleConn, this));
}

 HttpServer::~HttpServer() 
 { pthread_mutex_destroy(&mutex_);}

void HttpServer::initSqlConnPool(string user, string passWord, string DBName, int maxConn, 
                int closeLog = 1,string url = "localhost",int port = 3306)
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
    int num_fields = mysql_num_fields(result);

    //返回所有字段结构的数组
    MYSQL_FIELD *fields = mysql_fetch_fields(result);

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
    
}



