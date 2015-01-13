#ifndef USER_H
#define USER_H

#include "MemoryDescriptor.h"
#include "Process.h"
#include "File.h"
#include "INode.h"
#include "FileManager.h"

/*
 * @comment ������Unixv6�� struct user�ṹ��Ӧ�����ֻ�ı�
 * ���������޸ĳ�Ա�ṹ���֣������������͵Ķ�Ӧ��ϵ����:
 */
class User
{
public:
	static const int EAX = 0;	/* u.u_ar0[EAX]�������ֳ���������EAX�Ĵ�����ƫ���� */
	
	/* u_error's Error Code */
	enum ErrorCode
	{
		NOERROR	= 0,
		EPERM	= 1,
		ENOENT	= 2,
		ESRCH	= 3,
		EINTR	= 4,
		EIO		= 5,
		ENXIO	= 6,
		E2BIG	= 7,
		ENOEXEC	= 8,
		EBADF	= 9,
		ECHILD	= 10,
		EAGAIN	= 11,
		ENOMEM	= 12,
		EACCES	= 13,
		ENOTBLK	= 15,
		EBUSY 	= 16,
		EEXIST	= 17,
		EXDEV	= 18,
		ENODEV	= 19,
		ENOTDIR	= 20,
		EISDIR	= 21,
		EINVAL	= 22,
		ENFILE	= 23,
		EMFILE	= 24,
		ENOTTY	= 25,
		ETXTBSY	= 26,
		EFBIG	= 27,
		ENOSPC	= 28,
		ESPIPE	= 29,
		EROFS	= 30,
		EMLINK	= 31,
		EPIPE	= 32,
		ENOSYS	= 100,
		EFAULT	= 106
	};

	static const int NSIG = 32;	/* �źŸ��� */

	/* p_sig�н��ܵ����źŶ��� */
	static const int SIGNUL = 0;	/* No Signal Received */
	static const int SIGHUP = 1;	/* Hangup (kill controlling terminal) */
	static const int SIGINT = 2;    /* Interrupt from keyboard */
	static const int SIGQUIT = 3;	/* Quit from keyboard */
	static const int SIGILL = 4;	/* Illegal instrution */
	static const int SIGTRAP = 5;	/* Trace trap */
	static const int SIGABRT = 6;	/* use abort() API */
	static const int SIGBUS = 7;	/* Bus error */
	static const int SIGFPE = 8;	/* Floating point exception */
	static const int SIGKILL = 9;	/* Kill(can't be caught or ignored) */
	static const int SIGUSR1 = 10;	/* User defined signal 1 */
	static const int SIGSEGV = 11;	/* Invalid memory segment access */
	static const int SIGUSR2 = 12;	/* User defined signal 2 */
	static const int SIGPIPE = 13;	/* Write on a pipe with no reader, Broken pipe */
	static const int SIGALRM = 14;	/* Alarm clock */
	static const int SIGTERM = 15;	/* Termination */
	static const int SIGSTKFLT = 16; /* Stack fault */
	static const int SIGCHLD = 17; /* Child process has stopped or exited, changed */
	static const int SIGCONT = 18; /* Continue executing, if stopped */
	static const int SIGSTOP = 19; /* Stop executing */
	static const int SIGTSTP = 20; /* Terminal stop signal */
	static const int SIGTTIN = 21; /* Background process trying to read, from TTY */
	static const int SIGTTOU = 22; /* Background process trying to write, to TTY */
	static const int SIGURG = 23;  /* Urgent condition on socket */
	static const int SIGXCPU = 24; /* CPU limit exceeded */
	static const int SIGXFSZ = 25; /* File size limit exceeded */
	static const int SIGVTALRM = 26; /* Virtual alarm clock */
	static const int SIGPROF = 27; /* Profiling alarm clock */
	static const int SIGWINCH = 28; /* Window size change */
	static const int SIGIO = 29; /* I/O now possible */
	static const int SIGPWR = 30; /* Power failure restart */
	static const int SIGSYS = 31; /* invalid sys call */

public:
	unsigned long u_rsav[2];	/* ���ڱ���esp��ebpָ�� */
	unsigned long u_ssav[2];	/* ���ڶ�esp��ebpָ��Ķ��α��� */
	Process* u_procp;			/* ָ���u�ṹ��Ӧ��Process�ṹ */
	/* ����ӱ������������ԭ�еı���
	 * int u_uisa[16]
	 * int u_uisd[16]
	 * u_tsize
	 * u_dsize
	 * u_ssize
	 */
	MemoryDescriptor u_MemoryDescriptor;

