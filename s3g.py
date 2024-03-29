import sys
import getopt

# pull args
try:  
    opts, args = getopt.getopt(sys.argv[1:], "t:c:i:xas") 
except:
    print("Invalid Argument")
    exit(1)

# cmd args
template_dir = ""
cache_file = ""
composite_cache_file = ""
ignore_missing = False
index_mode = False
file_content = "".join(sys.stdin.readlines())

def extract():
    global file_content
    # strip header
    meta_str = file_content[4:( file_content.find("---\n", 4) + 4 )]
    file_content = file_content[( len(meta_str) + 4 ):] # strip file_content

    # cache the file
    f = open(cache_file, 'w') 
    f.write(meta_str)
    f.close()
    
    print(file_content)

def apply():
    # parse header 
    def parse_meta(meta_str):
        meta_field = {}
        meta_str = meta_str[0:( len(meta_field)-4 )] # strip trailing '---\n'
        while meta_str:
            key = meta_str[0:( meta_str.find(": ") )]
            value = meta_str[( len(key)+2 ):( meta_str.find("\n") )]        
            meta_field[key] = value
            meta_str = meta_str[( len(key) + 2 + len(value) + 1 ):] # on to the next kp 
        return meta_field 

    # QoL Read
    def read(path):
        c = ""
        with open(path) as f:
            c = f.read()
        return c

    # context
    context = [{}] # it's assumed that context[0] is the own page's context
    # read from own cache-file
    context[0] = parse_meta(read(cache_file))  

    if index_mode:
        composite = read(composite_cache_file) 
        file_caches = composite.split("---\n")
        file_caches.pop() # the last element is always null 
        for cache in file_caches:
            context.append(parse_meta(cache)) 

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
    html = ""
    i = 0
    while i < len(template):
        if template[i] == '@': 
            scheme_begin = template.find('#', i)
            scheme_end = template.find('#', scheme_begin+1)
            
            colon = template.find(':', i)
            key = template[i+1:colon]
            value = template[colon+1:scheme_begin]
             
            scheme = template[scheme_begin+1:scheme_end] 
        
            for c in context:
                if (key not in c.keys()) or (value and value != c[key]):
                    continue 
                html += local_parse(c, scheme) + '\n'

            i = ( scheme_end + 1 ) 
        elif template[i] == '#':
            end_of_scheme = template.find('#', i+1)
            html += local_parse(context[0], template[i+1:end_of_scheme]) 

            i = ( end_of_scheme + 1 )
        elif template[i] == '%':
            html += file_content
            i += 1
        else:
            html += template[i]
            i += 1

    print(html) 

mode = None # extract or apply

for opt, arg in opts: 
    # no switch statements :(
    if opt == '-t':
        template_dir = arg 
    elif opt == '-i':
        composite_cache_file = arg
        index_mode = True
    elif opt == '-c':
        cache_file = arg  
    elif opt == '-s':
        ignore_missing = True
    elif opt == '-x':
        mode = 1
    elif opt == '-a':
        mode = 0
    else:
        print("Unknown Arg: %s" % opt)
        exit(1)

# ensure -c ...
if not cache_file:
    print("Invalid Argument")
    exit(1)

# verify arg integrity
if mode == 0 and template_dir: # apply
    apply()
# this could be done more elegantly by checking opts
elif mode == 1 and not ( template_dir or index_mode or ignore_missing ): # extract
    extract()
else:
    print("Invalid Argument")
    exit(1)
