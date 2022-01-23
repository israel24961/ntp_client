main: main.c ntp_packet.c
	gcc --std=c17 main.c ntp_packet.c -o ntp_packet
