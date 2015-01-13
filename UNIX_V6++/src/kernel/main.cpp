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
	
	//��Ҫ���жϴ������ĩβ��8259A����EOI����
	//ʵ�鷢�֣���û������IOPort::OutByte(0x27, 0x20);�������Ч����һ����������Ϊ
	//����EOI����֮����к�����IRQ7�жϽ��룬 �������������IRQ7ֻ�����һ�Ρ�
	IOPort::OutByte(Chip8259A::MASTER_IO_PORT_1, Chip8259A::EOI);

	RestoreContext();

	Leave();

	InterruptReturn();
}

extern "C" int main0(void)
{
	Chip8253::Init(20);	//��ʼ��ʱ���ж�оƬ
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
	 * InitPageDirectory()�н����Ե�ַ0-4Mӳ�䵽�����ڴ�
	 * 0-4M��Ϊ��֤��ע����������������β�Ĵ�����ȷִ�У�
	 */

	//ʹ��0x10�μĴ���
	__asm__ __volatile__
		(" \
		mov $0x10, %ax\n\t \
		mov %ax, %ds\n\t \
		mov %ax, %ss\n\t \
		mov %ax, %es\n\t"
		);

	//����ʼ����ջ����Ϊ0xc0400000�������ƻ��˷�װ�ԣ�����ʹ�ø��õķ���
	__asm__ __volatile__
		(
		" \
		mov $0xc0400000, %ebp \n\t \
		mov $0xc0400000, %esp \n\t \
		jmp $0x8, $_next"
		);
	
	//i = 64;	
}

//loadexe()���runtime()�������������Ե�ַ0xC0000000����runtime()����ring0�˳���ring3��Ȩ��֮��ִ�еĴ���
extern "C" void runtime()
{
	/*
	1. ����runtime��stack Frame
	2. esp��ָ���û�ջ��argcλ�ã���ebp��δ��ȷ��ʼ��
	3. eax�д�ſ�ִ�г���EntryPoint
	4~6. exit(0)��������
	*/
	__asm__ __volatile__("	leave;	\
							movl %%esp, %%ebp;	\
							call *%%eax;		\
							movl $1, %%eax;	\
							movl $0, %%ebx;	\
							int $0x80"::);
}

/*
 * �û�̬���еĴ��룬�����û�̬���źŴ�������ȴ�ջ��ȡ����ڵ�ַ��
 * Ȼ�󱣻��ֳ���������ɺ󣬻ָ��ֳ�
 */

/*   ���������ʵ�ֽ��̴Ӻ���̬���û�̬����ִ���źŴ������������ڲ���Ҫ�ˡ�
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
  * 1#������ִ����MoveToUserStack()��ring0�˳���ring3���ȼ��󣬻����ExecShell()���˺���ͨ��"int $0x80"
  * (EAX=execvϵͳ���ú�)���ء�/Shell.exe�������书���൱�����û�������ִ��ϵͳ����execv(char* pathname, char* argv[])��
  * (�˴���Ҫ��������ԭ���ǣ��ں˴��벻��C������һ����룬�����ں��޷�����V6++ libc�е�execvϵͳ���á�)
  */
extern "C" void ExecShell()
{
	int argc = 0;
	char* argv = NULL;
	char* pathname = "/Shell.exe";
	__asm__ __volatile__ ("int $0x80"::"a"(11/* execv */),"b"(pathname),"c"(argc),"d"(argv));
	return;
}

