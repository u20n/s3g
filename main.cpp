#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <vector>
#include <map>

#include "config.h"

// assimilates **all** types into a string
template<typename... T>
std::string strf(T... t) {
  std::stringstream s;
  ((s << t), ...);
  return s.str();
}

// produces a tagged string with signature of type
// e.g. tag("p1", "a", 10, 0.5) => <p1>a100.5</p1>\n
template<typename... T>
std::string tag(const char* type, T... t) {
  return std::string(strf("<",type,">",(t)...,"</",type,">\n"));
}

std::string read(std::string fp) {
  std::ifstream f(fp);
  return std::string(
      (std::istreambuf_iterator<char>(f)),
      (std::istreambuf_iterator<char>())
    );
}

void /**over*/write(std::string fp, std::string c) {
  std::ofstream f(fp);
  f << c;
  f.close();
}

// sanitise characters to html-safe
std::string sanitise(char c) {
  switch (c) {
    case '&':
      return("&amp;");
    case '>':
      return("&lt;");
    case '<':
      return("&gt;");
    default:
      std::string r; r+=c;
      return r;
  }
}

void generate(std::filesystem::path fp) {
  std::string raw = read(fp.c_str());
  std::string html("<!DOCTYPE html>\n<html>\n<head>"); 
  // == header ==
  // find the first '-'
  // - go until line break
  // - read ':' pairs into a map
  // - return index of first line of content
 
  //[TODO] : Current Focus
  std::string head = raw.substr(4, raw.find("---", 4)-5);
  std::map<std::string, std::vector<std::string>> header;
  unsigned int i = 0; while(i < head.size()) { 
    size_t j = head.find(':', i); if (j == std::string::npos) break; 
    std::string k = head.substr(i, j-i); // account for space taken by colon
    std::string v = head.substr(j+2, head.find('\n', j)-j+2);
    std::cout << k << ':' << v << '\n'; // DEBUG
    i = (j+1)+(v.size()); // account for the space between the colon and the value, and for the newline character after the value (size is index'd on 1)
  }
  // [TODO] apply meta data to html

  // == body ==
  // loop over the entire file (from index provided above)
  // - translate markdown to html
  // - apply default style
  // - apply any qualifiying styles 
  
  for (unsigned int i=head.size()-1; i<raw.size(); i++) {
    const char c = raw.at(i); 
    if (
        c == '\\' && 
        raw.at(i+1) 
          != ('\r' || '\n')
        ) {
      html.append(sanitise(raw.at(i+1))); i++; continue; // escape
    }

    // check for special characters
    switch (c) {
      case '$':
        {
          // mathjax (pain) TODO
          // skip ahead
          const char* match = (raw.at(i+1) == '$') ? "$$" : "$";
          i+=raw.find(match, i)-i;
          break;
        }
      case '>': // block qoutes (TODO)
        break;
      case '#': // headers
        {
          unsigned int s = raw.find(' ', i)-i; // determine size of header
          unsigned int ii = i+s+1; // e.g. (#### abcdefg), would be index of 'a'
          std::string e = raw.substr(ii, raw.find("\n", ii)-ii); // content 
          html.append(tag(strf("h",s).c_str(), e.c_str()));
          i+=(s+e.size());
          break;
        }
      case '[': // link
        break;
      case '*': // italic (or) bold
        {
          // [TODO]: doesn't handle nested bold+italics
          const char* type; unsigned int ii;/**index*/ const char* mc; // match-case
          if (raw.at(i+1) == '*') { // bold
            ii = i+2; type = "b"; mc = "**";
          } else { // italic
            ii = i+1; type = "i"; mc = "*";
          }
          std::string e = raw.substr(ii, raw.find(mc, ii)-ii);
          html.append(tag(type, e.c_str())); 
          i+=((ii-i)*2+e.size());
          break;
        }
      default:
        // TODO: paragraph typing, e.g. <p> for normal text
        html.append(sanitise(c));
    }
  }
  write(
      strf(
        OUT_DIR,
        "/", fp.stem().c_str(),
        ".html"
        ),
      html
    ); 
}

int main(void) {
  for (
      auto const& e: 
      std::filesystem::directory_iterator{
      std::filesystem::path(IN_DIR)
      }
    ) { 
      if (e.path().extension() == ".md") {
        generate(e.path());
        std::cout << "built " << e.path().c_str() << '\n'; // DEBUG
      }
    }

  return 0; 
}
