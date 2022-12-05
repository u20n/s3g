#ifndef _H_CONFIG
#define _H_CONFIG
#include <unordered_set>
#include <string>

std::string OUT_DIR = "./out";
std::string IN_DIR = "./posts";
std::string TEMPLATE_DIR = "./templates";

char VAR_IND = '$';
char SCHEME_IND = '#';

// ignore double new lines
bool I_DNL = false;

// indexed meta-fields
std::unordered_set<std::string> INDEXED {
  "tags",
  "status",
  "published" 
};

#endif
