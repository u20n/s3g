Smol Static Site Generator (s3g)
----

s3g is a script which generates static sites.

### Usage

s3g uses an arbitrary markdown translator (e.g. [m2h](https://github.com/u20n/m2h)) to compose its pages.

refer to s3g's man page (`make man`) for specific usage details.

### File Configuration
The markdown files should have a meta-field tag at the top. This should be as follows;

```md
---
key:value
---
```
> *notice the spacing between key and value*

These `key: value` pairs will be valid variables in templates. Key types are entirely arbitrary, but there are reserved keys:

**Reserved Key Types**
| Key   | Information                              | Content                  |
|-------|------------------------------------------|--------------------------|
| type  | Item type to use in templating           | Valid template file stem |
| text  | Item content (e.g. the body of the file) | N/A                      |

### Templates
s3g will attempt to match the `type` header to a filename in `TEMPLATE_DIR` - e.g. `type: post` would link to `TEMPLATE_DIR/post.html`. Should explict linking fail, or there is no explict `type`, s3g will use the `TEMPLATE_DIR/default.html` template.

### Meta-Field Patterns

Patterns are designated by their scope; Multiple and Local. Local is straightfoward; it is scoped to a given page's meta-field. Multiple is often used within the context of multiple meta-fields, but it can also be used to signify a multiple within an otherwise local context; see Example #3 below.

| Indicator          | Type           |
|--------------------|----------------|
| `@key:value`       | Multiple Scope |
| `#`                | Local Scope    |
| `$`                | Variable       |

Patterns generally take the following forms:


1. A simple local.
```
<ul>
  #
  <li class="title">$title$</li>
  <li class="date">$date$</li>
  #
</ul>
```

2.a A tag page for physics posts.
```
<ul>
  @tag:physics#
  <li class="title">$title$</li>
  #@
</ul>
```

2.b A tag page for all tags.
```
<ul>
  @tag:#
  <li class="title">$title$ - $tag$</li>
  #@
</ul>
```

3. A local scoped multiple which lists a page's tags; requires non-`-i` usage. 
```
<ul>
  @tag:#
  <li class="tag">$tag$</li>
  #@
</ul>
```
