//.  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
//.  SPDX-PackageHomePage: https://repo.dentato.com/fsm
//.  SPDX-License-Identifier: MIT
#ifndef FSM_VERSION
#define FSM_VERSION 0x0003000C
#define fsm  if (1) {goto fsm_state_START; fsm_exit: ;} else 
#define fsmstate(s)  for(;;) for(;;) if (1) goto fsm_exit; else fsm_state_ ## s:
#define fsmgoto(s)  goto fsm_state_ ## s
#define fsmexit     goto fsm_exit
#endif // FSM_VERSION
