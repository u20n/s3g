Smol Static Site Generator (s3g)
----
Generates html based on [CommonMark standards](https://spec.commonmark.org/0.30).

> very fast, very smol; cloc'ing in @ < 300

### Features
RoadMap:
 - [x] Conditional Templates
 - [x] CSS
 - [x] MathJax
 - [x] \*notes (end, foot, side, etc.) 

**Non-Features**

Tracked Meta-Fields: While the templating does seem to promote tracking fields, its introduction would compromise the 'smol' nature of s3g by introducing partial templates, and the process of tracking on its own. While alternatives don't exist (yet), it is possible to replicate functionality through clever file and page structuring and some links.

RSS/Atom: It's out of the scope of s3g to handle RSS/Atom natively; you're better off using an html $\to$ RSS/Atom generator.

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

---

#### Templates
s3g will attempt to match the `type` header to a filename in `TEMPLATE_DIR` - e.g. `type: post` would link to `TEMPLATE_DIR/post.html`. Should explict linking fail, or there is no explict `type`, s3g will use the `TEMPLATE_DIR/default.html` template.

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
