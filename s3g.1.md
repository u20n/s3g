% s3g(1)
% uton
% 2023

# NAME
s3g - smol static site generator

# SYNOPSIS
**s3g** -x -c CACHE\_FILE

**s3g** -a \[-s\] -c CACHE\_FILE \[-i COMPOSITE\_CACHE\_FILE\] -t TEMPLATE\_FOLDER

# DESCRIPTION
**s3g** compiles either standalone or index html files from markdown and template files. **s3g** outputs the compiled html file to stdout. Running **s3g** without any arguments results in **s3g** printing this man page and its version, then exiting.

# OPTIONS

-a
: Apply templates. **s3g** takes its parsed html from `stdin`.

-x
: Extract meta-field. **s3g** takes the file, parses and caches the header, and then pushes the body of the file to `stdout`.

**-i** COMPOSITE\_CACHE\_FILE
: Sets to index mode. Requires a composite cache file of all files to be indexed.

-s
: Silently ignore missing variables. This applies to variables referenced in templates which have no value in a file's meta-field.

**-c CACHE_FILE**
: Sets the cache file. 

**-t TEMPLATE_FOLDER**
: Sets the template folder.

# EXIT VALUES
**0**
: Success

**1**
: Invalid argument

**2**
: Invalid File

**3**
: Misconfigured File

# BUGS
Bugs can be reported at <https://github.com/u20n/s3g/issues>.

# COPYRIGHT
Copyright © 2023 uton \<me@notu.dev\>. License MIT:

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
