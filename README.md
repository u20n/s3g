Smol Static Site Generator (s3g)
----
Generates html based on [CommonMark standards](https://spec.commonmark.org/0.30).

> very fast, very smol. cloc: `207`

### Features
RoadMap:
 - [x] Conditional Templates
 - [x] CSS
 - [x] MathJax
 - [ ] RSS
 - [ ] Atom
 - [ ] (Indexable) Meta-Fields

### Usage
`make recompile` to recompile

`make` to rebuild site

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

#### Templates
s3g will attempt to match the `type` header to a filename in `TEMPLATE_DIR` - e.g. `type: post` would link to `TEMPLATE_DIR/post.html`. Should explict linking fail, or there is no explict `type`, s3g will use the `TEMPLATE_DIR/default.html` template.

#### [Begin] Under Review
**Pack Expansion**
s3g uses modifiers to determine the way that values are displayed (e.g. expand value packs). These are prefixed the the variable in templates. The order of interpretation is as follows `A"S"$variable$`, where `A` is some modifier, `S` is some seperator, and `variable` is the variable to operate on.

| Modifier | Behavior                                       |
|----------|------------------------------------------------|
| @        | lists all of the values, in order of apperance |
| d        | lists values in (Alph-Num) descending order    |
| a        | lists values in (Alph-Num) ascending order     |

s3g will tag each item as a list item with a class corressponding to its key - e.g. `<li class=key>...</li>`.

**Indexing Values**
`config.hpp` contains an unordered set within which all indexed types are held. Should s3g encounter a page with a 'tracked' field, it will make the header of the page avaliable to index templates, which are located in `TEMPLATE_DIR/index/`. You must expand the pack before you can parse the inner fields, using scoping.
`@"- "(@", "$tags$)` is functionally equivalent to: 
```
for each item in items
  html.add("- ")
  for each tag in item.tags
    html.add(", "+tag)
```

#### [End] Under Review
