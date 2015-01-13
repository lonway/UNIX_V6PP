//Kernel .cpp

#include "Video.h"
#include "Simple.h"
#include "IOPort.h"
#include "Chip8253.h"
#include "Chip8259A.h"
#include "Machine.h"
#include "IDT.h"
#include "Assembly.h"
#include "Kernel.h"
#include "TaskStateSegment.h"

#include "PageDirectory.h"
#include "PageTable.h"
#include "SystemCall.h"

#include "Exception.h"
#include "DMA.h"
#include "CRT.h"
#include "TimeInterrupt.h"
#include "PEParser.h"
#include "CMOSTime.h"
#include "..\test\TestInclude.h"

bool isInit = false;

extern "C" void MasterIRQ7()
{
	SaveContext();
	
	Diagnose::Write("IRQ7 from Master 8259A!\n");
	
	//需要在中断处理程序末尾先8259A发送EOI命令
	//实验发现：有没有下面IOPort::OutByte(0x27, 0x20);这句运行效果都一样，本来以为
	//发送EOI命令之后会有后续的IRQ7中断进入， 但试下来结果是IRQ7只会产生一次。
	IOPort::OutByte(Chip8259A::MASTER_IO_PORT_1, Chip8259A::EOI);

	RestoreContext();

	Leave();

	InterruptReturn();
}

extern "C" int main0(void)
{
	Chip8253::Init(20);	//初始化时钟中断芯片
	Chip8259A::Init();
	Chip8259A::IrqEnable(Chip8259A::IRQ_TIMER);		
	DMA::Init();
	Chip8259A::IrqEnable(Chip8259A::IRQ_IDE);
	Chip8259A::IrqEnable(Chip8259A::IRQ_SLAVE);
	Chip8259A::IrqEnable(Chip8259A::IRQ_KBD);

	Machine& machine = Machine::Instance();
	//init gdt
	machine.InitGDT();
	machine.LoadGDT();
	//init idt
	machine.InitIDT();	
	machine.LoadIDT();

	//init page protection
	machine.InitPageDirectory();
	machine.EnablePageProtection();
	/* 
	 * InitPageDirectory()中将线性地址0-4M映射到物理内存
	 * 0-4M是为保证此注释以下至本函数结尾的代码正确执行！
	 */

	//使用0x10段寄存器
	__asm__ __volatile__
		(" \
		mov $0x10, %ax\n\t \
		mov %ax, %ds\n\t \
		mov %ax, %ss\n\t \
		mov %ax, %es\n\t"
		);

	//将初始化堆栈设置为0xc0400000，这里破坏了封装性，考虑使用更好的方法
	__asm__ __volatile__
		(
		" \
		mov $0xc0400000, %ebp \n\t \
		mov $0xc0400000, %esp \n\t \
		jmp $0x8, $_next"
		);
	
	//i = 64;	
}

//loadexe()会把runtime()函数拷贝到线性地址0xC0000000处，runtime()用于ring0退出到ring3特权级之后执行的代码
extern "C" void runtime()
{
	/*
	1. 销毁runtime的stack Frame
	2. esp中指向用户栈中argc位置，而ebp尚未正确初始化
	3. eax中存放可执行程序EntryPoint
	4~6. exit(0)结束进程
	*/
	__asm__ __volatile__("	leave;	\
							movl %%esp, %%ebp;	\
							call *%%eax;		\
							movl $1, %%eax;	\
							movl $0, %%ebx;	\
							int $0x80"::);
}

/*
 * 用户态运行的代码，处理用户态的信号处理程序，先从栈中取出入口地址，
 * 然后保护现场，运行完成后，恢复现场
 */

/*   曾经用这个实现进程从核心态回用户态后先执行信号处理函数……现在不需要了。
extern "C" void SignalHandler()
{
	__asm__ __volatile__("	leave;	\
			                xchgl %%eax, (%%esp);	\
							pushl %%ebx;	\
							pushl %%ecx;	\
							pushl %%edx;	\
							pushl %%esi;	\
							pushl %%edi;	\
							pushl %%ebp;	\
							call *%%eax;	\
							popl %%ebp;	\
							popl %%edi;	\
							popl %%esi;	\
							popl %%edx;	\
							popl %%ecx;	\
							popl %%ebx;	\
							popl %%eax;	\
							ret"::);
}
*/

