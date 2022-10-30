#ifndef _H_CONFIG
#define _H_CONFIG
#include <unordered_set>
#include <string>

std::string OUT_DIR = "./out";
std::string IN_DIR = "./posts";
std::string TEMPLATE_DIR = "./templates";

char VAR_IND = '$';
char SCHEME_IND = '#';

bool I_DNL = false; // ignore double newlines

std::unordered_set<std::string> INDEXED { // indexed meta-fields
  "tags",
  "status",
  "published" 
};

#endif
