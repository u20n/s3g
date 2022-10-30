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

These `key:value` pairs will be valid variables in templates. Prefix and end all variables with `$` - e.g. `$author$`. It is assumed that **all** values present in the header are a vector - you should do any textual formatting in the meta field, as s3g will insert all possible values for a key under its specifier. s3g will wrap the each occurance of the variable with a tag in the key's name - e.g. `type: value` would produce `<type>value</type>`. Most of the values are arbitrary, but there are some that are reserved;

**Reserved Key Types**
| Key   | Information                   | Content                         |
|-------|-------------------------------|---------------------------------|
| title |Page title                     | Path title to be displayed      |
| type  |Post type to use in templating | Valid template file stem        |

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
  ... <!-- for however many variables under the same tag -->
</ul>
```

---

**Indexing Values**

[TODO]

#### Templates
s3g will attempt to match the `type` header to a filename in `TEMPLATE_DIR` - e.g. `type: post` would link to `TEMPLATE_DIR/post.html`. Should explict linking fail, or there is no explict `type`, s3g will use the `TEMPLATE_DIR/default.html` template.
