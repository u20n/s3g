Smol Static Site Generator (s3g)
----
Generates html based on [CommonMark standards](https://spec.commonmark.org/0.30).

cloc: `180`

### Configuration
The markdown files should have a meta-field tag at the top. This should be as follows;

```md
---
key: value
---
```
*s3g will look for the three dashed header specifically.*

These key:value pairs will be valid variables in templates. Prefix all variables with `$` - e.g. `<author>$author</author>`. It is assumed that **all** values present in the header are a vector - you should do any textual formatting in the meta field, as s3g will insert all possible values for a key under its specifier. 

### Features
RoadMap:
 - [ ] Conditional Templates
 - [ ] CSS
 - [ ] MathJax


