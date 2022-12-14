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

// produces a tagged string with signature of type, and optional arguments; e.g. href
std::string tag(std::string type, std::string content, std::string args="") {
  if (!args.empty()) args = std::string(" "+args);
  return std::string(strf("<",type,args,">",content,"</",type,">"));
}

std::string read(std::string fp) {
  std::ifstream f(fp);
  return std::string(
      (std::istreambuf_iterator<char>(f)),
      (std::istreambuf_iterator<char>())
    );
}

void /**over*/write(std::string fp, std::string c) {
  std::ofstream f(fp); f << c; f.close();
}

// sanitise characters to html-safe
std::string sanitise(char c) {
  switch (c) {
    case '&':
      return("&amp;");
    case '>':
      return("&gt;");
    case '<':
      return("&lt;");
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

std::string parse(std::string s) {
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
      case '`': // Code Block
        {
          size_t j = (s.at(i+2) == c) ? 3 : 1;
          std::string code = s.substr(i+j, s.find(c, (i+j))-(i+j)); 
          r.append(tag("code", sanitise(code)));
          i += code.size() + (j*2) - 1;
          break;
        }
      case '$': // LaTex
        {
          // (this is similar to our b/i parsing)
          // we determine if this is a multi or single line [^a]
          // - this is represented as `j` by the count of `$`
          // we can use this information to:
          // - get the internal string [^b]
          // - get the corresponding LaTex indicator [^c]
          // - move `i` ahead [^d]
          std::string e; size_t j;
          j = (s.at(i+1) == '$') ? 2 : 1; // [a]
          e = s.substr(i+j, s.find(std::string(j, s.at(i)), i+j) - (i+j)); // [b]
          r.append(
              strf(
                (j < 2) ? "\\(" : "\\[", // [c]
                e,
                (j < 2) ? "\\)" : "\\]" // [c]
                )
            );
          i += (j*2) + e.size() - 1; // [d]
        }
        break;
      case '\n': // newline
        if (I_DNL && s.at(i-1) == '\n') continue;        
        r.append("\n<br>\n"); 
        break; 
      case '-': // [TODO] lists
        if (s.at(i+1) == ' ') { // list?
          break;
        }
      case '_':
        if (s.at(i+2) == c) {
          r.append("<hr />");
          i += 2;
          break;
        } continue;
      case '>': // block qoutes
        {
          size_t end = s.find("\n", i+2)-(i+2);
          std::string inner = parse(s.substr(i+2, end)); // parse inter-qoute content
          r.append(tag("qoute", inner));
          i += (end + 1); // account for extra newline
        }
        break;
      case '#': // headers
        {
          size_t h = s.find(' ', i)-i; // determine size of header
          size_t c = i+h+1; // content index, e.g. (#### abcdefg), would be index of 'a'
          std::string e = s.substr(c, s.find("\n", c)-c); // content 
          r.append(tag(strf("h",h), e));
          i += (h+e.size());
        }
        break;
      case '[': // links
        { 
          size_t alias_len = s.find(']', i+1);
          if (s.at(alias_len-1) == '\\') alias_len = s.find(']', alias_len+1); // check for escaped brackets 
  
          if (s.at(i+1) == '^') { // referential endnote
            std::string rurl, alias = s.substr(i+2, (alias_len - 2) - i); // (pos of bracket - size of bracket) - initial index 
            if (
                s.at(alias_len + 1) == ':' // check for source
              ) { // source 
              rurl = strf("id=\"", alias, "\"");
              i = alias_len+1; // account for ':'
            } else { // reference
              rurl = strf("href=\"#", alias, "\""); // link to source
              i = alias_len;
            }
            r.append(tag("sup", tag("a", sanitise(alias), rurl)));
          } else {
            std::string alias = s.substr(i+1, (alias_len - 1) - i);
            std::string link = s.substr(alias_len+2, s.find(')', alias_len)-alias_len-2); // (pos of para - pos bracket) - size of para(s)
            std::string rurl = strf("href=\"", link, "\""); // hyperlink 
            i = (alias_len + link.size()+2); // account for '(' and ')'
            r.append(tag("a", sanitise(alias), rurl));
          }
        } 
        break;
      case '*': // italic (or) bold
        {
          // This is completely over-engineered, but it was bugging me
          // we determine if this is a bold or italicised string
          // - this is represented as `j` by the count of `*` [^a]
          // we can use this information to:
          // - get the internal string [^b]
          // - get the corresponding html type [^c]
          // - move `i` ahead [^d]
          std::string e; size_t j;
          j = (s.at(i+1) == '*') ? 2 : 1; // [a]
          e = s.substr(i+j, s.find(std::string(j, s.at(i)), i+j) - (i+j)); // [b]
          r.append(
              tag(
                (j < 2) ? "i" : "b", // [c]
                parse(e) // enabled nested b/i/*
              )
            );
          i += (j*2) + e.size() - 1; // [d]
        }
        break;
      default: 
        r.append(sanitise(c));
    }
  }
  return r;
}

