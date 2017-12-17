# STM32F103C8T6-BootLoader
STM32F103C8T6的Boot程序。支持通过Y-mode协议进行IAP升级

## 1.地址分配

| PAGE | ADDRESS | Bytes |
| ------| ------ | ------ |
| **BOOT 区** |BOOT区的最后1K存放标志量和信息。所以BOOT最大只有11K
| page 0 | 0x0800 0000 - 0x0800 03FF | 1K |
| page 1 | 0x0800 0400 - 0x0800 07FF | 1K |
| page 2 | 0x0800 0800 - 0x0800 0BFF | 1K |
| ... | ... | ... |
| page 11 | 0x0800 2C00 - 0x0800 2FFF | 1K |
| **APP 区** |
| page 12 | 0x0800 3000 - 0x0800 33FF | 1K |
| page 13 | 0x0800 3400 - 0x0800 37FF | 1K |
| ... | ... | ... |
| page 31 | 0x0800 7C00 - 0x0800 7FFF | 1K |
## 2.说明：
### ymodem.c---int32_t Ymodem_Receive (uint8_t *buf) ###
	函数中定义的临时变量uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD] 长度比较大，有可能会造成数组越界。
	解决方法有两个：
	【1】 在 startup_stm32f103xb.s 中的 Stack_Size EQU 0x400 改为 Stack_Size EQU 0x800
	【2】 如果不用【1】的方法，直接去改栈的大小，那也可以在函数中临时分配堆空间给数组。
			把变量uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD]直接分配改为临时分配
			uint8_t *packet_data = NULL; 
			packet_data = (uint8_t *)malloc(sizeof(uint8_t)*(PACKET_1K_SIZE + PACKET_OVERHEAD));
			注：在函数退出前，一定要把分配的空间释放掉 free(packet_data)
