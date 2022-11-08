Smol Static Site Generator (s3g)
----
Generates html based on [CommonMark standards](https://spec.commonmark.org/0.30).

> very fast, very smol. cloc: `239`

### Features
RoadMap:
 - [x] Conditional Templates
 - [x] CSS
 - [x] MathJax
 - [x] endnotes
 - [ ] RSS
 - [ ] Atom
 - [ ] Tracked Meta-Fields

### Usage
`make recompile` to recompile

`make` to rebuild site

You should use `$` and `$$` to denote LaTex; `\(` and `\[` are treated as escaped sequences.

### Configuration
The markdown files should have a meta-field tag at the top. This should be as follows;

```md
---
key: value
---
```

These `key:value` pairs will be valid variables in templates. Prefix and end all variables with `VAR_IND`. Key types are entirely arbitrary, but there are reserved keys:

**Reserved Key Types**
| Key   | Information                              | Content                  |
|-------|------------------------------------------|--------------------------|
| type  |Post type to use in templating            | Valid template file stem |
| path  |(Optional) File path when item is tracked | N/A                      |

---

**Expanding Fields**

Any multi-value fields need to have a scheme. Schemes are denoted by `SCHEME_IND`. The html inside of the scheme will be duplicated for each value in the field.

Caveats:
- Items will be listed in the same order of appearance as the field.
- There can be **only one** variable in each scheme.

e.g. (with `SCHEME_IND` set to `#`)
```html
<ul>
  #<li class="$variable$-class">$variable$, </li>#
</ul>
```

would be built as

```html
<ul>
  <li class="...-class">..., </li>
  <!-- for however many variables under the same tag -->
</ul>
```

---

**Tracked Values**

Meta-Fields can be tracked; if a field is tracked, the item's path will be made avaliable to the site compiler for each value of the field. 

e.g. with `tags` tracked
item's `tags` field:
```md
...
tags: math, space, theory, physics
...
```
would track the file path of `item` for each of the `tags` values.

Referencing tracked fields only adds an initial layer of abstraction, beyond that it's just like any other meta-field. Wrap your typical scheme with `<TRACKED_IND><tracked_value><TRACKED_IND><scheme><TRACKED_IND>`, anything within the second set of `TRACKED_IND` is scoped to an iteration of each tracked item.

For example, to iterate over all items tagged with `tags: ... math ...`, and display their titles:

with `TRACKED_IND` set to `&`, `SCHEME_IND` to `#`, and `VAR_IND` to `$`;
```html
...
<ul>
&math&#<li><a href=$path$>##$heading$</a>, pub: ##$date$#</li>&
</ul>
```

#### Templates
s3g will attempt to match the `type` header to a filename in `TEMPLATE_DIR` - e.g. `type: post` would link to `TEMPLATE_DIR/post.html`. Should explict linking fail, or there is no explict `type`, s3g will use the `TEMPLATE_DIR/default.html` template.
