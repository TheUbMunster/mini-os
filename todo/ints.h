//#define IRQ_KBD 1
//#define IOAPIC 0xFEC00000
//#define REG_VER 0x01
//#define REG_ID 0x00
//#define REG_TABLE 0x10
//#define INT_DISABLED 0x00010000
//#define T_IRQ0 32
////copied mostly from XV6
//
//// IO APIC MMIO structure: write reg, then read or write data.
//struct ioapic {
//	unsigned int reg;
//	unsigned int pad[3];
//	unsigned int data;
//};
//
//volatile struct ioapic* ioapic;
//
//static unsigned int ioapicread(int reg)
//{
//	ioapic->reg = reg;
//	return ioapic->data;
//}
//
//static void ioapicwrite(int reg, unsigned int data)
//{
//	ioapic->reg = reg;
//	ioapic->data = data;
//}
//
//void ioapicinit(void)
//{
//	int i, id, maxintr;
//
//	ioapic = (volatile struct ioapic*)IOAPIC;
//	maxintr = (ioapicread(REG_VER) >> 16) & 0xFF;
//	id = ioapicread(REG_ID) >> 24;
//	/*if (id != ioapicid)
//		cprintf("ioapicinit: id isn't equal to ioapicid; not a MP\n");*/
//
//	// Mark all interrupts edge-triggered, active high, disabled,
//	// and not routed to any CPUs.
//	for (i = 0; i <= maxintr; i++) {
//		ioapicwrite(REG_TABLE + 2 * i, INT_DISABLED | (T_IRQ0 + i));
//		ioapicwrite(REG_TABLE + 2 * i + 1, 0);
//	}
//}
//
//void ioapicenable(int irq, int cpunum)
//{
//	// Mark interrupt edge-triggered, active high,
//	// enabled, and routed to the given cpunum,
//	// which happens to be that cpu's APIC ID.
//	ioapicwrite(REG_TABLE + 2 * irq, T_IRQ0 + irq);
//	ioapicwrite(REG_TABLE + 2 * irq + 1, cpunum << 24);
//}