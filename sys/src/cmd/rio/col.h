enum {
	Colrioback,

	Numcolors,
};

extern Image *col[Numcolors];
void themeload(char *s, int n);
char *themestring(int *n);
