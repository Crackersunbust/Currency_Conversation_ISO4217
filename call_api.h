#ifndef CALL_API_H
#define CALL_API_H

#include <iostream>
#include <iomanip>
#include <curl/curl.h>
#include <string.h> 
#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <vector>

typedef struct
{
char* string;
size_t size;

char code1[4],code2[4];

}Response;



size_t write_chunk(void *data,size_t size,size_t nmemb,void *userdata);  //Decleration of size_chunk
int call_api(Response &response); 

#endif // CALL_API_H
