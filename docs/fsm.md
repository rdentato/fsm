# Finite-State Machine (FSM) Macros — Programmer’s Manual

This manual explains how to use the tiny FSM macro suite below to build fast, readable, zero-overhead state machines in ISO C (C99/C11+). It covers the mental model, rules, macro reference, patterns, pitfalls, and worked examples.

```c
// --- header (given) ----------------------------------------------------------
// Macros to easily implement a clean Finite State Machine
// NOTE: There can be only ONE fsm per function.

#ifndef FSM_VERSION
#define FSM_VERSION 0x0002001B

// Enter the START state, and predefine the shared exit label.
#define fsm  if (1) {goto fsm_state_START; fsm_exit: ;} else

// Define a state labeled fsm_state_<name>.
// Reaching top or bottom of the state exits the FSM.
// 'break' or 'continue' inside the state also exit the FSM.
#define fsmstate(s)  for(;;) for(;;) if (1) goto fsm_exit; else fsm_state_ ## s:

// Transition to another state
#define fsmgoto(s)  goto fsm_state_ ## s

// Exit the FSM immediately
#define fsmexit     goto fsm_exit

#endif // FSM_VERSION
```

---

## 1) Quick start

**Minimal template**

```c
int scan(const char *s) {
  size_t i = 0;
  int vowels = 0;

  fsm {
    fsmstate(START) {
      if (!s[i]) fsmexit;
      if (strchr("aeiouAEIOU", s[i++])) fsmgoto(vowels_state);
      fsmgoto(START);
    }

    fsmstate(vowels_state) {
      vowels++;
      fsmgoto(START);
    }
  }

  return vowels;
}
```

Key ideas you can see already:

* You **start** the machine with `fsm { ... }`.
* You **must define** exactly one `fsmstate(START)`.
* You **move** between states with `fsmgoto(StateName)`.
* You **leave** the machine with `fsmexit` (or by “falling out” of a state—see below).

---

## 2) Mental model (how it works)

The macros are a structured façade over two labels (`fsm_state_*` and `fsm_exit`) and a few unconditional jumps:

* `fsm` expands to an `if (1) { goto fsm_state_START; fsm_exit: ; } else { ... }`.
  That means: when the function reaches `fsm`, control **jumps forward** into the `START` state inside the `else` block. The label `fsm_exit` is also defined up-front and reachable to end the FSM.

* Each `fsmstate(NAME)` expands to:
  `for(;;) for(;;) if (1) goto fsm_exit; else fsm_state_NAME:`
  followed by your `{ ... }` block.
  This guarantees:

  * If execution somehow starts from the very top of the state *without* a jump, the `if(1)` immediately sends you to `fsm_exit`.
  * If execution **falls off the end** of your state block (you didn’t `fsmgoto` anywhere), the next loop iteration hits the same `if(1)` and exits.
  * A bare `break` or `continue` inside the state exits the whole FSM (via those loops). Use this sparingly for clarity; prefer `fsmexit`.

So, **a state runs exactly until you `fsmgoto` another state (or `fsmexit`)**. If you don’t, you exit the FSM by falling off the end.

---

## 3) Ground rules & invariants

1. **One FSM per function.** The macros declare a single `fsm_exit` label and reserve the `fsm_state_` label prefix. A second `fsm` in the same function will collide.

2. **Exactly one `START` state.** It is the entry point (`fsm` jumps to it). If missing, you’ll get a compile-time “label not found” error.

3. **Only valid C identifiers as state names.** `fsmstate(parse_1)` is fine; `fsmstate(1st)` is not.

4. **No meaningful code between states.** Any code outside states but inside the `fsm { … }` block is **skipped** (the initial jump goes directly to the first label), and after any state ends you exit the FSM. Treat the block as a container for states only. (Comments are fine.)

5. **Transitions are explicit.** There’s no fall-through. To stay in the same state, do `fsmgoto(ThisState);`.

