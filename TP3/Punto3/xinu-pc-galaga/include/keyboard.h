#define inportb(p)      inb(p)
#define outportb(p,v)   outb(p,v)

#define KEYBOARD_DATAREG 0x60  // :Data Register(Read\Write)
#define KEYBOARD_CMDREG 0x64   // :Command Register(Read\Write)
#define KEYBOARD_BUFFER_LENGTH 10


extern unsigned char kblayout [128];  // { ... } Fill your layout yourself 

struct kbdata{
    sid32 kb_mutex;
    int pid_mutex;
    int	kbhead;		                        // Head of buffer
	int	kbtail;		                        // Tail of buffer
	unsigned char kbbuff[KEYBOARD_BUFFER_LENGTH];	// Buffer
    sid32   kbbuffsem;                      // Buffer free space semaphore
};

extern	struct	kbdata	Kb_data;