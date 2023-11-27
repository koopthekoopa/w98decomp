#ifndef INTRIN_H
#define INTRIN_H

__inline UINT __readeflags() {
	unsigned int result = 0;
	__asm {
		pushf
		pop (result)
	}
	return result;
}

__inline void __writeeflags(UINT x) {
	__asm {
		push (x)
		popf
	}
}

#endif // INTRIN_H


