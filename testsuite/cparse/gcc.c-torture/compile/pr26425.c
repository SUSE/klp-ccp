struct keyring_list {
 struct key *keys[0];
};
void keyring_destroy(struct keyring_list *keyring, unsigned short a)
{
 int loop;
  for (loop = a - 1; loop >= 0; loop--)
   key_put(keyring->keys[loop]);
}

/* cp-out: warning: [^:]*: line 8, column 3: identifier "key_put" not declared */
