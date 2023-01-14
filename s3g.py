import sys
import getopt
import subprocess

version = "2.0.1"

# pull args: targeted file + flags/options
# FIXME: This fails with the -p flag; we won't have a file name to parse ...
filename = ""
try: 
    flag_num = len(sys.argv) - 2 # remove the command name, and the file name (at the end)
    opts, args = getopt.getopt(sys.argv[1:flag_num+1], "t:c:pxivs")
    filename = sys.argv[flag_num+1]
except:
    exit(1)

# cmd args
template_dir = "."
cache_file = ""
mode = 0 # defualt is file
piped = 0 # defualt is no
extract = 0 # default is no
ignore_missing = 0 # default is no

def print_version():
    print("s3g version %s" % version)
    exit(0)

for opt, arg in opts: 
    # no switch statements :(
    if opt == '-t':
        template_dir = arg 
    elif opt == '-i':
        mode = 1
    elif opt == '-c':
        cache_file = arg
    elif opt == '-v':
        print_version()
    elif opt == '-s':
        ignore_missing = 1
    else:
        print("Unknown Arg: %s" % opt)
        exit(1)

# ensure [-c ... [-i]]
if mode and not cache_file:
    print("[!] -i requires -c")
    exit(1)

# QoL Read
def read(path):
    c = ""
    with open(path) as f:
        c = f.read()
    return c

# pull file content
file_content = read(filename)

# parse header
# headers are cached as:
#
# key: value 
# ...: ...
# ---
# 
def parse_meta(meta_str):
    meta_field = {}
    meta_str = meta_str[0:( len(meta_field)-4 )] # strip trailing '---\n'
    while meta_str:
        key = meta_str[0:( meta_str.find(": ") )]
        value = meta_str[( len(key)+2 ):( meta_str.find("\n") )]        
        meta_field[key] = value
        meta_str = meta_str[( len(key) + 2 + len(value) + 1 ):] # on to the next kp 
    return meta_field 

# strip header
meta_str = file_content[4:( file_content.find("---\n", 4) + 4 )]
file_content = file_content[( len(meta_str) + 4 ):] # strip file_content

# context
context = [{}] # it's assumed that context[0] is the own page's context
context[0] = parse_meta(meta_str) # parse own-context
# FIXME: this is kind of awful; s3g should be able to take this already parsed
# This would require two modes; 
#   - parsing and caching a header, and then returning the body to be piped somewhere else
#   - taking a filename and processed html body to then apply to templates
# Target is ~ 2.5.0
context[0]["text"] = subprocess.run([translator, filename], check=True, capture_output=True, text=True).stdout

if mode: # index-specific context
    cache_content = read(cache_file)
    file_caches = cache_content.split("---\n")
    file_caches.pop() # the last element is always null 
    for cache in file_caches:
        context.append(parse_meta(cache))
else: # just bump the cache
    f = open(cache_file, 'a')
    f.write(meta_str)
    f.close()

# templating
template_file = "default"
if "type" in context[0].keys():
    template_file = context[0]["type"]

template = read(template_dir+"/"+template_file+".html")

# translate a local scope
def local_parse(context, scheme_template):
    scheme = scheme_template
    while scheme.count('$'):
        index = scheme.index('$')+1
        key = scheme[index:scheme.find('$', index)]

        value = ""
        try:
            value = context[key]
        except:
            if not ignore_missing:
                print("Incorrect Key: %s" % key)
                exit(3)
        scheme = scheme.replace("$%s$" % key, value)

    return scheme

# apply template
# TODO
# this could be done more idomatically (but old habits die hard)

html = ""
i = 0
while i < len(template):
    if template[i] == '@': 
        scheme_begin = template.find('#', i)
        
        colon = template.find(':', i)
        key = template[i+1:colon]
        value = template[colon+1:scheme_begin]
        
        scheme = template[scheme_begin+1:( template.find('#', scheme_begin+1) )] 
        
        for c in context:
            if (key not in c.keys()) or (value and value != c[key]):
                continue 
            html += local_parse(c, scheme) + '\n'

        i = ( template.find('@', i+1) + 1 ) 
    elif template[i] == '#':
        end_of_scheme = template.find('#', i+1)
        html += local_parse(context[0], template[i+1:end_of_scheme]) 

        i = ( end_of_scheme + 1 )
    else:
        html += template[i]
        i += 1

print(html)
