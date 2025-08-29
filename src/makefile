#  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
#  SPDX-License-Identifier: MIT

SRC=../src
DIST=../dist

dist: $(DIST)/fsm.h

$(DIST)/fsm.h: $(SRC)/fsm.h
	sed -e '/^\/\/ /d' -e '/^\/\/$$/d' -e '/^ *$$/d' $(SRC)/fsm.h > $(DIST)/fsm.h

clean_dist:
	rm -f $(DIST)/fsm.h
