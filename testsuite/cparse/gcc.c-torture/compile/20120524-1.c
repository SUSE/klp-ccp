  char CJPAT_Packet[1508] = {};
void build_packet(int port, char *packet) {
            memcpy(packet, CJPAT_Packet, sizeof(CJPAT_Packet)); 
}

/* cp-out: warning: [^:]*: line 3, columns 12-18: identifier "memcpy" not declared */
