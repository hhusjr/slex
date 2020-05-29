# slex
SLex: a configurable lexical analyzer

This is one of my toy project while learning compiler techniques. SLex is a configurable lexical analyzer, in which you
can use Regular Expression to set up rules for lexemes and SLex will tokenize the input char stream automatically according
to your regexps. Of course, I will do long-term maintenance for this project.

The project is incomplete now. I'm working on it.

# Implementation
SLex contains four modules.

## Scanner
The scanner scans input file or string and convert the char stream into a unified data structure.

## Regexp Parser
Users can describe rules by regular expression. Regexp Parser will separate the regular expression into tokens, and build
an expression tree for regular expression. Then, regexp parser will traverse the expression tree and build Non-deterministic
Finite Automata(NFA) recursively.

## FA
FA includes functions for creating NFA, convert NFA to DFA, and minimize DFA, along with the data structures for storing them.

## SLex Main
It is the front-end part of the lexical analyzer. The most important function in SLex Main is slex(), which traverse the scanner,
and use Table-Driven DFA to match the patterns. Also, SLex Main provides interface for users to save rules into file, and load rules
from file.
