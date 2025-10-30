// RUN: %clang_cc1 -std=c++17 -fsyntax-only -verify %s

// C++20 [temp.spec] 13.9/6:
//   The usual access checking rules do not apply to names in a declaration
//   of an explicit instantiation or explicit specialization, with the
//   exception of names appearing in a function body, default argument,
//   base-clause, member-specification, enumerator-list, or static data member
//   or variable template initializer.
//   [Note : In particular, the template arguments and names used in the
//   function declarator(including parameter types, return types and exception
//   specifications) may be private types or objects that would normally not be
