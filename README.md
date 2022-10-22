Smol Static Site Generator (s3g)
----
Generates html based on [CommonMark standards](https://spec.commonmark.org/0.30).

> very fast, very smol. cloc: `197`

### Configuration
The markdown files should have a meta-field tag at the top. This should be as follows;

```md
---
key: value
---
```
*s3g will look for the three dashed header specifically.*

These `key:value` pairs will be valid variables in templates. Prefix and end all variables with `$` - e.g. `$author$`. It is assumed that **all** values present in the header are a vector - you should do any textual formatting in the meta field, as s3g will insert all possible values for a key under its specifier. s3g will wrap the each occurance of the variable with a tag in the key's name - e.g. `type: value` would produce `<type>value</type>`. Most of the values are arbitrary, but there are some that are reserved;

**Reserved Key Types**
| Key  | Information                   | Content                         |
|------|-------------------------------|---------------------------------|
| title|Page title                     | Path title to be displayed      |
| type |Post type to use in templating | Valid template file stem        |

#### Under Review
**Modifiers**
s3g uses modifiers to determine the way that values are displayed. These are prefixed the the variable in templates. The order of interpretation is as follows `A"S"$variable$`, where `A` is some modifier, `S` is some seperator, and `variable` is the variable to operate on.

| Modifier | Behavior                                       |
|----------|------------------------------------------------|
| @        | lists all of the values, in order of apperance |
| v        | lists values in descending order               |
| ^        | lists values in ascending order                |
#### [End] Under Review

#### Templates
Templates should be in `./templates`. s3g will attempt to match the `type` header to a filename in `./templates` - e.g. `type: post` would link to `./templates/post.html`. Should explict linking fail, or there is no explict `type`, s3g will use the `./templates/default.html` template.

### Features
RoadMap:
 - [x] Conditional Templates
 - [x] CSS
 - [x] MathJax
 - [ ] RSS
 - [ ] Atom
 - [ ] (Indexable) Values

### Usage
`make recompile` to recompile

`make` to rebuild site
