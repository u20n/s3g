#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <vector>
#include <map>
#include <cstring>

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
std::string sanitise(std::string s) {
  std::string r;
  for (auto& c: s) {
    if (c == '\\') continue; // skip the escape character
    r.append(sanitise(c));
  }
  return r;
}

std::string translate(std::string s) {
  std::string r;
  for (unsigned int i=0; i<s.size(); i++) {
    const char c = s.at(i); 
    if (
        c == '\\' && 
        s.at(i+1) 
          != ('\r' || '\n')
        ) {
      r.append(sanitise(s.at(i+1))); i++; continue; // escape
    }

    // check for special characters
    switch (c) {
      case '$': // mathjax (pain) TODO 
        break;
      case '\n': // newline
        if (s.at(i-1) == '\n') continue; // ignore double newlines
        r.append("\n<br>\n");
        break;
      case '>': // block qoutes
        {
          size_t end = s.find("\n", i+2)-(i+2);
          std::string inner = translate(s.substr(i+2, end)); // parse inter-qoute content
          r.append(tag("qoute", inner));
          i += end+1; // account for extra newline
        }
        break;
      case '#': // headers
        {
          unsigned int h = s.find(' ', i)-i; // determine size of header
          unsigned int ii = i+h+1; // e.g. (#### abcdefg), would be index of 'a'
          std::string e = s.substr(ii, s.find("\n", ii)-ii); // content 
          r.append(tag(strf("h",h).c_str(), e.c_str()));
          i+=(h+e.size());
        }
        break;
      case '[': // link
        {
          if (s.at(i+1) == '^') { // endnote [TODO]

          } else { // url
            size_t alias_len = s.find(']', i+1);
            if (s.at(alias_len-1) == '\\') alias_len = s.find(']', alias_len+1); // check for escaped brackets
            std::string alias = s.substr(i+1, alias_len-1-i); // (pos of bracket - size of bracket) - initial index
            size_t url_len = s.find(')', alias_len);
            std::string url = s.substr(alias_len+2, url_len-alias_len-2); // (pos of para - pos bracket) - size of para(s) 
            std::cout << sanitise(alias) << ':' << url << '\n'; // DEBUG
            r.append(strf("<a href=", url, "\">", sanitise(alias), "</a>"));
            i+=(alias.size()+url.size()+3); // content, [,],(,) => size is indexed on 1, so we only add 3
          }
        }
        break;
      case '*': // italic (or) bold
        {
          // [TODO]: doesn't handle nested bold+italics
          const char* type; unsigned int ii;/**index*/ const char* mc; // match-case
          if (s.at(i+1) == '*') { // bold
            ii = i+2; type = "b"; mc = "**";
          } else { // italic
            ii = i+1; type = "i"; mc = "*";
          }
          std::string e = s.substr(ii, s.find(mc, ii)-ii);
          r.append(tag(type, e.c_str())); 
          i+=((ii-i)*2+e.size());
        }
        break;
      default:
        // TODO: paragraph typing, e.g. <p> for normal text
        r.append(sanitise(c));
    }
  }
  return r;
}

void generate(std::filesystem::path fp) {
  std::string raw = read(fp.c_str());
  std::string html("<!DOCTYPE html>\n<html>\n<head>"); 
  // == header == 
  // - read ':' pairs into a map
  // - return index of first line of content
 
  //[TODO] : fix
  std::string head = raw.substr(4, raw.find("---", 4)-5);
  std::map<std::string, std::vector<std::string>> header;
  unsigned int i = 0; while(i < head.size()) { 
    size_t j = head.find(':', i); if (j == std::string::npos) break; 
    std::string k = head.substr(i, j-i); // account for space taken by colon
    std::string v = head.substr(j+2, head.find('\n', j)-j+2);
    //std::cout << k << ':' << v << '\n'; // DEBUG
    i = (j+1)+(v.size()); // account for the space between the colon and the value, and for the newline character after the value (size is index'd on 1)
  }
  // [TODO] apply meta data to html

  // == body ==
  // loop over the entire file (from index provided above)
  // - translate markdown to html
  // - apply default style
  // - apply any qualifiying styles 
  html.append(translate(raw.substr(head.size()-1)));
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
