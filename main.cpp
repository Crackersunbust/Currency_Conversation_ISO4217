#include <iostream>
#include <iomanip>
#include <curl/curl.h>
#include <string.h> 
#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <vector>

/*>>>>>>>>>>>>>Version 1<<<<<<<<<<<<<<<<<<<*/

// Define a new type of struct for storing the response string, where the 
// member string points to the string, and the member size stores the size of 
// the string.
typedef struct
{
char* string;
size_t size;

char code1[4],code2[4];

}Response;

size_t write_chunk(void *data,size_t size,size_t nmemb,void *userdata);

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

// Extract the value corresponding to a key like "USD" from the string
double get_rate(const char* json_str, const char* key) 
{
    char *pos = strstr(json_str, key);
    if (!pos) return -1;

    // Find the colon after the key
    pos = strchr(pos,':');
    if (!pos) return -1;
    pos++; // move past colon

    // Read the value
    double value = 0.0;
    sscanf(pos, "%lf",&value);
    return value;
}

// Extract all country codes from the rates block
std::vector<std::string> extract_country_codes(const char* json_str)
 {
    std::vector<std::string> codes;

    const char* rates_start = strstr(json_str, "\"rates\":");
    if (!rates_start) return codes;

    const char* brace_start = strchr(rates_start, '{');
    const char* brace_end = strchr(brace_start, '}');
    if (!brace_start || !brace_end) return codes;

    std::string rates_block(brace_start + 1, brace_end); // content inside {}

    size_t pos = 0;
    while (pos < rates_block.size()) {
        // find opening quote
        size_t quote1 = rates_block.find('"', pos);
        if (quote1 == std::string::npos) break;
        size_t quote2 = rates_block.find('"', quote1 + 1);
        if (quote2 == std::string::npos) break;

        std::string code = rates_block.substr(quote1 + 1, quote2 - quote1 - 1);
        codes.push_back(code);

        pos = quote2 + 1;
    }

    return codes;
}

  bool searchString(const std::string &target, const std::vector<std::string> &country_codes) 
  {
    for (const auto &code : country_codes) {
        if (code == target) {
            return true; // found
        }
    }
    return false; // not found
}

int main()
{
    Response response;
  
    //printf("Enter the Conversion Amount Currency to Converted to Currency\n");
    printf("Enter the Amount Currency code:");
    scanf("%s",response.code1);

    if(call_api(response)!=0)
    {
     printf("API Failed Accquired...\nPlease try the network status and try again\n");
    }
    
    std::vector<std::string> country_codes = extract_country_codes(response.string);

    while(searchString(response.code1,country_codes)==false) 
    {
      printf("Enter the Valid country code\n Try again..\n");
      printf("Enter the Amount Currency code:");
      scanf("%s",response.code1);
    }

    printf("Enter the Converted Currency code:");
    scanf("%s",response.code2);
    bool check= (strcmp(response.code1,response.code2)==0);
    while((searchString(response.code2,country_codes)==false) || (check == 1)) 
    {
      printf("Enter the Valid country code\n Try again..\n");
      printf("Enter the Converted Currency code:");
      scanf("%s",response.code2);
    }
    double currency_1 = get_rate(response.string, response.code1);
    double currency_2 = get_rate(response.string, response.code2);
    double rate=0;
    double total=0;

    printf("1 %s is Equal to %f %s\n",response.code1,currency_2,response.code2);
    while(1)
    {
    printf("Enter the Rate for Conversion:");
    scanf("%lf",&rate);
    total=(rate*currency_2);
    //printf("%f",total);
    printf("%f %s is equal to %f %s.\n",rate,response.code1,total,response.code2);
    }
    free(response.string);
    return 0; 
}