6. **Scope & declarations.**

   * Variables declared **before** `fsm` are normal and visible inside states.
   * Declarations **inside** a state are fine.
   * Declarations **between** states (or before the first state inside `fsm {}`) are **not executed** (initializers are skipped), though they do introduce names into scope. Don’t rely on those initializers.
   * **Do not** put VLA declarations (variably modified types) between states—C forbids `goto` *into* the scope of a VLA (C11 6.8.6.1p1). Declare VLAs either before `fsm` or inside a state.

7. **Reserved identifiers.** Avoid defining your own labels named `fsm_exit` or starting with `fsm_state_`.

---

## 4) Macro reference (with semantics & tips)

### `fsm { … }`

* **Purpose:** Introduce the FSM in a function and jump to `START`.
* **Semantics:** Jumps to `fsm_state_START:`. Defines the `fsm_exit` label.
* **Gotchas:** Only one per function. Treat the block as a container for `fsmstate(...)` definitions.

### `fsmstate(Name) { … }`

* **Purpose:** Define a state block.
* **Entry:** Only via `fsmgoto(Name)`. (The macro is designed so “accidental” entry or fall-through exits the FSM.)
* **Exit:**

  * `fsmgoto(Other)` to transition,
  * `fsmexit` to finish,
  * **falling off the end** exits,
  * `break;` or `continue;` anywhere in the state also exits (equivalent to `fsmexit`).

**Advice:** Prefer `fsmgoto`/`fsmexit` for clarity. Use `break`/`continue` only if you intend a hard exit and want to be terse.

### `fsmgoto(Name)`

* **Purpose:** Transition to `Name`.
* **Semantics:** `goto fsm_state_Name`.
* **Gotchas:** Target must exist (or you’ll get a “label not found” diagnostic). There is no implicit loop; if you need to “re-enter” the same state, do `fsmgoto(Name)`.

### `fsmexit`

* **Purpose:** Exit the FSM immediately (return to the statement *after* the `fsm { … }` block).
* **Semantics:** `goto fsm_exit`.

---

## 5) Patterns & examples

### A) Scanner with two states (vowels vs consonants)

```c
#include <stdio.h>
#include <ctype.h>

void classify(const char *s) {
  size_t i = 0;

  fsm {
    fsmstate(START) {
      if (!s[i]) fsmexit;
      int c = (unsigned char)s[i++];
      if (!isalpha(c)) fsmgoto(START);           // ignore non-letters
      switch (tolower(c)) {
        case 'a': case 'e': case 'i': case 'o': case 'u':
          fsmgoto(vowel);
        default:
          fsmgoto(consonant);
      }
    }

    fsmstate(vowel) {
      puts("vowel");
      fsmgoto(START);
    }

    fsmstate(consonant) {
      puts("consonant");
      // Demonstrate hard-exit with 'fsmexit'
      if (s[i] == '!') fsmexit;
      fsmgoto(START);
    }
  }
}
```

Notes:

* Any code between states would be ignored; we put **only** `fsmstate(...)` blocks inside `fsm { }`.
* Falling off the end of `vowel`/`consonant` would exit; we explicitly `fsmgoto(START)` to continue.

---

### B) Tokenizer: numbers vs identifiers

```c
#include <ctype.h>
#include <string.h>

typedef enum { TOK_EOF, TOK_ID, TOK_NUM } tok_t;

typedef struct {
  tok_t  kind;
  char   buf[64];
} token;

token next_token(const char **pcur) {
  const char *s = *pcur;
  token t = { .kind = TOK_EOF, .buf = {0} };
  size_t n = 0;

  fsm {
    fsmstate(START) {
      if (!*s) { t.kind = TOK_EOF; fsmexit; }
      unsigned char c = (unsigned char)*s;

      // Skip spaces
      if (isspace(c)) { s++; fsmgoto(START); }

      if (isalpha(c) || c == '_') { t.kind = TOK_ID; fsmgoto(ID); }
      if (isdigit(c))             { t.kind = TOK_NUM; fsmgoto(NUM); }

      // Unknown -> consume and continue
      s++; fsmgoto(START);
    }

    fsmstate(ID) {
      while (*s && (isalnum((unsigned char)*s) || *s == '_')) {
        if (n + 1 < sizeof t.buf) t.buf[n++] = *s;
        s++;
      }
      t.buf[n] = 0;
      fsmexit;
    }

    fsmstate(NUM) {
      while (isdigit((unsigned char)*s)) {
        if (n + 1 < sizeof t.buf) t.buf[n++] = *s;
        s++;
      }
      t.buf[n] = 0;
      fsmexit;
    }
  }

  *pcur = s;
  return t;
}
```

