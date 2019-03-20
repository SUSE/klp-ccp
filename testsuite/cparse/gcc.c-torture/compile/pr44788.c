void joint_decode(float* mlt_buffer1, int t) {
    int i;
    float decode_buffer[1060];
    foo(decode_buffer);
    for (i=0; i<10 ; i++) {
        mlt_buffer1[i] = i * decode_buffer[t];
    }
}

/* cp-out: warning: [^:]*: line 4, column 4: identifier "foo" not declared */
