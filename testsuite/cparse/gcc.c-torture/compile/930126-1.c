typedef unsigned T;
typedef char Tchar;
T mt (long, char *);
T ms (long, char *);
T mv (long, T, char);
T cons (T, T);
T decl (T * (*) (T *), char *);

T*L92(T*),*L15(T*),*L14(T*),*L13(T*),*L12(T*),*L11(T*),*L10(T*),*L9(T*),*L8(T*),*L7(T*),*L6(T*),*L5(T*),*L4(T*),*L3(T*),*L2(T*),*L1(T*);

static T *
Ldata (T * my_pc)
{
int cc = (((* ((T *) (my_pc))) >> 16) & 0xFF);
T B92, B91, B90, B15, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1, tO7, tO6, tO5, tO4, tO3, tO2, tO1, tO0;
T object = mv (168, 0, ((Tchar) 1));
T * cb = (T *) (((T) (object & 0x3FF)) | 0x400);
tO0 = mv (92, 0, ((Tchar) 1));
B92 = decl (L92, "");
B15 = decl (L15, "");
B14 = decl (L14, "");
B13 = decl (L13, "");
B12 = decl (L12, "");
B11 = decl (L11, "");
B10 = decl (L10, "");
B9 = decl (L9, "");
B8 = decl (L8, "");
B7 = decl (L7, "");
B6 = decl (L6, "");
B5 = decl (L5, "");
B4 = decl (L4, "");
B3 = decl (L3, "");
B2 = decl (L2, "");
B1 = decl (L1, "");
cb[19] = ((((cc) & 0xFF) << 16) | (9 & 0xFF));
cb[21] = ((((cc) & 0xFF) << 16) | ((10) & 0xFF));
cb[23] = ((((cc) & 0xFF) << 16) | (11 & 0xFF));
cb[25] = ((((cc) & 0xFF) << 16) | (12 & 0xFF));
cb[27] = ((((cc) & 0xFF) << 16) | (13 & 0xFF));
cb[29] = ((((cc) & 0xFF) << 16) | (14 & 0xFF));
cb[31] = ((((cc) & 0xFF) << 16) | (15 & 0xFF));
cb[35] = ((((cc) & 0xFF) << 16) | (17 & 0xFF));
cb[36] = ((0x1A) << 26) | (((0x39) << 26) | 1) & 0x3FF;
cb[39] = ms (24, ((char *) ""));
cb[41] = ((0x1A) << 26) | (((0x39) << 26) | 1) & 0x3FF;
cb[44] = 3;
cb[46] = 2;
cb[48] = 3;
cb[50] = 6;
cb[52] = 4;
cb[146] = tO0;
((T *) (((tO0 & 0x3FF)) | 0x400))[92] = B1;
((T *) (((tO0 & 0x3FF)) | 0x400))[91] = B2;
((T *) (((tO0 & 0x3FF)) | 0x400))[2] = B90;
((T *) (((tO0 & 0x3FF)) | 0x400))[2] = B91;
((T *) (((tO0 & 0x3FF)) | 0x400))[1] = B92;
cb[58] = 0x2800 | (T) ((T *) ((B6 & 0x3FF) | 0x400) + 3);
cb[57] = 0x2800 | (T) ((T *) ((B7 & 0x3FF) | 0x400) + 3) & ~0xC00;
cb[56] = 0x2800 | (T) ((T *) ((B8 & 0x3FF) | 0x400) + 3) & ~0xC00;
cb[55] = 0x2800 | (T) ((T *) ((B9 & 0x3FF) | 0x400) + 3) & ~0xC00;
tO7 = mv (8, 0, ((Tchar) 1));
tO4 = ms (9, ((char *) ""));
tO3 = mv (58, 0, ((Tchar) 1));
tO6 = ms (4, ((char *) ""));
tO2 = mv (4, 0, ((Tchar) 1));
tO5 = ms (4, ((char *) ""));
tO1 = mv (28, 0, ((Tchar) 1));
cb[165] = tO1;
cb[163] = cons (((ms (10, ((char *) "")))), (cons (tO5, 0)));
cb[162] = cons (1, (cons (2, 0)));
cb[150] = cons (1, (cons (2, (cons (3, (cons (4, (cons (5, (cons (6, 0)))))))))));
cb[148] = tO7;
return cb;
}

/* cp-out: warning: [^:]*: line 43, columns 28-29: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 45, columns 28-29: integer overflow in shift operation */
