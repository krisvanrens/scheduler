# scheduler
Task scheduler / run loop in C++

# Compiler Explorer

WIP version: https://www.godbolt.org/z/75YdzP

# Requirements

Requirements levels as defined in [RFC 2119](https://www.ietf.org/rfc/rfc2119.txt).

## MUST

* Support running tasks based on class member functions.
* Support running tasks based on free functions.
* Support running tasks based on lambda expressions.
* Support a configurable/changeable scheduling strategy.
* Support the configuration of the amount of workers.

## SHOULD

* Support the concept of task priority.
* Support scheduling of automatic periodic tasks.

## MAY

...
