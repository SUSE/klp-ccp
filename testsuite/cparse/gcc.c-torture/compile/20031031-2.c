/* PR/10239 */

enum node_type
{
  INITIAL = 0, FREE,
  PRECOLORED,
  SIMPLIFY, SIMPLIFY_SPILL, SIMPLIFY_FAT, FREEZE, SPILL,
  SELECT,
  SPILLED, COALESCED, COLORED,
  LAST_NODE_TYPE
};

inline void
put_web (enum node_type type)
{
  switch (type)
    {
    case INITIAL:
    case FREE:
    case FREEZE:
    case SPILL:
      foo ();
      break;
    case PRECOLORED:
      bar ();
      break;
    default:
      baz ();
    }
}

void
reset_lists ()
{
  put_web (INITIAL);
}

/* cp-out: warning: [^:]*: line 22, columns 6-9: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 25, columns 6-9: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 28, columns 6-9: identifier "baz" not declared */