Highlights:

* **Start state** triages input; **worker states** collect characters.
* Each worker state **falls out via `fsmexit`** (explicit), returning a built token.

---

### C) Coroutine-like stepper

You can structure long operations into micro-steps (handy for cooperative scheduling or chunked processing):

```c
// Returns 0 while there’s more work, 1 when finished.
int step_compress(struct ctx *ctx) {
  fsm {
    fsmstate(START) {
      init_tables(ctx);
      ctx->i = 0;
      fsmgoto(scan);
    }

    fsmstate(scan) {
      if (ctx->i >= ctx->n) fsmgoto(flush);
      process_byte(ctx, ctx->in[ctx->i++]);
      // Yield to caller (stop FSM now, resume next call)
      fsmexit;
    }

    fsmstate(flush) {
      finish_blocks(ctx);
      return 1;            // returning from a state is fine
    }
  }
  return 0;
}
```

The trick: call `step_compress` repeatedly. Each call executes **one** `scan` step then exits. On the next call, it re-enters at `fsm` and immediately jumps to `START` again—so you must carry “where you are” in your own data (`ctx->i`, flags, etc.) if you need true coroutine behavior. (This macro set doesn’t persist the program counter; you model continuation in your state logic/data.)

---

## 6) Control-flow details you should know

* **Falling out of a state exits the FSM.** If the closing `}` of a state is reached without `fsmgoto` / `fsmexit` / `return`, the nested `for(;;)` forces an exit on the next iteration.

* **`break` and `continue` inside a state exit the FSM.** They target the nearest loop (the macro’s `for(;;)`), and the very next iteration hits `if (1) goto fsm_exit;`.
  Prefer `fsmexit` for readability—future readers won’t wonder which loop you meant.

* **`switch` inside a state:** `break` exits the `switch` only (as usual). It does **not** leave the state unless that `break` is outside the switch. If you want to end the whole FSM from inside a `switch`, use `fsmexit`.

* **`return` inside a state** returns from the function immediately. That’s fine and sometimes the cleanest exit.

* **Debugging:** Each state is a real label `fsm_state_<name>`. Stepping in a debugger jumps between these labels as expected. Grepable, too.

---

## 7) Variables, scope, and initialization

* **Best practice:** Declare variables **before** `fsm` (so any initialization runs) or **inside** states (so you can’t accidentally skip initializers).
  Avoid declarations with side-effect initializers between states.

* **VLAs and `goto`:** Don’t place VLAs between states or before the first state inside `fsm {}`. Jumping into the block would violate C’s “no entering VLA scope” rule. If you need a VLA, declare it inside the state that uses it, or before `fsm`.

* **Lifetime:** States don’t open or close scopes beyond the block you write. Automatic variables behave exactly as you expect.

---

## 8) Error reporting & compile-time checks

* **Missing `START`:** You’ll see “label ‘fsm\_state\_START’ used but not defined.” Add `fsmstate(START)`.

* **Misspelled state in `fsmgoto`:** “label ‘fsm\_state\_…’ not found.” Fix the name.

* **Two FSMs in one function:** “redefinition of label ‘fsm\_exit’.” Put each FSM in its own function.

---

## 9) Performance characteristics

* **Zero runtime overhead:** It’s all labels and unconditional jumps; compilers generate straight-line code with direct branches. The two `for(;;)` loops don’t loop in the hot path—they exist to ensure well-defined exits.

* **Branch prediction:** As with any hand-rolled FSM, keep “hot” transitions straight-line when possible for cache and predictor friendliness.

---

## 10) FAQ

**Q: Can I put helper functions/macros between states?**
A: You can put **preprocessor directives** (e.g., `#define`, `#include`) or **type/variable declarations** there if you only need names in scope. But any executable statements (including initializers) will be skipped. Prefer to place helpers **above** `fsm` or in headers.

