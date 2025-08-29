//.  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
//.  SPDX-PackageHomePage: https://repo.dentato.com/fsm
//.  SPDX-License-Identifier: MIT

//   oooooooooooo    .oooooo..o   ooo        ooooo
//   `888'     `8   d8P'    `Y8   `88.       .888'
//    888           Y88bo.         888b     d'888 
//    888oooo8       `"Y8888o.     8 Y88. .P  888 
//    888    "           `"Y88b    8  `888'   888 
//    888           oo     .d8P    8    Y     888 
//   o888o          8""88888P'    o8o        o888o

// Macros to easily implement a clean Finite State Machine
// NOTE: There can be only ONE fsm per function.
//
// Example:
//
//   fsm {
//       /* There must be one and only one START state per FSM */
//       fsmstate(START) { 
//         if (cur_chr == 'a') fsmgoto(vowels);
//         if (cur_chr == 'z') fsmgoto(consonants);
//         if (++cur_char) fsmgoto(START); 
//         /* Exit the FSM if end of string */
//       }
//   
//       fsmstate(vowels) { // State names must be valid C identifiers
//         /* PROCESS VOWELS */
//         fsmgoto(START); 
//       }
//
//       /* ANY CODE IN-BETWEEN STATES WILL BE IGNORED */
//
//       fsmstate(consonants) {
//         /* PROCESS CONSONANTS */
//         fsmexit; /* STOP PROCESSING */
//       }
//   }       

#ifndef FSM_VERSION
#define FSM_VERSION 0x0003000C

// When executing the FSM, we'll enter the 'true' branch of the 'if' statement and
// will jump to the state 'START' which must exist in the 'false' block of the 'if'.
// The (empty) code after the `fsm_exit` label will not optimized away because
// there will surely be a `goto fsm_exit` statement (see the `fsmstate` macro).
#define fsm  if (1) {goto fsm_state_START; fsm_exit: ;} else 

// A state is always entered via `goto` to the `fsm_state_XXXX` label.
// The `for` and `if` are there to ensure that if the execution starts from the top
// or reaches the bottom of the state code, we'll exit from the FSM.
// The double `for` ensures that a 'break' or 'continue' within a state
// will exit the entire fsm and not just the current state.
// Anyways, the prescribed way to exit a fsm is via `fsmexit`.
#define fsmstate(s)  for(;;) for(;;) if (1) goto fsm_exit; else fsm_state_ ## s:

// Move to the next state
#define fsmgoto(s)  goto fsm_state_ ## s

// Exit from the FSM
#define fsmexit     goto fsm_exit

#endif // FSM_VERSION
