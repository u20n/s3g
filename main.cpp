#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <map>
#include <array>

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

namespace parse {
  std::string _tag(const char* type, std::string content) {
    return strf(
      "<", type, ">",
      content,
      "</", type, ">"
      );
  }
  std::map<
    std::string, 
    std::string
    > _header(
      std::string raw, 
      std::map<
      std::string, 
      std::string> _m = {}
      ) { 
      // [TODO], this doesn't ~~accurately~~ parse arrays
    auto d = raw.find(':');
    auto k = raw.substr(0, d);
    auto _vd = raw.find('\n');
    auto v = raw.substr(d+1, _vd-1);
    _m[k] = v; 
    if (_vd == 0) return _m;
    _header(raw.substr(_vd));
  }
}
namespace generate {
  std::array<std::string, 2> // 0 is head, 1 is unparsed body
  head(std::filesystem::path fp) {
    std::string head, raw = read(fp.c_str()); 
    // == header ==
    // find the first '-'
    // - go until line break
    // - read ':' pairs into a map
    // - return index of first line of content 
    std::string skv(raw.substr(3/** ---' '*/, raw.find("---", 3)-3));

    // translate to map
    std::map<std::string, std::string> cfg_map = parse::_header(skv); 
    // [TODO] this is where we'd pass the map to a rule form (see note below)
    std::string body = raw.substr(raw.find("---", 3)+3);
    // [TODO] move config to a seperate file, to make custom rules easier to implement
    head.append("<link ref=\"stylesheet\" href=\"./default.css\">"); // PATCH
    
    return {head, body}; 
  }
  std::string body(std::string raw) {
    std::string body; 
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
        body += raw.at(i+1); i++; continue; // escape
      }

      switch (c) {
        case '\n': // newline
          body.append("<br>");
          break;
        case '$':
          {
            // mathjax (pain) TODO
            // skip ahead
            const char* match = (raw.at(i+1) == '$') ? "$$" : "$";
            i+=raw.find(match, i)-i;
            break;
          }
        case '-': // horizontal line
          if (raw.at(i+3) == '-') {
            body.append("<hr>");
            i+=3;
          }
          break;
        case '#': // headers
          {
            unsigned int s = raw.find(' ', i)-i; // determine size of header
            unsigned int ii = i+s+1; // e.g. (#### abcdefg), would be index of 'a'
            std::string e = raw.substr(ii, raw.find("\n", ii)-ii); // content 
            body.append(parse::_tag(strf("h",s).c_str(), e.c_str()));
            i+=(s+e.size());
            break;
          }
        case '[': // link (or citation) [TODO]
          break;
        case '_': // italic 
        case '*': // italic (or) bold
          {
            // [TODO]: doesn't handle nested bold+italics
            const char* type, *mc/**match-case*/; unsigned int ii;/**index*/
            if (raw.at(i+1) == '*') { // bold
              ii = i+2; type = "b"; mc = "**";
            } else { // italic
              ii = i+1; type = "i"; mc = &c;
            } 
            std::string e = raw.substr(ii, raw.find(mc, ii)-ii);
            body.append(parse::_tag(type, e.c_str()));
            i+=((ii-i)*2+e.size());
            break;
          }
        default:
          // TODO: paragraph typing, e.g. <p> for normal text
          body += c;
      }
    }
    return body;
  }
}
void create(std::filesystem::path fp) {
  std::array<std::string, 2> h_pair = generate::head(fp);
  std::string body = generate::body(h_pair[1]);
  write(
      strf(
        OUT_DIR,
        "/", fp.stem().c_str(),
        ".html"
        ),
      strf(
        "<!DOCTYPE html>\n",
        "<html>\n<head>\n",
        h_pair[0],
        "</head>\n<body>\n",
        body,
        "</body>\n</html>"
        )
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
        create(e.path());
        std::cout << "built " << e.path().c_str() << '\n'; // DEBUG
      }
    }

  // load items
  return 0; 
}
