# Thesis text

This folder contains the LaTeX sources for the thesis text, the popular article and the IEEE article.

## Building

The three texts can be compiled to PDFs with version 3.0 of the [arara](https://github.com/cereda/arara) toolchain:

* `arara ieee-article.tex`
* `arara popular-article.tex`
* `arara thesis.tex`

The thesis text includes the produced PDFs as appendices, so make sure to compile it *after* the two articles.