/*
  * 1#进程在执行完MoveToUserStack()从ring0退出到ring3优先级后，会调用ExecShell()，此函数通过"int $0x80"
  * (EAX=execv系统调用号)加载“/Shell.exe”程序，其功能相当于在用户程序中执行系统调用execv(char* pathname, char* argv[])。
  * (此处需要这样做的原因是：内核代码不与C函数库一起编译，所以内核无法引用V6++ libc中的execv系统调用。)
  */
extern "C" void ExecShell()
{
	int argc = 0;
	char* argv = NULL;
	char* pathname = "/Shell.exe";
	__asm__ __volatile__ ("int $0x80"::"a"(11/* execv */),"b"(pathname),"c"(argc),"d"(argv));
	return;
}

/* 切勿随意改变从runtime()到loadexe()这些函数源代码的位置(可能会导致难以排查的运行时错误)！！*/
extern "C" void loadexe()
{
	unsigned long exesrc = (unsigned long)runtime;
	unsigned char* src = (unsigned char*)exesrc;
	unsigned char* des = (unsigned char*)0xC0000000;

	for (unsigned int i = 0; i < (unsigned long)loadexe - (unsigned long)runtime; i++ )
	{
		*des++ = *src++;
	}
}

extern "C" void Delay()
{
	for ( int i = 0; i < 50; i++ )
		for ( int j = 0; j < 10000; j++ )
		{
			int a;
			int b;
			int c=a+b;
			c++;
		}
}