void generate(std::filesystem::path fp) {
  std::string raw = read(fp.string()); 
  // == header ==  
  std::string head = raw.substr(4, raw.find("---", 4)-5);
  std::map<std::string, std::vector<std::string>> header;

  for (unsigned int i=0; i<head.size();) {
    size_t j = head.find('\n', i); 
    std::string r = head.substr(i, j-i);
    std::string k = r.substr(0, r.find(':'));
    std::string v = r.substr(r.find(':')+2, r.size());
 
    if (v.find(',') != std::string::npos) {
      size_t ii = 0; while(ii+1 < v.size()) {
        size_t e = (v.find(',', ii) != std::string::npos) ? v.find(',', ii) : v.size(); 
        header[k].push_back(v.substr(ii, e-ii));
        ii = e+2;
      }
    } else header[k] = {v}; 
    if (j >= head.size()) break;
    i = j+1;
  } 

  // == templating == 
  std::string temp = read(strf(TEMPLATE_DIR, "/", (header.count("type")) ? 
    header["type"].at(0) : // pull given 
    "default" // pull default
    , ".html"));
  std::string build; // actual html

  // == pull variable/scheme ==
  auto __search = [](std::string src, char delim, unsigned int i=0) {
    return src.substr(i+1, src.find(delim, i+1)-(i+1));
  };

  // == parse variables/schemes == 
  for (unsigned int i=0; i<temp.size(); i++) { 
    if (temp.at(i) == SCHEME_IND) { 
      std::string scheme = __search(temp, SCHEME_IND, i); // pull scheme
      std::string variable = __search(scheme, VAR_IND, scheme.find(VAR_IND)); // pull variable 
      
      for (unsigned int j=0; j<header[variable].size(); j++) { // replicate for each value in the field
        std::string __scheme = scheme; // make an instance of the scheme 
        for (unsigned int l=0; l<__scheme.size(); l++) { 
          if (__scheme.at(l) == VAR_IND) {
            std::string variable = __search(__scheme, VAR_IND, l);
            __scheme.erase(l, variable.size()+2);
            __scheme.insert(l, header[variable].at(j)); 
            l += (variable.size()+1);
          }
        } 
        build.append(strf(__scheme, '\n')); // push
      }  
      i += scheme.size()+1; // skip over completed scheme 
    } else if (temp.at(i) == VAR_IND) {
      std::string variable = __search(temp, VAR_IND, i); 
      if (variable == "text") { // body of text!
        build.append(parse(raw.substr(head.size()+7))); // account for header denotion (index'd on 0)
      } else if (header.count(variable)) build.append(header[variable].at(0));
      i += (variable.size()+1); // skip over variable
    } else build += temp.at(i);
  } 

  write(
      strf(
        OUT_DIR,
        "/", fp.stem().string(),
        ".html"
        ),
      build
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
        std::cout << e.path().string(); // DEBUG
        try {
          generate(e.path());
          std::cout << " -> built \n"; // DEBUG 
        } catch (std::exception& e) {
          std::cout << " -> failed\n"; // DEBUG
          std::cout << e.what() << '\n'; // DEBUG
          std::cout << std::string(10, '-') << '\n'; // DEBUG
        }
      }
    }
  return 0; 
}
