#include <iostream>
#include <iomanip>
#include <curl/curl.h>
#include <string.h> 
#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "call_api.h"

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