	/* ϵͳ������س�Ա */
	unsigned int	*u_ar0;		/* ָ�����ջ�ֳ���������EAX�Ĵ���
								��ŵ�ջ��Ԫ�����ֶδ�Ÿ�ջ��Ԫ�ĵ�ַ��
								��V6��r0���ϵͳ���õķ���ֵ���û�����
								x86ƽ̨��ʹ��EAX��ŷ���ֵ�����u.u_ar0[R0] */

	int u_arg[5];				/* ��ŵ�ǰϵͳ���ò��� */
	char* u_dirp;				/* ϵͳ���ò���(һ������Pathname)��ָ�� */

	/* ʱ����س�Ա */
	int u_utime;		/* �����û�̬ʱ�� */
	int u_stime;		/* ���̺���̬ʱ�� */
	int u_cutime;		/* �ӽ����û�̬ʱ���ܺ� */
	int u_cstime;		/* �ӽ��̺���̬ʱ���ܺ� */
	
	/* �źŴ�����س�Ա */
	unsigned long u_signal[NSIG];	/* �źŴ���� */
	unsigned long u_qsav[2];		/* ���ڽ��յ��ź�ʱֱ�Ӵ�Sleep()����������Trap() */
	bool u_intflg;		/* ϵͳ�����ڼ��Ƿ��ܵ��źŴ�� ��1��ʾ����ϡ�0��ʾδ�����*/
	
	/* �ļ�ϵͳ��س�Ա */
	Inode* u_cdir;		/* ָ��ǰĿ¼��Inodeָ�� */
	Inode* u_pdir;		/* ָ��Ŀ¼��Inodeָ�� */

	DirectoryEntry u_dent;					/* ��ǰĿ¼��Ŀ¼�� */
	char u_dbuf[DirectoryEntry::DIRSIZ];	/* ��ǰ·������ */
	char u_curdir[128];						/* ��ǰ����Ŀ¼����·�� */

	ErrorCode u_error;			/* ��Ŵ����� */
	int u_segflg;				/* ����I/O����û���ϵͳ�ռ� */

	/* ���̵��û���ʶ */
	short u_uid;		/* ��Ч�û�ID */
	short u_gid;		/* ��Ч��ID */
	short u_ruid;		/* ��ʵ�û�ID */
	short u_rgid;		/* ��ʵ��ID */
	
	/* �ļ�ϵͳ��س�Ա */
	OpenFiles u_ofiles;		/* ���̴��ļ������������ */

	/* �ļ�I/O���� */
	IOParameter u_IOParam;	/* ��¼��ǰ����д�ļ���ƫ�������û�Ŀ�������ʣ���ֽ������� */

	/* Member Functions */
public:
	/* ����ϵͳ���ò���uid������Ч�û�ID����ʵ�û�ID�������û�ID(p_uid) */
	void Setuid();

	/* ��ȡ�û�ID����16����Ϊ��ʵ�û�ID(u_ruid)����16����Ϊ��Ч�û�ID(u_uid) */
	void Getuid();

	/* ����ϵͳ���ò���gid������Ч��ID����ʵ��ID */
	void Setgid();

	/* ��ȡ��ID, ��16����Ϊ��ʵ��ID(u_rgid)����16����Ϊ��Ч��ID(u_gid) */
	void Getgid();

	/* ��ȡ��ǰ�û�����Ŀ¼ */
	void Pwd();

	/* ��鵱ǰ�û��Ƿ��ǳ����û� */
	bool SUser();
};

#endif

