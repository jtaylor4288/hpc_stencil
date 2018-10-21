warnings = -Wall -Werror -Wextra -pedantic

stencil: stencil.c
	gcc -std=c99 $(warnings) $^ -o $@ $(optimise)

clean:
	rm stencil

O2: | O2_set stencil

O2_set:
	$(eval optimise=-O2)

O3: | O3_set stencil

O3_set:
	$(eval optimise=-O3)
