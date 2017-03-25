#include <iostream>
#include <string>
#include <ctime>
#include <sys/time.h>

using namespace std;
string getPreLog(string file, int line)
{
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char   tmbuf[64], buf[64];

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm   = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
    snprintf(buf, sizeof buf, "%s.%ld (%-20s, %5d)", tmbuf, tv.tv_usec, file.c_str(), line);

    string rtn(buf);
    return rtn;
}

int main( int argc, char* argv[] )
{

    cout << getPreLog(__FILE__, __LINE__) << endl;
    cout << getPreLog(__FILE__, __LINE__) << endl;
    cout << getPreLog(__FILE__, __LINE__) << endl;
    cout << getPreLog(__FILE__, __LINE__) << endl;
    cout << getPreLog(__FILE__, __LINE__) << endl;
    cout << getPreLog(__FILE__, __LINE__) << endl;
    cout << getPreLog(__FILE__, __LINE__) << endl;
    cout << getPreLog(__FILE__, __LINE__) << endl;
    return 0;
}
