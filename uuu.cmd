uuu_version 1.2.135
SDP: boot -f SPL
SDPU: delay 10
SDPU: write -f SPL -addr 0x400
SDPU: jump -addr 0x400
SDPU: done