**Q: Can I nest FSMs?**
A: Not in the same function (labels would clash). Call another function that contains its own `fsm`.

**Q: How do I stay in the same state?**
A: `fsmgoto(ThisState);`

**Q: Can a state loop internally?**
A: Yes—use `while`/`for` inside the state. Just remember that `break` (outside a `switch`) exits the FSM; if you want to break only your inner loop, use a flag or `goto` a local label within the state’s block.

---

## 11) Recipe snippets

**A) Re-consume input / “put back” one character**

```c
fsmstate(READ) {
  if (!*p) fsmexit;
  int c = (unsigned char)*p++;
  if (c == '#') { p--; fsmgoto(COMMENT); } // reconsumes '#'
  /* ... */
}
```

**B) One-shot state with cleanup**

```c
fsmstate(CLEANUP) {
  free(tmp);
  fsmexit; // explicit
}
```

**C) Error path**

```c
fsmstate(PARSE_INT) {
  if (!isdigit((unsigned char)*p)) { err = EINVAL; fsmgoto(ERROR); }
  /* ... */
}

fsmstate(ERROR) {
  // centralize error handling
  log_error(err);
  fsmexit;
}
```

---

## 12) Style guidelines

* Name states in **UPPER\_SNAKE\_CASE** or **lower\_snake\_case** consistently.
* Keep each state **short** and focused; move heavy lifting to helpers.
* Prefer explicit `fsmgoto`/`fsmexit` over `break`/`continue` for clarity.
* Put **no executable code** outside states inside the `fsm {}` block.

---

## 13) Full worked example — simple INI line parser

Parses a single line into `key=value` pairs, ignoring comments and whitespace.

```c
#include <ctype.h>
#include <string.h>

typedef struct { const char *k, *v; size_t kn, vn; } kv;

int parse_ini_line(const char *line, kv *out) {
  const char *p = line, *k = NULL, *v = NULL;
  size_t kn = 0, vn = 0;

  fsm {
    fsmstate(START) {
      // skip spaces
      while (isspace((unsigned char)*p)) p++;
      if (!*p || *p == ';' || *p == '#') fsmexit; // blank/comment
      k = p; fsmgoto(KEY);
    }

    fsmstate(KEY) {
      while (*p && *p != '=' && !isspace((unsigned char)*p)) p++;
      kn = (size_t)(p - k);
      while (isspace((unsigned char)*p)) p++;
      if (*p != '=') { kn = vn = 0; fsmexit; } // invalid
      p++; // skip '='
      while (isspace((unsigned char)*p)) p++;
      if (!*p) { vn = 0; fsmexit; }
      v = p; fsmgoto(VALUE);
    }

    fsmstate(VALUE) {
      const char *start = p;
      while (*p && *p != ';' && *p != '#') p++;        // read until comment
      const char *end = p;
      while (end > start && isspace((unsigned char)end[-1])) end--; // rtrim
      v  = start;
      vn = (size_t)(end - start);
      fsmexit;
    }
  }

  if (kn && out) { out->k = k; out->kn = kn; out->v = v; out->vn = vn; return 1; }
  return 0;
}
```

---

## 14) Troubleshooting checklist

* **Compiler complains about missing labels?** Check `START` exists and the state name in `fsmgoto` matches exactly.
* **Initialization didn’t run?** Did you put it between states? Move it into `START` or before `fsm`.
* **Warnings about jumping into VLA scope?** Move VLA declarations inside states or above `fsm`.
* **Accidentally used `break`?** Remember it exits the FSM. Replace with `fsmgoto(NEXT)` or restructure with a local loop label.

---

## 15) Conclusions

These macros give you:

* **Explicit control flow** (each transition is named),
* **Predictable lifetime & scope** (just C blocks),
* **No runtime overhead** (pure labels/gotos),
* **Tiny surface area** (four macros).

They’re ideal for scanners, tiny parsers, protocol handlers, and step-wise jobs where you want the clarity of named states without the ceremony of a big `switch` or the overhead of function calls.

Happy state-crafting!