extern "C" void next()
{
	//loadexe();

	//这个时候0M-4M的内存映射已经不被使用了，所以要重新映射用户态的页表，为用户态程序运行做好准备
	Machine::Instance().InitUserPageTable();
	FlushPageDirectory();

	Machine::Instance().LoadTaskRegister();
	
	/* 获取CMOS当前时间，设置系统时钟 */
	struct SystemTime cTime;
	CMOSTime::ReadCMOSTime(&cTime);
	/* MakeKernelTime()计算出内核时间，从1970年1月1日0时至当前的秒数 */
	Time::time = Utility::MakeKernelTime(&cTime);

	/* 从CMOS中获取物理内存大小 */
	unsigned short memSize = 0;	/* size in KB */
	unsigned char lowMem, highMem;

	/* 这里只是借用CMOSTime类中的ReadCMOSByte函数读取CMOS中物理内存大小信息 */
	lowMem = CMOSTime::ReadCMOSByte(CMOSTime::EXTENDED_MEMORY_ABOVE_1MB_LOW);
	highMem = CMOSTime::ReadCMOSByte(CMOSTime::EXTENDED_MEMORY_ABOVE_1MB_HIGH);
	memSize = (highMem << 8) + lowMem;

	/* 加上1MB以下物理内存区域，计算总内存容量，以字节为单位的内存大小 */
	memSize += 1024; /* KB */
	PageManager::PHY_MEM_SIZE = memSize * 1024;
	UserPageManager::USER_PAGE_POOL_SIZE = PageManager::PHY_MEM_SIZE - UserPageManager::USER_PAGE_POOL_START_ADDR;
	//Diagnose::Write("EXTENDED_MEMORY_ABOVE_1MB = %d KB  (%d MB) (%x)\n", memSize, memSize / 1024, PageManager::PHY_MEM_SIZE);

	/* 
	 * 真正操作系统内核初始化逻辑，
	 * @TODO记得写文档区分machine的初始化与kernel的初始化 
	 */
	Kernel::Instance().Initialize();	
	Kernel::Instance().GetProcessManager().SetupProcessZero();
	isInit = true;

	Kernel::Instance().GetFileSystem().LoadSuperBlock();
	Diagnose::Write("Unix V6++ FileSystem Loaded......OK\n");

	/*  初始化rootDirInode和用户当前工作目录，以便NameI()正常工作 */
	FileManager& fileMgr = Kernel::Instance().GetFileManager();
	fileMgr.rootDirInode = g_InodeTable.IGet(DeviceManager::ROOTDEV, FileSystem::ROOTINO);
	fileMgr.rootDirInode->i_flag &= (~Inode::ILOCK);

	User& us = Kernel::Instance().GetUser();
	us.u_cdir = g_InodeTable.IGet(DeviceManager::ROOTDEV, FileSystem::ROOTINO);
	us.u_cdir->i_flag &= (~Inode::ILOCK);
	Utility::StringCopy("/", us.u_curdir);

	/* 打开TTy设备 */
	int fd_tty = lib_open("/dev/tty1", File::FREAD);

	if ( fd_tty != 0 )
	{
		Utility::Panic("STDIN Error!");
	}
	fd_tty = lib_open("/dev/tty1", File::FWRITE);
	if ( fd_tty != 1 )
	{
		Utility::Panic("STDOUT Error!");
	}
	Diagnose::TraceOn();

	//loadexe();
	
	int pid = Kernel::Instance().GetProcessManager().NewProc();  //0#进程复制图像给1#进程时，只能借助核心空间的页表
	if( 0 == pid )
	{
		us.u_procp->p_ttyp = NULL;
		Kernel::Instance().GetProcessManager().Sched();
		//while(true)
		//{
		//	Diagnose::Write("Parent Process...Swtch()\n");
		//	us.u_procp->Sleep((unsigned long)us.u_procp, ProcessManager::PWAIT);
		//}
	}
	else
	{
		/*这里对系统用户页表进行初始化，让所有的页面 p = 1，R/W = 1（可读可写）。这是因为1#进程复制了0#进程的
		相对虚实地址映射表。那张表的内容是全0，而0#进程创建1#进程的时候有执行过MapToPageTable，这样0#进程运行在系统用户页表
		几乎是全0的状态（MapToPageTable有补上0#页面和2047#页面）。这样造成的结果就是，接下去执行MoveToUserStack()时，用户态地址
		$1f - 0xC0000000没有映射。缺页，1#进程无法顺利exec (shell)。此外 为了保证1#进程页表中存在指令 $1f - 0xC0000000的映射，
		创建页表后不可以 Expand和EstablishUserPageTable（会将映射擦除）。

		下一个问题，没有显式为1#进程分配针对用户栈的物理空间，exec又是如何执行的？
		答案是，InitUserPageTable()写页表时，将2047#页表映射到了以 0x7ff000为起始地址的页框。这里就是1#进程执行exec系统调用的
		用户栈现场。  exec是不会释放0x7ff000为起始地址的物理页框的，但我们的UserPageManager没有登记这张页面已经分配出去了；不影响
		系统正常运行页面的使用（也就是4M物理空间之上，是1#进程的代码段和可交换部分，然后是一块大的连续内存空间）

		接下来，试试去掉ExecShell，去掉SigHandler。
		*/
		Machine::Instance().InitUserPageTable();
		FlushPageDirectory();
		//us.u_procp->Expand(ProcessManager::USIZE + 0x1000 /* initial stack size */);
		//us.u_MemoryDescriptor.EstablishUserPageTable(0, 0, 0, 0, 0x1000);
		CRT::ClearScreen();
		/* 
		 * MoveToUserStack()使当前1#进程从ring0退出到ring3优先级下，此处调用MoveToUserStack()宏之后
		 * 所有代码都将处于ring3用户态优先级下执行，因此切记这里直至本函数末尾，不可再随意调用内核中
		 * 任何函数，或者引用当前函数中先前定义的变量，因为函数和变量都位于ring0内核优先级中，将会触发
		 * 页表特权级异常。
		 * 或在此处定义变量并引用，会有可能无法访问到变量的正确值或者触发异常，因为从核心栈-->用户栈转移，
		 * esp, ebp指向的内容已经有物理内存位置上的变化，通过esp(ebp) +/- offset访问局部变量，offset都是相对于
		 * 之前核心栈上，由编译器“hardcode”的变量位置偏移量。
		 * NOTE:    ----->Keep the code here as **Simple** as possible!!! <-----
		 */
		//lib_execv("/Shell.exe", NULL);


		MoveToUserStack();
		/* 
		  * 执行到此处时： Machine::InitPageDirectory()中将线性地址0x00000000-0x00400000映射到物理内存0-4M
		  * 尚未被覆盖，0-4M物理内存恰是存放内核映像的区域，这使得我们可以在ring3用户优先级下通过访问用户地址
		  * 空间0x0-0x400000内的线性地址，来访问位于内核映像中的函数，如ExecShell()。
		  * 需要将函数核心态线性地址-0xC0000000计算出函数所在的用户态线性地址；这里需要使用call指令调用函数。
		  */
		//__asm__ __volatile__ ("call *%%eax" :: "a"((unsigned long)ExecShell - (unsigned long)runtime));
		__asm__ __volatile__ ("call *%%eax" :: "a"((unsigned long)ExecShell - 0xC0000000));
		/* Or, use the copy of ExecShell() at physical address 0x0000000.
		  *__asm__ __volatile__ ("call *%%eax" :: "a"((unsigned long)ExecShell - (unsigned long)runtime));
		  */
	}
	while(1);
}



