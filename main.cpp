#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

#include "config.h"

/** QoL strf */
template<typename... T>
std::string strf(T... t) {
  std::stringstream s;
  ((s << t), ...);
  return s.str();
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

void generate(std::filesystem::path fp) {
  std::string html, raw = read(fp.c_str()); 
  // == header ==
  // find the first '-'
  // - go until line break
  // - read ':' pairs into a map
  // - return index of first line of content
  // TODO: apply headers???? (some sort of html templating?)

  // == body ==
  // loop over the entire file (from index provided above)
  // - translate markdown to html
  // - apply default style
  // - apply any qualifiying styles 
  for (unsigned int i=0; i<raw.size(); i++) {
    const char c = raw.at(i); 
    if (
        c == '\\' && 
        raw.at(i+1) 
          != ('\r' || '\n')
        ) {
      html += raw.at(i+1); i++; continue; // escape
    }

    switch (c) {
      case '$':
        {
          // mathjax (pain) TODO
          // skip ahead
          const char* match = (raw.at(i+1) == '$') ? "$$" : "$";
          i+=raw.find(match, i)-i;
          break;
        }
      case '-':        
        break;
      case '#': // headers
        {
          unsigned int s = raw.find(' ', i)-i; // determine size of header
          unsigned int ii = i+s+1; // e.g. (#### abcdefg), would be index of 'a'
          std::string e = raw.substr(ii, raw.find("\n", ii)-ii); // content 
          html.append(
              strf(
                "<h", s, ">", 
                e.c_str(), 
                "</h", s, ">\n"
                )
            );
          i+=(s+e.size());
          break;
        }
      case '[': // link
        break;
      case '*': // italic (or) bold
        {
          // [TODO]: doesn't handle nested bold+italics
          char type; unsigned int ii;/**index*/ const char* mc; // match-case
          if (raw.at(i+1) == '*') { // bold
            ii = i+2; type = 'b'; mc = "**";
          } else { // italic
            ii = i+1; type = 'i'; mc = "*";
          }
          std::string e = raw.substr(ii, raw.find(mc, ii)-ii);
          html.append(
              strf(
                "<", type, ">",
                e.c_str(),
                "</", type, ">"
              )
            );
          i+=((ii-i)*2+e.size());
          break;
        }
      default:
        // TODO: paragraph typing, e.g. <p> for normal text
        html += c;
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

  // load items
  return 0; 
}
