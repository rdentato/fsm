# fsm

[![Version](https://img.shields.io/badge/version-0.3.000%20RC-blue.svg)](https://github.com/rdentato/fsm)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

A minimal library to implement Finite State Machines in C

# FSM — Tiny Finite-State Machines for C

A tiny, zero-overhead set of C macros to write **clear, explicit** finite-state machines using plain `goto` and labels—without big `switch` statements or function trampolines.

* **Single-header.** Drop the header in your project and `#include` it.
* **Zero runtime overhead.** Compiles to straight branches.
* **Readable control-flow.** Named states, explicit transitions.
* **Portable.** Standard C (C99+), no deps.

Full documentation is in the <a href="docs/fsm.md"><b>Programmers manual</b></a>.

---

## Quick example

```c
#include <stdio.h>
#include <ctype.h>
#include "fsm.h"

void classify(const char *s) {
  size_t i = 0;

  fsm {
    fsmstate(START) {
      if (!s[i]) fsmexit;
      int c = (unsigned char)s[i++];
      if (!isalpha(c)) fsmgoto(START);
      switch (c | 32) { // tolower
        case 'a': case 'e': case 'i': case 'o': case 'u': fsmgoto(VOWEL);
        default:                                          fsmgoto(CONSONANT);
      }
    }

    fsmstate(VOWEL)     { puts("vowel");     fsmgoto(START); }
    fsmstate(CONSONANT) { puts("consonant"); fsmgoto(START); }
  }
}

int main(void) { classify("aZ?!e"); }
```
Output:

```
vowel
consonant
vowel
```

---

## How it works (in one minute)

* `fsm { … }` starts the machine and jumps to `fsmstate(START)`.
* `fsmstate(NAME) { … }` defines a state you can jump to.
* `fsmgoto(NAME)` transitions to another state.
* `fsmexit` leaves the FSM and continues after the `fsm { … }` block.

Under the hood it’s just labels and `goto`, arranged so:

* entering the FSM **always** begins at `START`;
* falling off the end of a state **exits** the FSM (no silent fall-through);
* `break`/`continue` inside a state also exit the FSM (prefer `fsmgoto`/`fsmexit` for clarity).

---

## API

```c
fsm { /* states only */ }          // open an FSM block (one per function)

fsmstate(START) { /* ... */ }      // required entry state (exactly one)
fsmstate(Name)  { /* ... */ }      // more states

fsmgoto(Name);                     // transition to a state
fsmexit;                           // exit the FSM
```

---

## Rules of thumb

* One FSM per function. `START` must exist exactly once.
* Put **only states** inside `fsm { … }`. Any code between states won’t execute.
* Declare and initialize variables **before** `fsm` or **inside** a state (so initializers run).
* If you need to “stay” in a state, explicitly `fsmgoto(ThisState);`.

---

## Why use this?

* You get the **clarity of named states** and transitions.
* You keep **tight, predictable codegen** (no function call overhead).
* You avoid fragile `switch` fall-throughs and huge nested loops.

---

## Getting started

1. Copy `fsm.h` into your project. The file in the `dist/` directory is identical to the one in the `src/` directory, excepet for having all comments removed.
2. `#include "fsm.h"` where you implement the machine.
3. Write your `fsm` with a `START` state and explicit `fsmgoto` transitions.

