#include <iostream>
#include <iomanip>
#include <curl/curl.h>
#include <string.h> 
#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "call_api.h"

// Define a new type of struct for storing the response string, where the 
// member string points to the string, and the member size stores the size of 
// the string.

int call_api(Response &response)
{
  // curl stores the curl handle, result stores the return value after calling
  // the curl_easy_perform() function
   CURL *curl;
   CURLcode result;
   
   // Initialize the curl easy interface session, output an error message and 
   // exit the program with an error status if the initialization fails.
   curl=curl_easy_init();
   if(curl==NULL)
   {
   
    fprintf(stderr,"HTTP REQUEST FAILED\n");
    return -1;
   }
   
    // Declare and initialize a Response struct.  We have the member string point
    // to a block of memory 1 byte in size, we'll use realloc() to enlarge the 
    // block of memory so that it's large enough to store the response string.
     // Response response;
      response.string =(char *)malloc(1);
    if(response.string==NULL)
    {
        printf("filed to allocate memory for sting");
        return -1;
    }
    response.size = 0;

   // Set option to have request made to POSTMAN EXCHANGE RATE API
   std::string baseCurrency = response.code1;
   std::string url="https://api.exchangerate-api.com/v4/latest/"+baseCurrency;
   curl_easy_setopt(curl,CURLOPT_URL,url.c_str());

    // Set option to have the function write_chunk called repeatedly by 
  // curl_easy_perform() for each chunk of the string.

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk); 
  
  // Set option to have the memory address of the Response struct response 
  // passed to the write_chunk function (as the 4th argument) each time it is
  // called by curl_easy_perform().
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);
  
  // Perform the HTTP request, curl_easy_perform() will repeatedly call the 
  // write_chunk function with a chunk of the response string.
    
   result=curl_easy_perform(curl);
   if(result != CURLE_OK)
   {
    fprintf(stderr,"Error: %s\n",curl_easy_strerror(result));
    return -1;
   }
  curl_easy_cleanup(curl);
  
  return 0;
}

// Fuction that used to write a retun API response into string.
size_t write_chunk(void *data, size_t size, size_t nmemb, void *userdata)
{
  size_t real_size = size * nmemb; 
  Response *response = (Response *) userdata; 
  char *ptr = (char *)realloc(response->string, response->size + real_size + 1);
  if (ptr == NULL)
  {
    // return 0;
    return CURL_WRITEFUNC_ERROR;  
  }
  response->string = ptr;
  memcpy(&(response->string[response->size]), data, real_size);
  response->size += real_size;
  return real_size;
}