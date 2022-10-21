Smol Static Site Generator (s3g)
----
Generates html based on [CommonMark standards](https://spec.commonmark.org/0.30).

cloc: `181`

### Configuration
The markdown files should have a meta-field tag at the top. This should be as follows;

```md
---
key: value
---
```
*s3g will look for the three dashed header specifically.*

These key:value pairs will be valid variables in templates. Prefix all variables with `$` - e.g. `<author>$author</author>`.

### Features
RoadMap:
 - [ ] Conditional Templates
 - [ ] CSS
 - [ ] MathJax


