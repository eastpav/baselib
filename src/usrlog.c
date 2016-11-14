/* usrlog.c - LOG source file */
/* 修改记录
	2012-3-8 程梦
	(1)增加内部全局变量gLogObj，在写入日志时，如果输入指针为空，则使用该变量
	(2)重复日志内容延迟添加
	(3)日志文件大小控制
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "usrlog.h"

// Forward declaration
static int file_get_line( FILE *f, char *s, int n);
static void file_put_line( FILE *f, char *line );


#define MAX_LOG_FILE_LEN (2*1024*1024)
#define MAX_LOG_MSG_LEN  (1024)
static unsigned int log_msg_cnt = 0;
static char last_log_msg[MAX_LOG_MSG_LEN] = {0};

// Log object definition
typedef struct log_obj
{
	char fname[256];
	FILE *fp;
	off_t rdpos;
	int   level;
}LogObj;

// Global Log object definition
static LogObj *g_pLogObj = NULL;

// Create a LOG object.
// RETURNS: Handle to LOG object, NULL if failed.
void* Log_create( LogAttribute *pAttr )
{
	LogObj *hndl;
	FILE   *fp = NULL;

	if (pAttr == NULL) return NULL;

	fp = fopen(pAttr->name, "a+");
	if (fp == NULL) return NULL;

	hndl = (LogObj*)malloc(sizeof(LogObj));
	if(hndl == NULL)
	{
		fclose(fp);
		return NULL;
	}
	
	strncpy(hndl->fname,pAttr->name,256);
	hndl->fp    = fp;
	hndl->rdpos = 0;
	hndl->level = pAttr->level;

	g_pLogObj = hndl;
	log_msg_cnt = 0;
	
	memset(last_log_msg,0,MAX_LOG_MSG_LEN);
	
	return (void*)hndl;
}

// Delete a LOG object.
// RETURNS: 0 - success, -1 if error.
int Log_delete(void* handle)
{
	LogObj *hndl = (LogObj*)handle;

	if (hndl == NULL) {
		if (g_pLogObj == NULL) {
			return -1;
		} else {
			hndl = g_pLogObj;
		}
	}
	
	if (hndl->fp != NULL)
	{
		fclose(hndl->fp);
	}

	free((char*)hndl);
	g_pLogObj = NULL;
	
	return 0;
}

// Write to log file.
// RETURNS: 0 - OK, -1 if error.
int Log_write( void* handle, int usrlog, int level, char* pBuf )
{return 0;
	char tmpBuf[512];
	char levelstr[2] = {0, 0};
	time_t result;
	struct tm *pTm;
	LogObj* hndl = (LogObj*)handle;
	FILE* fp = 0;

	if (hndl == NULL) {
		if (g_pLogObj == NULL) {
			return -1;
		} else {
			hndl = g_pLogObj;
		}
	}	
	
	if (hndl->fp == NULL) return -1;
	if ((hndl->level & level) != level)
	{
		return 0;
	}
	
	// 如果与上一次添加的日志信息内容一致，则只计数，不添加
	if ( 0 == strncmp(last_log_msg, pBuf, MAX_LOG_MSG_LEN) ) {
		log_msg_cnt++;
		return 0;
	}
	
	if(level == LOG_LEVEL_MSG)
		levelstr[0] = 'M';
	else if(level == LOG_LEVEL_WRN)
		levelstr[0] = 'W';
	else
		levelstr[0] = 'E';

	result = time(NULL);
	pTm = localtime(&result);
	if (pTm == NULL) return -1;

	if ( log_msg_cnt ) { // 有重复日志，先写入再写新信息
		sprintf(tmpBuf, "%d-%02d-%02d %02d:%02d:%02d same above %d times",
			pTm->tm_year + 1900, pTm->tm_mon + 1,
			pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec, log_msg_cnt);
		//fseek( hndl->fp, 0, SEEK_END);
		file_put_line( hndl->fp, tmpBuf );
		fflush(hndl->fp);

		memset(last_log_msg,0,MAX_LOG_MSG_LEN);
		log_msg_cnt=0;
	}
	
	sprintf(tmpBuf, "%d-%02d-%02d %02d:%02d:%02d %s %s %s",
		pTm->tm_year + 1900, pTm->tm_mon + 1,
		pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec, 
		usrlog ? "U" : "K", levelstr, pBuf);
	//fseek( hndl->fp, 0, SEEK_END);
	file_put_line( hndl->fp, tmpBuf );
	fflush(hndl->fp);
	fsync(fileno(hndl->fp));
	
	strncpy(last_log_msg,pBuf,MAX_LOG_MSG_LEN); // 更新最后一次提交日志信息
	
	// 判断文件大小，如果超过制定数量就将写指针跳回文件最开始
	fseek( hndl->fp, 0, SEEK_END);
	if ( ftell( hndl->fp ) > MAX_LOG_FILE_LEN ) {
		fclose( hndl->fp );
		
		fp = fopen(hndl->fname, "w+");
		if (fp == NULL) {
			hndl->fp = NULL;
			return -1;
		} else {
			hndl->fp = fp;
		}
	}
	
	return 0;
}

// Read one record from log file.
// -1 - ERROR, 0 - OK, 1 - EOF
int Log_read( void* handle, char* pBuf, int len )
{
	LogObj* hndl = (LogObj*)handle;
	int status = 0;

	if (hndl == NULL) {
		if (g_pLogObj == NULL) {
			return -1;
		} else {
			hndl = g_pLogObj;
		}
	}
	
	if (hndl->fp == NULL) return -1;
	
	fseek( hndl->fp, hndl->rdpos, SEEK_SET);
	status = file_get_line( hndl->fp, pBuf, len);
	hndl->rdpos = ftell(hndl->fp);
	
	return status;
}

#define LF 10
#define CR 13

/* get one line from F. */
static int file_get_line( FILE *f, char *s, int n) 
{
    register int i=0;

    while(1) 
    {
        s[i] = (char)fgetc(f);
        
        if( s[i] == CR) 
        {
        	s[i] = fgetc(f);
        }
        
        if( (s[i] == 0x4) || (s[i] == LF) || (i == (n-1))) 
		{
            s[i] = '\0';
            
            return (feof(f) ? 1 : 0);
        }
        
        ++ i;
    }
}

/* put one line to F from buffer LINE. */
static void file_put_line( FILE *f, char *line ) 
{
    int x;

    for(x=0; line[x]; x++) 
    {
		fputc(line[x], f);
	}

    fputc('\n',f);
}

// End of file
