# Glom

Glom is a Scheme Interpreter written in C++.

Glom basically follows the [R5RS Scheme standard](https://docs.racket-lang.org/r5rs/r5rs-std/index.html), but since it's an assignment of a unit in my Y1S1, I wouldn't add complex features such as macros, continuations at first, I'll add them later if I have time.

## Features
- Distinction Ideas
  - [x] Tokenizer
  - [x] Parser & Expr
  - [x] Context
  - [x] Primitive Members
  - [x] Evaluation with Immutable Context
  - [x] Tail Call Optimization
- High Distinction Ideas
  - [x] int64 & bigint & rational & real(long double)
  - [x] Evaluation with Mutable Context
  - [x] Call with Current Context (call/cc)
  - [ ] Delayed Evaluation
  - [ ] Module
  - [ ] Standard Library

## Build & Test

### Nix

I highly recommend using Nix to build and test Glom, since it can manage dependencies easily.

First and foremost, prepare your Nix and Flake:

- Ensure you have enabled `experimental-features = nix-command flakes` in your Nix configuration.

Then, you can build Glom with: `nix build .` and you can see the executable in `./result/bin/glom`.

For testing, you can run: `nix flake check -L` to run the tests.

### Other

You can check `flake.nix` to see the dependencies and the steps of building & testing, you need at least CMake 3.31.7 and a C++20 compatible compiler.

## Why C++?

I'm a Rustacean, so I don't like C++, naturally.

But here's the key problem: I don't know much about C++, which is not a good reason to dislike it.

After a **revolution** of my unit in my university, we learn about Programming Concepts, in languages, C#, C/C++ and Python.

The final project of the unit is to set up my own project in one of the languages above, it's a good change to learn C++, thus I chose it.

## Why Scheme?

I'm about to finish reading [Structure and Interpretation of Computer Programs](https://web.mit.edu/6.001/6.037/sicp.pdf) (SICP) by Harold Abelson and Gerald Jay Sussman, it's a great book, I highly recommend it for those who want to learn programming in a philosophic level.

In SICP, the authors use Scheme as the programming language to illustrate the concepts, and I fell in love with it, so I decided to implement a Scheme interpreter.

## Why Glom?

It's my nickname, and it means "to grab or hold onto something" which is a nice metaphor for a programming language that allows you to manipulate data and functions.