/* ��������ı��runtime()��loadexe()��Щ����Դ�����λ��(���ܻᵼ�������Ų������ʱ����)����*/
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

	//���ʱ��0M-4M���ڴ�ӳ���Ѿ�����ʹ���ˣ�����Ҫ����ӳ���û�̬��ҳ��Ϊ�û�̬������������׼��
	Machine::Instance().InitUserPageTable();
	FlushPageDirectory();

	Machine::Instance().LoadTaskRegister();
	
	/* ��ȡCMOS��ǰʱ�䣬����ϵͳʱ�� */
	struct SystemTime cTime;
	CMOSTime::ReadCMOSTime(&cTime);
	/* MakeKernelTime()������ں�ʱ�䣬��1970��1��1��0ʱ����ǰ������ */
	Time::time = Utility::MakeKernelTime(&cTime);

	/* ��CMOS�л�ȡ�����ڴ��С */
	unsigned short memSize = 0;	/* size in KB */
	unsigned char lowMem, highMem;

	/* ����ֻ�ǽ���CMOSTime���е�ReadCMOSByte������ȡCMOS�������ڴ��С��Ϣ */
	lowMem = CMOSTime::ReadCMOSByte(CMOSTime::EXTENDED_MEMORY_ABOVE_1MB_LOW);
	highMem = CMOSTime::ReadCMOSByte(CMOSTime::EXTENDED_MEMORY_ABOVE_1MB_HIGH);
	memSize = (highMem << 8) + lowMem;

	/* ����1MB���������ڴ����򣬼������ڴ����������ֽ�Ϊ��λ���ڴ��С */
	memSize += 1024; /* KB */
	PageManager::PHY_MEM_SIZE = memSize * 1024;
	UserPageManager::USER_PAGE_POOL_SIZE = PageManager::PHY_MEM_SIZE - UserPageManager::USER_PAGE_POOL_START_ADDR;
	//Diagnose::Write("EXTENDED_MEMORY_ABOVE_1MB = %d KB  (%d MB) (%x)\n", memSize, memSize / 1024, PageManager::PHY_MEM_SIZE);

	/* 
	 * ��������ϵͳ�ں˳�ʼ���߼���
	 * @TODO�ǵ�д�ĵ�����machine�ĳ�ʼ����kernel�ĳ�ʼ�� 
	 */
	Kernel::Instance().Initialize();	
	Kernel::Instance().GetProcessManager().SetupProcessZero();
	isInit = true;

	Kernel::Instance().GetFileSystem().LoadSuperBlock();
	Diagnose::Write("Unix V6++ FileSystem Loaded......OK\n");

	/*  ��ʼ��rootDirInode���û���ǰ����Ŀ¼���Ա�NameI()�������� */
	FileManager& fileMgr = Kernel::Instance().GetFileManager();
	fileMgr.rootDirInode = g_InodeTable.IGet(DeviceManager::ROOTDEV, FileSystem::ROOTINO);
	fileMgr.rootDirInode->i_flag &= (~Inode::ILOCK);

	User& us = Kernel::Instance().GetUser();
	us.u_cdir = g_InodeTable.IGet(DeviceManager::ROOTDEV, FileSystem::ROOTINO);
	us.u_cdir->i_flag &= (~Inode::ILOCK);
	Utility::StringCopy("/", us.u_curdir);

	/* ��TTy�豸 */
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
	
	int pid = Kernel::Instance().GetProcessManager().NewProc();  //0#���̸���ͼ���1#����ʱ��ֻ�ܽ������Ŀռ��ҳ��
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
		/*�����ϵͳ�û�ҳ����г�ʼ���������е�ҳ�� p = 1��R/W = 1���ɶ���д����������Ϊ1#���̸�����0#���̵�
		�����ʵ��ַӳ������ű��������ȫ0����0#���̴���1#���̵�ʱ����ִ�й�MapToPageTable������0#����������ϵͳ�û�ҳ��
		������ȫ0��״̬��MapToPageTable�в���0#ҳ���2047#ҳ�棩��������ɵĽ�����ǣ�����ȥִ��MoveToUserStack()ʱ���û�̬��ַ
		$1f - 0xC0000000û��ӳ�䡣ȱҳ��1#�����޷�˳��exec (shell)������ Ϊ�˱�֤1#����ҳ���д���ָ�� $1f - 0xC0000000��ӳ�䣬
		����ҳ��󲻿��� Expand��EstablishUserPageTable���Ὣӳ���������

		��һ�����⣬û����ʽΪ1#���̷�������û�ջ������ռ䣬exec�������ִ�еģ�
		���ǣ�InitUserPageTable()дҳ��ʱ����2047#ҳ��ӳ�䵽���� 0x7ff000Ϊ��ʼ��ַ��ҳ���������1#����ִ��execϵͳ���õ�
		�û�ջ�ֳ���  exec�ǲ����ͷ�0x7ff000Ϊ��ʼ��ַ������ҳ��ģ������ǵ�UserPageManagerû�еǼ�����ҳ���Ѿ������ȥ�ˣ���Ӱ��
		ϵͳ��������ҳ���ʹ�ã�Ҳ����4M����ռ�֮�ϣ���1#���̵Ĵ���κͿɽ������֣�Ȼ����һ���������ڴ�ռ䣩

		������������ȥ��ExecShell��ȥ��SigHandler��
		*/
		Machine::Instance().InitUserPageTable();
		FlushPageDirectory();
		//us.u_procp->Expand(ProcessManager::USIZE + 0x1000 /* initial stack size */);
		//us.u_MemoryDescriptor.EstablishUserPageTable(0, 0, 0, 0, 0x1000);
		CRT::ClearScreen();
		/* 
		 * MoveToUserStack()ʹ��ǰ1#���̴�ring0�˳���ring3���ȼ��£��˴�����MoveToUserStack()��֮��
		 * ���д��붼������ring3�û�̬���ȼ���ִ�У�����м�����ֱ��������ĩβ����������������ں���
		 * �κκ������������õ�ǰ��������ǰ����ı�������Ϊ�����ͱ�����λ��ring0�ں����ȼ��У����ᴥ��
		 * ҳ����Ȩ���쳣��
		 * ���ڴ˴�������������ã����п����޷����ʵ���������ȷֵ���ߴ����쳣����Ϊ�Ӻ���ջ-->�û�ջת�ƣ�
		 * esp, ebpָ��������Ѿ��������ڴ�λ���ϵı仯��ͨ��esp(ebp) +/- offset���ʾֲ�������offset���������
		 * ֮ǰ����ջ�ϣ��ɱ�������hardcode���ı���λ��ƫ������
		 * NOTE:    ----->Keep the code here as **Simple** as possible!!! <-----
		 */
		//lib_execv("/Shell.exe", NULL);


		MoveToUserStack();
		/* 
		  * ִ�е��˴�ʱ�� Machine::InitPageDirectory()�н����Ե�ַ0x00000000-0x00400000ӳ�䵽�����ڴ�0-4M
		  * ��δ�����ǣ�0-4M�����ڴ�ǡ�Ǵ���ں�ӳ���������ʹ�����ǿ�����ring3�û����ȼ���ͨ�������û���ַ
		  * �ռ�0x0-0x400000�ڵ����Ե�ַ��������λ���ں�ӳ���еĺ�������ExecShell()��
		  * ��Ҫ����������̬���Ե�ַ-0xC0000000������������ڵ��û�̬���Ե�ַ��������Ҫʹ��callָ����ú�����
		  */
		//__asm__ __volatile__ ("call *%%eax" :: "a"((unsigned long)ExecShell - (unsigned long)runtime));
		__asm__ __volatile__ ("call *%%eax" :: "a"((unsigned long)ExecShell - 0xC0000000));
		/* Or, use the copy of ExecShell() at physical address 0x0000000.
		  *__asm__ __volatile__ ("call *%%eax" :: "a"((unsigned long)ExecShell - (unsigned long)runtime));
		  */
	}
	while(1);
}



