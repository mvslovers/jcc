static long test1 (unsigned char * b, unsigned long * svcmem,
  char ** outputs, char ** inouts, long * cursor,
  unsigned char ** oattrs, unsigned char ** ioattrs) {
long o;
long i;
long f;
long l;
long p;
long key;
long curloc;

/* Start */

o = 0;
i = 0;
p = 0;

/* Open Keyboard, Set Position */
memcpy (&(b [p]), "\x0F5\x0C3\x011\x040\x000", 5);
p += 5;

/* Colour: White, style: Normal, direction: output */
memcpy (&(b [p]), "\x029\x003\x0C0\x0F4\x042\x0F7\x041\x0F0", 8);
p += 8;
memcpy (&(b [p]), "\x03C\x040\x01A\x040", 4); /* 25 Spaces */
p += 4;

memcpy (&(b [p]), "The 3270 panel designer", 23);
p += 23;

memcpy (&(b [p]), "\x03C\x047\x026\x040", 4); /* 437 Spaces */
p += 4;

memcpy (&(b [p]), "DSN:", 4);
p += 4;

memcpy (&(b [p]), "\x03C\x047\x02F\x040", 4); /* 5 Spaces */
p += 4;

/* Colour: Bright Yellow, style: Underscore, direction: input */
memcpy (&(b [p]), "\x029\x003\x0C0\x0C5\x042\x0F6\x041\x0F4", 8);
if (ioattrs) {
  if (ioattrs [i][2])
    b [p + 3] = 0xF4;
  if (ioattrs [i][0])
    b [p + 5] = ioattrs [i][0];
  if (ioattrs [i][1])
    b [p + 7] = ioattrs [i][1];
}
p += 8;
if (*cursor == i) {
  memcpy (&(b [p]), "\x013", 1);
  p++;
}
/* Input/Output length 44 */
l = strlen (inouts [i]);
if (l > 44) l = 44;
memcpy (&(b [p]), inouts [i], l);
p += l;
memset (&(b [p]), 0, 44 - l);
p += 44 - l;
i++;

/* Colour: White, style: Normal, direction: output */
memcpy (&(b [p]), "\x029\x003\x0C0\x0F4\x042\x0F7\x041\x0F0", 8);
p += 8;
memcpy (&(b [p]), "\x03C\x048\x036\x040", 4); /* 25 Spaces */
p += 4;

memcpy (&(b [p]), "MEMBER:", 7);
p += 7;

memcpy (&(b [p]), "  ", 2);
p += 2;

/* Colour: Bright Yellow, style: Underscore, direction: input */
memcpy (&(b [p]), "\x029\x003\x0C0\x0C5\x042\x0F6\x041\x0F4", 8);
if (ioattrs) {
  if (ioattrs [i][2])
    b [p + 3] = 0xF4;
  if (ioattrs [i][0])
    b [p + 5] = ioattrs [i][0];
  if (ioattrs [i][1])
    b [p + 7] = ioattrs [i][1];
}
p += 8;
if (*cursor == i) {
  memcpy (&(b [p]), "\x013", 1);
  p++;
}
/* Input/Output length 8 */
l = strlen (inouts [i]);
if (l > 8) l = 8;
memcpy (&(b [p]), inouts [i], l);
p += l;
memset (&(b [p]), 0, 8 - l);
p += 8 - l;
i++;

/* Colour: White, style: Normal, direction: output */
memcpy (&(b [p]), "\x029\x003\x0C0\x0F4\x042\x0F7\x041\x0F0", 8);
p += 8;
memcpy (&(b [p]), "\x03C\x04C\x026\x040", 4); /* 221 Spaces */
p += 4;

memcpy (&(b [p]), "DSN:", 4);
p += 4;

memcpy (&(b [p]), "\x03C\x04C\x02F\x040", 4); /* 5 Spaces */
p += 4;

/* Colour: Bright Yellow, style: Underscore, direction: input */
memcpy (&(b [p]), "\x029\x003\x0C0\x0C5\x042\x0F6\x041\x0F4", 8);
if (ioattrs) {
  if (ioattrs [i][2])
    b [p + 3] = 0xF4;
  if (ioattrs [i][0])
    b [p + 5] = ioattrs [i][0];
  if (ioattrs [i][1])
    b [p + 7] = ioattrs [i][1];
}
p += 8;
if (*cursor == i) {
  memcpy (&(b [p]), "\x013", 1);
  p++;
}
/* Input/Output length 44 */
l = strlen (inouts [i]);
if (l > 44) l = 44;
memcpy (&(b [p]), inouts [i], l);
p += l;
memset (&(b [p]), 0, 44 - l);
p += 44 - l;
i++;

/* Colour: White, style: Normal, direction: output */
memcpy (&(b [p]), "\x029\x003\x0C0\x0F4\x042\x0F7\x041\x0F0", 8);
p += 8;
memcpy (&(b [p]), "\x03C\x04D\x036\x040", 4); /* 25 Spaces */
p += 4;

memcpy (&(b [p]), "MEMBER:", 7);
p += 7;

memcpy (&(b [p]), "  ", 2);
p += 2;

/* Colour: Bright Yellow, style: Underscore, direction: input */
memcpy (&(b [p]), "\x029\x003\x0C0\x0C5\x042\x0F6\x041\x0F4", 8);
if (ioattrs) {
  if (ioattrs [i][2])
    b [p + 3] = 0xF4;
  if (ioattrs [i][0])
    b [p + 5] = ioattrs [i][0];
  if (ioattrs [i][1])
    b [p + 7] = ioattrs [i][1];
}
p += 8;
if (*cursor == i) {
  memcpy (&(b [p]), "\x013", 1);
  p++;
}
/* Input/Output length 8 */
l = strlen (inouts [i]);
if (l > 8) l = 8;
memcpy (&(b [p]), inouts [i], l);
p += l;
memset (&(b [p]), 0, 8 - l);
p += 8 - l;
i++;

/* Colour: White, style: Normal, direction: output */
memcpy (&(b [p]), "\x029\x003\x0C0\x0F4\x042\x0F7\x041\x0F0", 8);
p += 8;
memcpy (&(b [p]), "\x03C\x055\x025\x040", 4); /* 476 Spaces */
p += 4;

/* Colour: Bright Red, style: Normal, direction: output */
memcpy (&(b [p]), "\x029\x003\x0C0\x0F4\x042\x0F2\x041\x0F0", 8);
p += 8;
memcpy (&(b [p]), "\x03C\x05C\x030\x040", 4); /* 458 Spaces */
p += 4;

/* Colour: White, style: Normal, direction: output */
memcpy (&(b [p]), "\x029\x003\x0C0\x0F4\x042\x0F7\x041\x0F0", 8);
p += 8;
memcpy (&(b [p]), "F3=Exit  F5=Copy", 16);
p += 16;

memcpy (&(b [p]), "  ", 2);
p += 2;

/* Colour: Bright Red, style: Normal, direction: output */
memcpy (&(b [p]), "\x029\x003\x0C0\x0F4\x042\x0F2\x041\x0F0", 8);
if (oattrs) {
  if (oattrs [o][0])
    b [p + 5] = oattrs [o][0];
  if (oattrs [o][1])
    b [p + 7] = oattrs [o][1];
}
p += 8;
/* Output length 40 */
l = strlen (outputs [o]);
if (l > 40) l = 40;
memcpy (&(b [p]), outputs [o], l);
p += l;
memset (&(b [p]), 0, 40 - l);
p += 40 - l;
o++;

memcpy (&(b [p]), "\x03C\x05D\x03F\x040\x040", 5); /* 20 Spaces */
p += 5;

f = i;

svcmem [0] = (p & 0x0000FFFF);
svcmem [1] = (0x03000000 | ((long)b & 0x00FFFFFF));
svcmem [2] = 0;
svcmem [3] = 0x81000000;

MYTERMO (svcmem);
p = MYTERMI (b);

if (p <= 0) {
  key = -1;
} else if (b [0] == '\x07D') {
  key = 0;
} else if ((b [0] >= '\x0F1') && (b [0] <= '\x0F9')) {
  key = (long)(b [0] - '\x0F0');
} else if ((b [0] >= '\x07A') && (b [0] <= '\x07C')) {
  key = (long)(b [0] - '\x070');
} else if ((b [0] >= '\x0C1') && (b [0] <= '\x0C9')) {
  key = (long)(b [0] - '\x0B4');
} else if ((b [0] >= '\x04A') && (b [0] <= '\x04C')) {
  key = (long)(b [0] - '\x034');
} else {
  key = -2;
}

o = 3;
i = 0;
if ((ioattrs) && (ioattrs [i][2]))
  while ((i < f) && (ioattrs [i][2])) i++;
if (i >= f) return (key);

if (p >= 3) curloc = (((b [1] & 0x3F) << 6) | (b [2] & 0x3F));
if ((o < p) && (b [o] == '\x011')) {
  if (curloc >= (((b [o + 1] & 0x3F) << 6) | (b [o + 2] & 0x3F)))
    *cursor = i;
  o += 3;
}

l = 0;
while (o < p) {
  if (b [o] == '\x011') {
    inouts [i][l] = 0;
    i++;
    if ((ioattrs) && (ioattrs [i][2]))
      while ((i < f) && (ioattrs [i][2])) i++;
    if (i >= f) return (key);
    if (curloc >= (((b [o + 1] & 0x3F) << 6) | (b [o + 2] & 0x3F)))
      *cursor = i;
    o += 3;
    l = 0;
  } else {
    inouts [i][l++] = b [o++];
  }
}
if ((p >= 3) && (inouts)) {
  inouts [i][l] = 0;
}

return (key);

/* End */
}
