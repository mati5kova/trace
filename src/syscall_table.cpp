//
// Created by matevz on 7/1/26.
//

#include "trace/syscall_table.hpp"

trace::syscall_table::SyscallInfo trace::syscall_table::get_syscall_info_from_nr(const unsigned long nr) {
    switch (nr)
    {
    case 0: return {
	    .nr = 0,
	    .name = "io_setup",
	    .args = {
		    {0, "unsigned nr_reqs"},
		    {1, "aio_context_t *ctx"},
	    }
    };
	case 1: return {
		.nr = 1,
		.name = "io_destroy",
		.args = {
			{0, "aio_context_t ctx"},
		}
	};
	case 2: return {
		.nr = 2,
		.name = "io_submit",
		.args = {
			{0, "aio_context_t"},
			{1, "long"},
			{2, "struct iocb * *"},
		}
	};
	case 3: return {
		.nr = 3,
		.name = "io_cancel",
		.args = {
			{0, "aio_context_t ctx_id"},
			{1, "struct iocb *iocb"},
			{2, "struct io_event *result"},
		}
	};
	case 4: return {
		.nr = 4,
		.name = "io_getevents",
		.args = {
			{0, "aio_context_t ctx_id"},
			{1, "long min_nr"},
			{2, "long nr"},
			{3, "struct io_event *events"},
			{4, "struct __kernel_timespec *timeout"},
		}
	};
	case 5: return {
		.nr = 5,
		.name = "setxattr",
		.args = {
			{0, "const char *path"},
			{1, "const char *name"},
			{2, "const void *value"},
			{3, "size_t size"},
			{4, "int flags"},
		}
	};
	case 6: return {
		.nr = 6,
		.name = "lsetxattr",
		.args = {
			{0, "const char *path"},
			{1, "const char *name"},
			{2, "const void *value"},
			{3, "size_t size"},
			{4, "int flags"},
		}
	};
	case 7: return {
		.nr = 7,
		.name = "fsetxattr",
		.args = {
			{0, "int fd"},
			{1, "const char *name"},
			{2, "const void *value"},
			{3, "size_t size"},
			{4, "int flags"},
		}
	};
	case 8: return {
		.nr = 8,
		.name = "getxattr",
		.args = {
			{0, "const char *path"},
			{1, "const char *name"},
			{2, "void *value"},
			{3, "size_t size"},
		}
	};
	case 9: return {
		.nr = 9,
		.name = "lgetxattr",
		.args = {
			{0, "const char *path"},
			{1, "const char *name"},
			{2, "void *value"},
			{3, "size_t size"},
		}
	};
	case 10: return {
		.nr = 10,
		.name = "fgetxattr",
		.args = {
			{0, "int fd"},
			{1, "const char *name"},
			{2, "void *value"},
			{3, "size_t size"},
		}
	};
	case 11: return {
		.nr = 11,
		.name = "listxattr",
		.args = {
			{0, "const char *path"},
			{1, "char *list"},
			{2, "size_t size"},
		}
	};
	case 12: return {
		.nr = 12,
		.name = "llistxattr",
		.args = {
			{0, "const char *path"},
			{1, "char *list"},
			{2, "size_t size"},
		}
	};
	case 13: return {
		.nr = 13,
		.name = "flistxattr",
		.args = {
			{0, "int fd"},
			{1, "char *list"},
			{2, "size_t size"},
		}
	};
	case 14: return {
		.nr = 14,
		.name = "removexattr",
		.args = {
			{0, "const char *path"},
			{1, "const char *name"},
		}
	};
	case 15: return {
		.nr = 15,
		.name = "lremovexattr",
		.args = {
			{0, "const char *path"},
			{1, "const char *name"},
		}
	};
	case 16: return {
		.nr = 16,
		.name = "fremovexattr",
		.args = {
			{0, "int fd"},
			{1, "const char *name"},
		}
	};
	case 17: return {
		.nr = 17,
		.name = "getcwd",
		.args = {
			{0, "char *buf"},
			{1, "unsigned long size"},
		}
	};
	case 18: return {
		.nr = 18,
		.name = "lookup_dcookie",
		.args = {
			{0, "u64 cookie64"},
			{1, "char *buf"},
			{2, "size_t len"},
		}
	};
	case 19: return {
		.nr = 19,
		.name = "eventfd2",
		.args = {
			{0, "unsigned int count"},
			{1, "int flags"},
		}
	};
	case 20: return {
		.nr = 20,
		.name = "epoll_create1",
		.args = {
			{0, "int flags"},
		}
	};
	case 21: return {
		.nr = 21,
		.name = "epoll_ctl",
		.args = {
			{0, "int epfd"},
			{1, "int op"},
			{2, "int fd"},
			{3, "struct epoll_event *event"},
		}
	};
	case 22: return {
		.nr = 22,
		.name = "epoll_pwait",
		.args = {
			{0, "int epfd"},
			{1, "struct epoll_event *events"},
			{2, "int maxevents"},
			{3, "int timeout"},
			{4, "const sigset_t *sigmask"},
			{5, "size_t sigsetsize"},
		}
	};
	case 23: return {
		.nr = 23,
		.name = "dup",
		.args = {
			{0, "unsigned int fildes"},
		}
	};
	case 24: return {
		.nr = 24,
		.name = "dup3",
		.args = {
			{0, "unsigned int oldfd"},
			{1, "unsigned int newfd"},
			{2, "int flags"},
		}
	};
	case 25: return {
		.nr = 25,
		.name = "fcntl",
		.args = {
			{0, "unsigned int fd"},
			{1, "unsigned int cmd"},
			{2, "unsigned long arg"},
		}
	};
	case 26: return {
		.nr = 26,
		.name = "inotify_init1",
		.args = {
			{0, "int flags"},
		}
	};
	case 27: return {
		.nr = 27,
		.name = "inotify_add_watch",
		.args = {
			{0, "int fd"},
			{1, "const char *path"},
			{2, "u32 mask"},
		}
	};
	case 28: return {
		.nr = 28,
		.name = "inotify_rm_watch",
		.args = {
			{0, "int fd"},
			{1, "__s32 wd"},
		}
	};
	case 29: return {
		.nr = 29,
		.name = "ioctl",
		.args = {
			{0, "unsigned int fd"},
			{1, "unsigned int cmd"},
			{2, "unsigned long arg"},
		}
	};
	case 30: return {
		.nr = 30,
		.name = "ioprio_set",
		.args = {
			{0, "int which"},
			{1, "int who"},
			{2, "int ioprio"},
		}
	};
	case 31: return {
		.nr = 31,
		.name = "ioprio_get",
		.args = {
			{0, "int which"},
			{1, "int who"},
		}
	};
	case 32: return {
		.nr = 32,
		.name = "flock",
		.args = {
			{0, "unsigned int fd"},
			{1, "unsigned int cmd"},
		}
	};
	case 33: return {
		.nr = 33,
		.name = "mknodat",
		.args = {
			{0, "int dfd"},
			{1, "const char * filename"},
			{2, "umode_t mode"},
			{3, "unsigned dev"},
		}
	};
	case 34: return {
		.nr = 34,
		.name = "mkdirat",
		.args = {
			{0, "int dfd"},
			{1, "const char * pathname"},
			{2, "umode_t mode"},
		}
	};
	case 35: return {
		.nr = 35,
		.name = "unlinkat",
		.args = {
			{0, "int dfd"},
			{1, "const char * pathname"},
			{2, "int flag"},
		}
	};
	case 36: return {
		.nr = 36,
		.name = "symlinkat",
		.args = {
			{0, "const char * oldname"},
			{1, "int newdfd"},
			{2, "const char * newname"},
		}
	};
	case 37: return {
		.nr = 37,
		.name = "linkat",
		.args = {
			{0, "int olddfd"},
			{1, "const char *oldname"},
			{2, "int newdfd"},
			{3, "const char *newname"},
			{4, "int flags"},
		}
	};
	case 38: return {
		.nr = 38,
		.name = "renameat",
		.args = {
			{0, "int olddfd"},
			{1, "const char * oldname"},
			{2, "int newdfd"},
			{3, "const char * newname"},
		}
	};
	case 39: return {
		.nr = 39,
		.name = "umount2",
		.args = {
			{0, "?"},
			{1, "?"},
			{2, "?"},
			{3, "?"},
			{4, "?"},
			{5, "?"},
		}
	};
	case 40: return {
		.nr = 40,
		.name = "mount",
		.args = {
			{0, "char *dev_name"},
			{1, "char *dir_name"},
			{2, "char *type"},
			{3, "unsigned long flags"},
			{4, "void *data"},
		}
	};
	case 41: return {
		.nr = 41,
		.name = "pivot_root",
		.args = {
			{0, "const char *new_root"},
			{1, "const char *put_old"},
		}
	};
	case 42: return {
		.nr = 42,
		.name = "nfsservctl",
		.args = {
			{0, "?"},
			{1, "?"},
			{2, "?"},
			{3, "?"},
			{4, "?"},
			{5, "?"},
		}
	};
	case 43: return {
		.nr = 43,
		.name = "statfs",
		.args = {
			{0, "const char * path"},
			{1, "struct statfs *buf"},
		}
	};
	case 44: return {
		.nr = 44,
		.name = "fstatfs",
		.args = {
			{0, "unsigned int fd"},
			{1, "struct statfs *buf"},
		}
	};
	case 45: return {
		.nr = 45,
		.name = "truncate",
		.args = {
			{0, "const char *path"},
			{1, "long length"},
		}
	};
	case 46: return {
		.nr = 46,
		.name = "ftruncate",
		.args = {
			{0, "unsigned int fd"},
			{1, "unsigned long length"},
		}
	};
	case 47: return {
		.nr = 47,
		.name = "fallocate",
		.args = {
			{0, "int fd"},
			{1, "int mode"},
			{2, "loff_t offset"},
			{3, "loff_t len"},
		}
	};
	case 48: return {
		.nr = 48,
		.name = "faccessat",
		.args = {
			{0, "int dfd"},
			{1, "const char *filename"},
			{2, "int mode"},
		}
	};
	case 49: return {
		.nr = 49,
		.name = "chdir",
		.args = {
			{0, "const char *filename"},
		}
	};
	case 50: return {
		.nr = 50,
		.name = "fchdir",
		.args = {
			{0, "unsigned int fd"},
		}
	};
	case 51: return {
		.nr = 51,
		.name = "chroot",
		.args = {
			{0, "const char *filename"},
		}
	};
	case 52: return {
		.nr = 52,
		.name = "fchmod",
		.args = {
			{0, "unsigned int fd"},
			{1, "umode_t mode"},
		}
	};
	case 53: return {
		.nr = 53,
		.name = "fchmodat",
		.args = {
			{0, "int dfd"},
			{1, "const char * filename"},
			{2, "umode_t mode"},
		}
	};
	case 54: return {
		.nr = 54,
		.name = "fchownat",
		.args = {
			{0, "int dfd"},
			{1, "const char *filename"},
			{2, "uid_t user"},
			{3, "gid_t group"},
			{4, "int flag"},
		}
	};
	case 55: return {
		.nr = 55,
		.name = "fchown",
		.args = {
			{0, "unsigned int fd"},
			{1, "uid_t user"},
			{2, "gid_t group"},
		}
	};
	case 56: return {
		.nr = 56,
		.name = "openat",
		.args = {
			{0, "int dfd"},
			{1, "const char *filename"},
			{2, "int flags"},
			{3, "umode_t mode"},
		}
	};
	case 57: return {
		.nr = 57,
		.name = "close",
		.args = {
			{0, "unsigned int fd"},
		}
	};
	case 58: return {
		.nr = 58,
		.name = "vhangup",
		.args = {
		}
	};
	case 59: return {
		.nr = 59,
		.name = "pipe2",
		.args = {
			{0, "int *fildes"},
			{1, "int flags"},
		}
	};
	case 60: return {
		.nr = 60,
		.name = "quotactl",
		.args = {
			{0, "unsigned int cmd"},
			{1, "const char *special"},
			{2, "qid_t id"},
			{3, "void *addr"},
		}
	};
	case 61: return {
		.nr = 61,
		.name = "getdents64",
		.args = {
			{0, "unsigned int fd"},
			{1, "struct linux_dirent64 *dirent"},
			{2, "unsigned int count"},
		}
	};
	case 62: return {
		.nr = 62,
		.name = "lseek",
		.args = {
			{0, "unsigned int fd"},
			{1, "off_t offset"},
			{2, "unsigned int whence"},
		}
	};
	case 63: return {
		.nr = 63,
		.name = "read",
		.args = {
			{0, "unsigned int fd"},
			{1, "char *buf"},
			{2, "size_t count"},
		}
	};
	case 64: return {
		.nr = 64,
		.name = "write",
		.args = {
			{0, "unsigned int fd"},
			{1, "const char *buf"},
			{2, "size_t count"},
		}
	};
	case 65: return {
		.nr = 65,
		.name = "readv",
		.args = {
			{0, "unsigned long fd"},
			{1, "const struct iovec *vec"},
			{2, "unsigned long vlen"},
		}
	};
	case 66: return {
		.nr = 66,
		.name = "writev",
		.args = {
			{0, "unsigned long fd"},
			{1, "const struct iovec *vec"},
			{2, "unsigned long vlen"},
		}
	};
	case 67: return {
		.nr = 67,
		.name = "pread64",
		.args = {
			{0, "unsigned int fd"},
			{1, "char *buf"},
			{2, "size_t count"},
			{3, "loff_t pos"},
		}
	};
	case 68: return {
		.nr = 68,
		.name = "pwrite64",
		.args = {
			{0, "unsigned int fd"},
			{1, "const char *buf"},
			{2, "size_t count"},
			{3, "loff_t pos"},
		}
	};
	case 69: return {
		.nr = 69,
		.name = "preadv",
		.args = {
			{0, "unsigned long fd"},
			{1, "const struct iovec *vec"},
			{2, "unsigned long vlen"},
			{3, "unsigned long pos_l"},
			{4, "unsigned long pos_h"},
		}
	};
	case 70: return {
		.nr = 70,
		.name = "pwritev",
		.args = {
			{0, "unsigned long fd"},
			{1, "const struct iovec *vec"},
			{2, "unsigned long vlen"},
			{3, "unsigned long pos_l"},
			{4, "unsigned long pos_h"},
		}
	};
	case 71: return {
		.nr = 71,
		.name = "sendfile",
		.args = {
			{0, "int out_fd"},
			{1, "int in_fd"},
			{2, "off_t *offset"},
			{3, "size_t count"},
		}
	};
	case 72: return {
		.nr = 72,
		.name = "pselect6",
		.args = {
			{0, "int"},
			{1, "fd_set *"},
			{2, "fd_set *"},
			{3, "fd_set *"},
			{4, "struct __kernel_timespec *"},
			{5, "void *"},
		}
	};
	case 73: return {
		.nr = 73,
		.name = "ppoll",
		.args = {
			{0, "struct pollfd *"},
			{1, "unsigned int"},
			{2, "struct __kernel_timespec *"},
			{3, "const sigset_t *"},
			{4, "size_t"},
		}
	};
	case 74: return {
		.nr = 74,
		.name = "signalfd4",
		.args = {
			{0, "int ufd"},
			{1, "sigset_t *user_mask"},
			{2, "size_t sizemask"},
			{3, "int flags"},
		}
	};
	case 75: return {
		.nr = 75,
		.name = "vmsplice",
		.args = {
			{0, "int fd"},
			{1, "const struct iovec *iov"},
			{2, "unsigned long nr_segs"},
			{3, "unsigned int flags"},
		}
	};
	case 76: return {
		.nr = 76,
		.name = "splice",
		.args = {
			{0, "int fd_in"},
			{1, "loff_t *off_in"},
			{2, "int fd_out"},
			{3, "loff_t *off_out"},
			{4, "size_t len"},
			{5, "unsigned int flags"},
		}
	};
	case 77: return {
		.nr = 77,
		.name = "tee",
		.args = {
			{0, "int fdin"},
			{1, "int fdout"},
			{2, "size_t len"},
			{3, "unsigned int flags"},
		}
	};
	case 78: return {
		.nr = 78,
		.name = "readlinkat",
		.args = {
			{0, "int dfd"},
			{1, "const char *path"},
			{2, "char *buf"},
			{3, "int bufsiz"},
		}
	};
	case 79: return {
		.nr = 79,
		.name = "newfstatat",
		.args = {
			{0, "int dfd"},
			{1, "const char *filename"},
			{2, "struct stat *statbuf"},
			{3, "int flag"},
		}
	};
	case 80: return {
		.nr = 80,
		.name = "fstat",
		.args = {
			{0, "unsigned int fd"},
			{1, "struct __old_kernel_stat *statbuf"},
		}
	};
	case 81: return {
		.nr = 81,
		.name = "sync",
		.args = {
		}
	};
	case 82: return {
		.nr = 82,
		.name = "fsync",
		.args = {
			{0, "unsigned int fd"},
		}
	};
	case 83: return {
		.nr = 83,
		.name = "fdatasync",
		.args = {
			{0, "unsigned int fd"},
		}
	};
	case 84: return {
		.nr = 84,
		.name = "sync_file_range",
		.args = {
			{0, "int fd"},
			{1, "loff_t offset"},
			{2, "loff_t nbytes"},
			{3, "unsigned int flags"},
		}
	};
	case 85: return {
		.nr = 85,
		.name = "timerfd_create",
		.args = {
			{0, "int clockid"},
			{1, "int flags"},
		}
	};
	case 86: return {
		.nr = 86,
		.name = "timerfd_settime",
		.args = {
			{0, "int ufd"},
			{1, "int flags"},
			{2, "const struct __kernel_itimerspec *utmr"},
			{3, "struct __kernel_itimerspec *otmr"},
		}
	};
	case 87: return {
		.nr = 87,
		.name = "timerfd_gettime",
		.args = {
			{0, "int ufd"},
			{1, "struct __kernel_itimerspec *otmr"},
		}
	};
	case 88: return {
		.nr = 88,
		.name = "utimensat",
		.args = {
			{0, "int dfd"},
			{1, "const char *filename"},
			{2, "struct __kernel_timespec *utimes"},
			{3, "int flags"},
		}
	};
	case 89: return {
		.nr = 89,
		.name = "acct",
		.args = {
			{0, "const char *name"},
		}
	};
	case 90: return {
		.nr = 90,
		.name = "capget",
		.args = {
			{0, "cap_user_header_t header"},
			{1, "cap_user_data_t dataptr"},
		}
	};
	case 91: return {
		.nr = 91,
		.name = "capset",
		.args = {
			{0, "cap_user_header_t header"},
			{1, "const cap_user_data_t data"},
		}
	};
	case 92: return {
		.nr = 92,
		.name = "personality",
		.args = {
			{0, "unsigned int personality"},
		}
	};
	case 93: return {
		.nr = 93,
		.name = "exit",
		.args = {
			{0, "int error_code"},
		}
	};
	case 94: return {
		.nr = 94,
		.name = "exit_group",
		.args = {
			{0, "int error_code"},
		}
	};
	case 95: return {
		.nr = 95,
		.name = "waitid",
		.args = {
			{0, "int which"},
			{1, "pid_t pid"},
			{2, "struct siginfo *infop"},
			{3, "int options"},
			{4, "struct rusage *ru"},
		}
	};
	case 96: return {
		.nr = 96,
		.name = "set_tid_address",
		.args = {
			{0, "int *tidptr"},
		}
	};
	case 97: return {
		.nr = 97,
		.name = "unshare",
		.args = {
			{0, "unsigned long unshare_flags"},
		}
	};
	case 98: return {
		.nr = 98,
		.name = "futex",
		.args = {
			{0, "u32 *uaddr"},
			{1, "int op"},
			{2, "u32 val"},
			{3, "struct __kernel_timespec *utime"},
			{4, "u32 *uaddr2"},
			{5, "u32 val3"},
		}
	};
	case 99: return {
		.nr = 99,
		.name = "set_robust_list",
		.args = {
			{0, "struct robust_list_head *head"},
			{1, "size_t len"},
		}
	};
	case 100: return {
		.nr = 100,
		.name = "get_robust_list",
		.args = {
			{0, "int pid"},
			{1, "struct robust_list_head * *head_ptr"},
			{2, "size_t *len_ptr"},
		}
	};
	case 101: return {
		.nr = 101,
		.name = "nanosleep",
		.args = {
			{0, "struct __kernel_timespec *rqtp"},
			{1, "struct __kernel_timespec *rmtp"},
		}
	};
	case 102: return {
		.nr = 102,
		.name = "getitimer",
		.args = {
			{0, "int which"},
			{1, "struct itimerval *value"},
		}
	};
	case 103: return {
		.nr = 103,
		.name = "setitimer",
		.args = {
			{0, "int which"},
			{1, "struct itimerval *value"},
			{2, "struct itimerval *ovalue"},
		}
	};
	case 104: return {
		.nr = 104,
		.name = "kexec_load",
		.args = {
			{0, "unsigned long entry"},
			{1, "unsigned long nr_segments"},
			{2, "struct kexec_segment *segments"},
			{3, "unsigned long flags"},
		}
	};
	case 105: return {
		.nr = 105,
		.name = "init_module",
		.args = {
			{0, "void *umod"},
			{1, "unsigned long len"},
			{2, "const char *uargs"},
		}
	};
	case 106: return {
		.nr = 106,
		.name = "delete_module",
		.args = {
			{0, "const char *name_user"},
			{1, "unsigned int flags"},
		}
	};
	case 107: return {
		.nr = 107,
		.name = "timer_create",
		.args = {
			{0, "clockid_t which_clock"},
			{1, "struct sigevent *timer_event_spec"},
			{2, "timer_t * created_timer_id"},
		}
	};
	case 108: return {
		.nr = 108,
		.name = "timer_gettime",
		.args = {
			{0, "timer_t timer_id"},
			{1, "struct __kernel_itimerspec *setting"},
		}
	};
	case 109: return {
		.nr = 109,
		.name = "timer_getoverrun",
		.args = {
			{0, "timer_t timer_id"},
		}
	};
	case 110: return {
		.nr = 110,
		.name = "timer_settime",
		.args = {
			{0, "timer_t timer_id"},
			{1, "int flags"},
			{2, "const struct __kernel_itimerspec *new_setting"},
			{3, "struct __kernel_itimerspec *old_setting"},
		}
	};
	case 111: return {
		.nr = 111,
		.name = "timer_delete",
		.args = {
			{0, "timer_t timer_id"},
		}
	};
	case 112: return {
		.nr = 112,
		.name = "clock_settime",
		.args = {
			{0, "clockid_t which_clock"},
			{1, "const struct __kernel_timespec *tp"},
		}
	};
	case 113: return {
		.nr = 113,
		.name = "clock_gettime",
		.args = {
			{0, "clockid_t which_clock"},
			{1, "struct __kernel_timespec *tp"},
		}
	};
	case 114: return {
		.nr = 114,
		.name = "clock_getres",
		.args = {
			{0, "clockid_t which_clock"},
			{1, "struct __kernel_timespec *tp"},
		}
	};
	case 115: return {
		.nr = 115,
		.name = "clock_nanosleep",
		.args = {
			{0, "clockid_t which_clock"},
			{1, "int flags"},
			{2, "const struct __kernel_timespec *rqtp"},
			{3, "struct __kernel_timespec *rmtp"},
		}
	};
	case 116: return {
		.nr = 116,
		.name = "syslog",
		.args = {
			{0, "int type"},
			{1, "char *buf"},
			{2, "int len"},
		}
	};
	case 117: return {
		.nr = 117,
		.name = "ptrace",
		.args = {
			{0, "long request"},
			{1, "long pid"},
			{2, "unsigned long addr"},
			{3, "unsigned long data"},
		}
	};
	case 118: return {
		.nr = 118,
		.name = "sched_setparam",
		.args = {
			{0, "pid_t pid"},
			{1, "struct sched_param *param"},
		}
	};
	case 119: return {
		.nr = 119,
		.name = "sched_setscheduler",
		.args = {
			{0, "pid_t pid"},
			{1, "int policy"},
			{2, "struct sched_param *param"},
		}
	};
	case 120: return {
		.nr = 120,
		.name = "sched_getscheduler",
		.args = {
			{0, "pid_t pid"},
		}
	};
	case 121: return {
		.nr = 121,
		.name = "sched_getparam",
		.args = {
			{0, "pid_t pid"},
			{1, "struct sched_param *param"},
		}
	};
	case 122: return {
		.nr = 122,
		.name = "sched_setaffinity",
		.args = {
			{0, "pid_t pid"},
			{1, "unsigned int len"},
			{2, "unsigned long *user_mask_ptr"},
		}
	};
	case 123: return {
		.nr = 123,
		.name = "sched_getaffinity",
		.args = {
			{0, "pid_t pid"},
			{1, "unsigned int len"},
			{2, "unsigned long *user_mask_ptr"},
		}
	};
	case 124: return {
		.nr = 124,
		.name = "sched_yield",
		.args = {
		}
	};
	case 125: return {
		.nr = 125,
		.name = "sched_get_priority_max",
		.args = {
			{0, "int policy"},
		}
	};
	case 126: return {
		.nr = 126,
		.name = "sched_get_priority_min",
		.args = {
			{0, "int policy"},
		}
	};
	case 127: return {
		.nr = 127,
		.name = "sched_rr_get_interval",
		.args = {
			{0, "pid_t pid"},
			{1, "struct __kernel_timespec *interval"},
		}
	};
	case 128: return {
		.nr = 128,
		.name = "restart_syscall",
		.args = {
		}
	};
	case 129: return {
		.nr = 129,
		.name = "kill",
		.args = {
			{0, "pid_t pid"},
			{1, "int sig"},
		}
	};
	case 130: return {
		.nr = 130,
		.name = "tkill",
		.args = {
			{0, "pid_t pid"},
			{1, "int sig"},
		}
	};
	case 131: return {
		.nr = 131,
		.name = "tgkill",
		.args = {
			{0, "pid_t tgid"},
			{1, "pid_t pid"},
			{2, "int sig"},
		}
	};
	case 132: return {
		.nr = 132,
		.name = "sigaltstack",
		.args = {
			{0, "const struct sigaltstack *uss"},
			{1, "struct sigaltstack *uoss"},
		}
	};
	case 133: return {
		.nr = 133,
		.name = "rt_sigsuspend",
		.args = {
			{0, "sigset_t *unewset"},
			{1, "size_t sigsetsize"},
		}
	};
	case 134: return {
		.nr = 134,
		.name = "rt_sigaction",
		.args = {
			{0, "int"},
			{1, "const struct sigaction *"},
			{2, "struct sigaction *"},
			{3, "size_t"},
		}
	};
	case 135: return {
		.nr = 135,
		.name = "rt_sigprocmask",
		.args = {
			{0, "int how"},
			{1, "sigset_t *set"},
			{2, "sigset_t *oset"},
			{3, "size_t sigsetsize"},
		}
	};
	case 136: return {
		.nr = 136,
		.name = "rt_sigpending",
		.args = {
			{0, "sigset_t *set"},
			{1, "size_t sigsetsize"},
		}
	};
	case 137: return {
		.nr = 137,
		.name = "rt_sigtimedwait",
		.args = {
			{0, "const sigset_t *uthese"},
			{1, "siginfo_t *uinfo"},
			{2, "const struct __kernel_timespec *uts"},
			{3, "size_t sigsetsize"},
		}
	};
	case 138: return {
		.nr = 138,
		.name = "rt_sigqueueinfo",
		.args = {
			{0, "pid_t pid"},
			{1, "int sig"},
			{2, "siginfo_t *uinfo"},
		}
	};
	case 139: return {
		.nr = 139,
		.name = "rt_sigreturn",
		.args = {
			{0, "?"},
			{1, "?"},
			{2, "?"},
			{3, "?"},
			{4, "?"},
			{5, "?"},
		}
	};
	case 140: return {
		.nr = 140,
		.name = "setpriority",
		.args = {
			{0, "int which"},
			{1, "int who"},
			{2, "int niceval"},
		}
	};
	case 141: return {
		.nr = 141,
		.name = "getpriority",
		.args = {
			{0, "int which"},
			{1, "int who"},
		}
	};
	case 142: return {
		.nr = 142,
		.name = "reboot",
		.args = {
			{0, "int magic1"},
			{1, "int magic2"},
			{2, "unsigned int cmd"},
			{3, "void *arg"},
		}
	};
	case 143: return {
		.nr = 143,
		.name = "setregid",
		.args = {
			{0, "gid_t rgid"},
			{1, "gid_t egid"},
		}
	};
	case 144: return {
		.nr = 144,
		.name = "setgid",
		.args = {
			{0, "gid_t gid"},
		}
	};
	case 145: return {
		.nr = 145,
		.name = "setreuid",
		.args = {
			{0, "uid_t ruid"},
			{1, "uid_t euid"},
		}
	};
	case 146: return {
		.nr = 146,
		.name = "setuid",
		.args = {
			{0, "uid_t uid"},
		}
	};
	case 147: return {
		.nr = 147,
		.name = "setresuid",
		.args = {
			{0, "uid_t ruid"},
			{1, "uid_t euid"},
			{2, "uid_t suid"},
		}
	};
	case 148: return {
		.nr = 148,
		.name = "getresuid",
		.args = {
			{0, "uid_t *ruid"},
			{1, "uid_t *euid"},
			{2, "uid_t *suid"},
		}
	};
	case 149: return {
		.nr = 149,
		.name = "setresgid",
		.args = {
			{0, "gid_t rgid"},
			{1, "gid_t egid"},
			{2, "gid_t sgid"},
		}
	};
	case 150: return {
		.nr = 150,
		.name = "getresgid",
		.args = {
			{0, "gid_t *rgid"},
			{1, "gid_t *egid"},
			{2, "gid_t *sgid"},
		}
	};
	case 151: return {
		.nr = 151,
		.name = "setfsuid",
		.args = {
			{0, "uid_t uid"},
		}
	};
	case 152: return {
		.nr = 152,
		.name = "setfsgid",
		.args = {
			{0, "gid_t gid"},
		}
	};
	case 153: return {
		.nr = 153,
		.name = "times",
		.args = {
			{0, "struct tms *tbuf"},
		}
	};
	case 154: return {
		.nr = 154,
		.name = "setpgid",
		.args = {
			{0, "pid_t pid"},
			{1, "pid_t pgid"},
		}
	};
	case 155: return {
		.nr = 155,
		.name = "getpgid",
		.args = {
			{0, "pid_t pid"},
		}
	};
	case 156: return {
		.nr = 156,
		.name = "getsid",
		.args = {
			{0, "pid_t pid"},
		}
	};
	case 157: return {
		.nr = 157,
		.name = "setsid",
		.args = {
		}
	};
	case 158: return {
		.nr = 158,
		.name = "getgroups",
		.args = {
			{0, "int gidsetsize"},
			{1, "gid_t *grouplist"},
		}
	};
	case 159: return {
		.nr = 159,
		.name = "setgroups",
		.args = {
			{0, "int gidsetsize"},
			{1, "gid_t *grouplist"},
		}
	};
	case 160: return {
		.nr = 160,
		.name = "uname",
		.args = {
			{0, "struct old_utsname *"},
		}
	};
	case 161: return {
		.nr = 161,
		.name = "sethostname",
		.args = {
			{0, "char *name"},
			{1, "int len"},
		}
	};
	case 162: return {
		.nr = 162,
		.name = "setdomainname",
		.args = {
			{0, "char *name"},
			{1, "int len"},
		}
	};
	case 163: return {
		.nr = 163,
		.name = "getrlimit",
		.args = {
			{0, "unsigned int resource"},
			{1, "struct rlimit *rlim"},
		}
	};
	case 164: return {
		.nr = 164,
		.name = "setrlimit",
		.args = {
			{0, "unsigned int resource"},
			{1, "struct rlimit *rlim"},
		}
	};
	case 165: return {
		.nr = 165,
		.name = "getrusage",
		.args = {
			{0, "int who"},
			{1, "struct rusage *ru"},
		}
	};
	case 166: return {
		.nr = 166,
		.name = "umask",
		.args = {
			{0, "int mask"},
		}
	};
	case 167: return {
		.nr = 167,
		.name = "prctl",
		.args = {
			{0, "int option"},
			{1, "unsigned long arg2"},
			{2, "unsigned long arg3"},
			{3, "unsigned long arg4"},
			{4, "unsigned long arg5"},
		}
	};
	case 168: return {
		.nr = 168,
		.name = "getcpu",
		.args = {
			{0, "unsigned *cpu"},
			{1, "unsigned *node"},
			{2, "struct getcpu_cache *cache"},
		}
	};
	case 169: return {
		.nr = 169,
		.name = "gettimeofday",
		.args = {
			{0, "struct timeval *tv"},
			{1, "struct timezone *tz"},
		}
	};
	case 170: return {
		.nr = 170,
		.name = "settimeofday",
		.args = {
			{0, "struct timeval *tv"},
			{1, "struct timezone *tz"},
		}
	};
	case 171: return {
		.nr = 171,
		.name = "adjtimex",
		.args = {
			{0, "struct __kernel_timex *txc_p"},
		}
	};
	case 172: return {
		.nr = 172,
		.name = "getpid",
		.args = {
		}
	};
	case 173: return {
		.nr = 173,
		.name = "getppid",
		.args = {
		}
	};
	case 174: return {
		.nr = 174,
		.name = "getuid",
		.args = {
		}
	};
	case 175: return {
		.nr = 175,
		.name = "geteuid",
		.args = {
		}
	};
	case 176: return {
		.nr = 176,
		.name = "getgid",
		.args = {
		}
	};
	case 177: return {
		.nr = 177,
		.name = "getegid",
		.args = {
		}
	};
	case 178: return {
		.nr = 178,
		.name = "gettid",
		.args = {
		}
	};
	case 179: return {
		.nr = 179,
		.name = "sysinfo",
		.args = {
			{0, "struct sysinfo *info"},
		}
	};
	case 180: return {
		.nr = 180,
		.name = "mq_open",
		.args = {
			{0, "const char *name"},
			{1, "int oflag"},
			{2, "umode_t mode"},
			{3, "struct mq_attr *attr"},
		}
	};
	case 181: return {
		.nr = 181,
		.name = "mq_unlink",
		.args = {
			{0, "const char *name"},
		}
	};
	case 182: return {
		.nr = 182,
		.name = "mq_timedsend",
		.args = {
			{0, "mqd_t mqdes"},
			{1, "const char *msg_ptr"},
			{2, "size_t msg_len"},
			{3, "unsigned int msg_prio"},
			{4, "const struct __kernel_timespec *abs_timeout"},
		}
	};
	case 183: return {
		.nr = 183,
		.name = "mq_timedreceive",
		.args = {
			{0, "mqd_t mqdes"},
			{1, "char *msg_ptr"},
			{2, "size_t msg_len"},
			{3, "unsigned int *msg_prio"},
			{4, "const struct __kernel_timespec *abs_timeout"},
		}
	};
	case 184: return {
		.nr = 184,
		.name = "mq_notify",
		.args = {
			{0, "mqd_t mqdes"},
			{1, "const struct sigevent *notification"},
		}
	};
	case 185: return {
		.nr = 185,
		.name = "mq_getsetattr",
		.args = {
			{0, "mqd_t mqdes"},
			{1, "const struct mq_attr *mqstat"},
			{2, "struct mq_attr *omqstat"},
		}
	};
	case 186: return {
		.nr = 186,
		.name = "msgget",
		.args = {
			{0, "key_t key"},
			{1, "int msgflg"},
		}
	};
	case 187: return {
		.nr = 187,
		.name = "msgctl",
		.args = {
			{0, "int msqid"},
			{1, "int cmd"},
			{2, "struct msqid_ds *buf"},
		}
	};
	case 188: return {
		.nr = 188,
		.name = "msgrcv",
		.args = {
			{0, "int msqid"},
			{1, "struct msgbuf *msgp"},
			{2, "size_t msgsz"},
			{3, "long msgtyp"},
			{4, "int msgflg"},
		}
	};
	case 189: return {
		.nr = 189,
		.name = "msgsnd",
		.args = {
			{0, "int msqid"},
			{1, "struct msgbuf *msgp"},
			{2, "size_t msgsz"},
			{3, "int msgflg"},
		}
	};
	case 190: return {
		.nr = 190,
		.name = "semget",
		.args = {
			{0, "key_t key"},
			{1, "int nsems"},
			{2, "int semflg"},
		}
	};
	case 191: return {
		.nr = 191,
		.name = "semctl",
		.args = {
			{0, "int semid"},
			{1, "int semnum"},
			{2, "int cmd"},
			{3, "unsigned long arg"},
		}
	};
	case 192: return {
		.nr = 192,
		.name = "semtimedop",
		.args = {
			{0, "int semid"},
			{1, "struct sembuf *sops"},
			{2, "unsigned nsops"},
			{3, "const struct __kernel_timespec *timeout"},
		}
	};
	case 193: return {
		.nr = 193,
		.name = "semop",
		.args = {
			{0, "int semid"},
			{1, "struct sembuf *sops"},
			{2, "unsigned nsops"},
		}
	};
	case 194: return {
		.nr = 194,
		.name = "shmget",
		.args = {
			{0, "key_t key"},
			{1, "size_t size"},
			{2, "int flag"},
		}
	};
	case 195: return {
		.nr = 195,
		.name = "shmctl",
		.args = {
			{0, "int shmid"},
			{1, "int cmd"},
			{2, "struct shmid_ds *buf"},
		}
	};
	case 196: return {
		.nr = 196,
		.name = "shmat",
		.args = {
			{0, "int shmid"},
			{1, "char *shmaddr"},
			{2, "int shmflg"},
		}
	};
	case 197: return {
		.nr = 197,
		.name = "shmdt",
		.args = {
			{0, "char *shmaddr"},
		}
	};
	case 198: return {
		.nr = 198,
		.name = "socket",
		.args = {
			{0, "int"},
			{1, "int"},
			{2, "int"},
		}
	};
	case 199: return {
		.nr = 199,
		.name = "socketpair",
		.args = {
			{0, "int"},
			{1, "int"},
			{2, "int"},
			{3, "int *"},
		}
	};
	case 200: return {
		.nr = 200,
		.name = "bind",
		.args = {
			{0, "int"},
			{1, "struct sockaddr *"},
			{2, "int"},
		}
	};
	case 201: return {
		.nr = 201,
		.name = "listen",
		.args = {
			{0, "int"},
			{1, "int"},
		}
	};
	case 202: return {
		.nr = 202,
		.name = "accept",
		.args = {
			{0, "int"},
			{1, "struct sockaddr *"},
			{2, "int *"},
		}
	};
	case 203: return {
		.nr = 203,
		.name = "connect",
		.args = {
			{0, "int"},
			{1, "struct sockaddr *"},
			{2, "int"},
		}
	};
	case 204: return {
		.nr = 204,
		.name = "getsockname",
		.args = {
			{0, "int"},
			{1, "struct sockaddr *"},
			{2, "int *"},
		}
	};
	case 205: return {
		.nr = 205,
		.name = "getpeername",
		.args = {
			{0, "int"},
			{1, "struct sockaddr *"},
			{2, "int *"},
		}
	};
	case 206: return {
		.nr = 206,
		.name = "sendto",
		.args = {
			{0, "int"},
			{1, "void *"},
			{2, "size_t"},
			{3, "unsigned"},
			{4, "struct sockaddr *"},
			{5, "int"},
		}
	};
	case 207: return {
		.nr = 207,
		.name = "recvfrom",
		.args = {
			{0, "int"},
			{1, "void *"},
			{2, "size_t"},
			{3, "unsigned"},
			{4, "struct sockaddr *"},
			{5, "int *"},
		}
	};
	case 208: return {
		.nr = 208,
		.name = "setsockopt",
		.args = {
			{0, "int fd"},
			{1, "int level"},
			{2, "int optname"},
			{3, "char *optval"},
			{4, "int optlen"},
		}
	};
	case 209: return {
		.nr = 209,
		.name = "getsockopt",
		.args = {
			{0, "int fd"},
			{1, "int level"},
			{2, "int optname"},
			{3, "char *optval"},
			{4, "int *optlen"},
		}
	};
	case 210: return {
		.nr = 210,
		.name = "shutdown",
		.args = {
			{0, "int"},
			{1, "int"},
		}
	};
	case 211: return {
		.nr = 211,
		.name = "sendmsg",
		.args = {
			{0, "int fd"},
			{1, "struct user_msghdr *msg"},
			{2, "unsigned flags"},
		}
	};
	case 212: return {
		.nr = 212,
		.name = "recvmsg",
		.args = {
			{0, "int fd"},
			{1, "struct user_msghdr *msg"},
			{2, "unsigned flags"},
		}
	};
	case 213: return {
		.nr = 213,
		.name = "readahead",
		.args = {
			{0, "int fd"},
			{1, "loff_t offset"},
			{2, "size_t count"},
		}
	};
	case 214: return {
		.nr = 214,
		.name = "brk",
		.args = {
			{0, "unsigned long brk"},
		}
	};
	case 215: return {
		.nr = 215,
		.name = "munmap",
		.args = {
			{0, "unsigned long addr"},
			{1, "size_t len"},
		}
	};
	case 216: return {
		.nr = 216,
		.name = "mremap",
		.args = {
			{0, "unsigned long addr"},
			{1, "unsigned long old_len"},
			{2, "unsigned long new_len"},
			{3, "unsigned long flags"},
			{4, "unsigned long new_addr"},
		}
	};
	case 217: return {
		.nr = 217,
		.name = "add_key",
		.args = {
			{0, "const char *_type"},
			{1, "const char *_description"},
			{2, "const void *_payload"},
			{3, "size_t plen"},
			{4, "key_serial_t destringid"},
		}
	};
	case 218: return {
		.nr = 218,
		.name = "request_key",
		.args = {
			{0, "const char *_type"},
			{1, "const char *_description"},
			{2, "const char *_callout_info"},
			{3, "key_serial_t destringid"},
		}
	};
	case 219: return {
		.nr = 219,
		.name = "keyctl",
		.args = {
			{0, "int cmd"},
			{1, "unsigned long arg2"},
			{2, "unsigned long arg3"},
			{3, "unsigned long arg4"},
			{4, "unsigned long arg5"},
		}
	};
	case 220: return {
		.nr = 220,
		.name = "clone",
		.args = {
			{0, "unsigned long"},
			{1, "unsigned long"},
			{2, "int *"},
			{3, "int *"},
			{4, "unsigned long"},
		}
	};
	case 221: return {
		.nr = 221,
		.name = "execve",
		.args = {
			{0, "const char *filename"},
			{1, "const char *const *argv"},
			{2, "const char *const *envp"},
		}
	};
	case 222: return {
		.nr = 222,
		.name = "mmap",
		.args = {
			{0, "?"},
			{1, "?"},
			{2, "?"},
			{3, "?"},
			{4, "?"},
			{5, "?"},
		}
	};
	case 223: return {
		.nr = 223,
		.name = "fadvise64",
		.args = {
			{0, "int fd"},
			{1, "loff_t offset"},
			{2, "size_t len"},
			{3, "int advice"},
		}
	};
	case 224: return {
		.nr = 224,
		.name = "swapon",
		.args = {
			{0, "const char *specialfile"},
			{1, "int swap_flags"},
		}
	};
	case 225: return {
		.nr = 225,
		.name = "swapoff",
		.args = {
			{0, "const char *specialfile"},
		}
	};
	case 226: return {
		.nr = 226,
		.name = "mprotect",
		.args = {
			{0, "unsigned long start"},
			{1, "size_t len"},
			{2, "unsigned long prot"},
		}
	};
	case 227: return {
		.nr = 227,
		.name = "msync",
		.args = {
			{0, "unsigned long start"},
			{1, "size_t len"},
			{2, "int flags"},
		}
	};
	case 228: return {
		.nr = 228,
		.name = "mlock",
		.args = {
			{0, "unsigned long start"},
			{1, "size_t len"},
		}
	};
	case 229: return {
		.nr = 229,
		.name = "munlock",
		.args = {
			{0, "unsigned long start"},
			{1, "size_t len"},
		}
	};
	case 230: return {
		.nr = 230,
		.name = "mlockall",
		.args = {
			{0, "int flags"},
		}
	};
	case 231: return {
		.nr = 231,
		.name = "munlockall",
		.args = {
		}
	};
	case 232: return {
		.nr = 232,
		.name = "mincore",
		.args = {
			{0, "unsigned long start"},
			{1, "size_t len"},
			{2, "unsigned char * vec"},
		}
	};
	case 233: return {
		.nr = 233,
		.name = "madvise",
		.args = {
			{0, "unsigned long start"},
			{1, "size_t len"},
			{2, "int behavior"},
		}
	};
	case 234: return {
		.nr = 234,
		.name = "remap_file_pages",
		.args = {
			{0, "unsigned long start"},
			{1, "unsigned long size"},
			{2, "unsigned long prot"},
			{3, "unsigned long pgoff"},
			{4, "unsigned long flags"},
		}
	};
	case 235: return {
		.nr = 235,
		.name = "mbind",
		.args = {
			{0, "unsigned long start"},
			{1, "unsigned long len"},
			{2, "unsigned long mode"},
			{3, "const unsigned long *nmask"},
			{4, "unsigned long maxnode"},
			{5, "unsigned flags"},
		}
	};
	case 236: return {
		.nr = 236,
		.name = "get_mempolicy",
		.args = {
			{0, "int *policy"},
			{1, "unsigned long *nmask"},
			{2, "unsigned long maxnode"},
			{3, "unsigned long addr"},
			{4, "unsigned long flags"},
		}
	};
	case 237: return {
		.nr = 237,
		.name = "set_mempolicy",
		.args = {
			{0, "int mode"},
			{1, "const unsigned long *nmask"},
			{2, "unsigned long maxnode"},
		}
	};
	case 238: return {
		.nr = 238,
		.name = "migrate_pages",
		.args = {
			{0, "pid_t pid"},
			{1, "unsigned long maxnode"},
			{2, "const unsigned long *from"},
			{3, "const unsigned long *to"},
		}
	};
	case 239: return {
		.nr = 239,
		.name = "move_pages",
		.args = {
			{0, "pid_t pid"},
			{1, "unsigned long nr_pages"},
			{2, "const void * *pages"},
			{3, "const int *nodes"},
			{4, "int *status"},
			{5, "int flags"},
		}
	};
	case 240: return {
		.nr = 240,
		.name = "rt_tgsigqueueinfo",
		.args = {
			{0, "pid_t tgid"},
			{1, "pid_t pid"},
			{2, "int sig"},
			{3, "siginfo_t *uinfo"},
		}
	};
	case 241: return {
		.nr = 241,
		.name = "perf_event_open",
		.args = {
			{0, "struct perf_event_attr *attr_uptr"},
			{1, "pid_t pid"},
			{2, "int cpu"},
			{3, "int group_fd"},
			{4, "unsigned long flags"},
		}
	};
	case 242: return {
		.nr = 242,
		.name = "accept4",
		.args = {
			{0, "int"},
			{1, "struct sockaddr *"},
			{2, "int *"},
			{3, "int"},
		}
	};
	case 243: return {
		.nr = 243,
		.name = "recvmmsg",
		.args = {
			{0, "int fd"},
			{1, "struct mmsghdr *msg"},
			{2, "unsigned int vlen"},
			{3, "unsigned flags"},
			{4, "struct __kernel_timespec *timeout"},
		}
	};
	case 244: return {
		.nr = 244,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 245: return {
		.nr = 245,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 246: return {
		.nr = 246,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 247: return {
		.nr = 247,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 248: return {
		.nr = 248,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 249: return {
		.nr = 249,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 250: return {
		.nr = 250,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 251: return {
		.nr = 251,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 252: return {
		.nr = 252,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 253: return {
		.nr = 253,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 254: return {
		.nr = 254,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 255: return {
		.nr = 255,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 256: return {
		.nr = 256,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 257: return {
		.nr = 257,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 258: return {
		.nr = 258,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 259: return {
		.nr = 259,
		.name = "not implemented",
		.args = {
			{0, ""},
			{1, ""},
			{2, ""},
			{3, ""},
			{4, ""},
			{5, ""},
		}
	};
	case 260: return {
		.nr = 260,
		.name = "wait4",
		.args = {
			{0, "pid_t pid"},
			{1, "int *stat_addr"},
			{2, "int options"},
			{3, "struct rusage *ru"},
		}
	};
	case 261: return {
		.nr = 261,
		.name = "prlimit64",
		.args = {
			{0, "pid_t pid"},
			{1, "unsigned int resource"},
			{2, "const struct rlimit64 *new_rlim"},
			{3, "struct rlimit64 *old_rlim"},
		}
	};
	case 262: return {
		.nr = 262,
		.name = "fanotify_init",
		.args = {
			{0, "unsigned int flags"},
			{1, "unsigned int event_f_flags"},
		}
	};
	case 263: return {
		.nr = 263,
		.name = "fanotify_mark",
		.args = {
			{0, "int fanotify_fd"},
			{1, "unsigned int flags"},
			{2, "u64 mask"},
			{3, "int fd"},
			{4, "const char *pathname"},
		}
	};
	case 264: return {
		.nr = 264,
		.name = "name_to_handle_at",
		.args = {
			{0, "int dfd"},
			{1, "const char *name"},
			{2, "struct file_handle *handle"},
			{3, "int *mnt_id"},
			{4, "int flag"},
		}
	};
	case 265: return {
		.nr = 265,
		.name = "open_by_handle_at",
		.args = {
			{0, "int mountdirfd"},
			{1, "struct file_handle *handle"},
			{2, "int flags"},
		}
	};
	case 266: return {
		.nr = 266,
		.name = "clock_adjtime",
		.args = {
			{0, "clockid_t which_clock"},
			{1, "struct __kernel_timex *tx"},
		}
	};
	case 267: return {
		.nr = 267,
		.name = "syncfs",
		.args = {
			{0, "int fd"},
		}
	};
	case 268: return {
		.nr = 268,
		.name = "setns",
		.args = {
			{0, "int fd"},
			{1, "int nstype"},
		}
	};
	case 269: return {
		.nr = 269,
		.name = "sendmmsg",
		.args = {
			{0, "int fd"},
			{1, "struct mmsghdr *msg"},
			{2, "unsigned int vlen"},
			{3, "unsigned flags"},
		}
	};
	case 270: return {
		.nr = 270,
		.name = "process_vm_readv",
		.args = {
			{0, "pid_t pid"},
			{1, "const struct iovec *lvec"},
			{2, "unsigned long liovcnt"},
			{3, "const struct iovec *rvec"},
			{4, "unsigned long riovcnt"},
			{5, "unsigned long flags"},
		}
	};
	case 271: return {
		.nr = 271,
		.name = "process_vm_writev",
		.args = {
			{0, "pid_t pid"},
			{1, "const struct iovec *lvec"},
			{2, "unsigned long liovcnt"},
			{3, "const struct iovec *rvec"},
			{4, "unsigned long riovcnt"},
			{5, "unsigned long flags"},
		}
	};
	case 272: return {
		.nr = 272,
		.name = "kcmp",
		.args = {
			{0, "pid_t pid1"},
			{1, "pid_t pid2"},
			{2, "int type"},
			{3, "unsigned long idx1"},
			{4, "unsigned long idx2"},
		}
	};
	case 273: return {
		.nr = 273,
		.name = "finit_module",
		.args = {
			{0, "int fd"},
			{1, "const char *uargs"},
			{2, "int flags"},
		}
	};
	case 274: return {
		.nr = 274,
		.name = "sched_setattr",
		.args = {
			{0, "pid_t pid"},
			{1, "struct sched_attr *attr"},
			{2, "unsigned int flags"},
		}
	};
	case 275: return {
		.nr = 275,
		.name = "sched_getattr",
		.args = {
			{0, "pid_t pid"},
			{1, "struct sched_attr *attr"},
			{2, "unsigned int size"},
			{3, "unsigned int flags"},
		}
	};
	case 276: return {
		.nr = 276,
		.name = "renameat2",
		.args = {
			{0, "int olddfd"},
			{1, "const char *oldname"},
			{2, "int newdfd"},
			{3, "const char *newname"},
			{4, "unsigned int flags"},
		}
	};
	case 277: return {
		.nr = 277,
		.name = "seccomp",
		.args = {
			{0, "unsigned int op"},
			{1, "unsigned int flags"},
			{2, "void *uargs"},
		}
	};
	case 278: return {
		.nr = 278,
		.name = "getrandom",
		.args = {
			{0, "char *buf"},
			{1, "size_t count"},
			{2, "unsigned int flags"},
		}
	};
	case 279: return {
		.nr = 279,
		.name = "memfd_create",
		.args = {
			{0, "const char *uname_ptr"},
			{1, "unsigned int flags"},
		}
	};
	case 280: return {
		.nr = 280,
		.name = "bpf",
		.args = {
			{0, "int cmd"},
			{1, "union bpf_attr *attr"},
			{2, "unsigned int size"},
		}
	};
	case 281: return {
		.nr = 281,
		.name = "execveat",
		.args = {
			{0, "int dfd"},
			{1, "const char *filename"},
			{2, "const char *const *argv"},
			{3, "const char *const *envp"},
			{4, "int flags"},
		}
	};
	case 282: return {
		.nr = 282,
		.name = "userfaultfd",
		.args = {
			{0, "int flags"},
		}
	};
	case 283: return {
		.nr = 283,
		.name = "membarrier",
		.args = {
			{0, "int cmd"},
			{1, "int flags"},
		}
	};
	case 284: return {
		.nr = 284,
		.name = "mlock2",
		.args = {
			{0, "unsigned long start"},
			{1, "size_t len"},
			{2, "int flags"},
		}
	};
	case 285: return {
		.nr = 285,
		.name = "copy_file_range",
		.args = {
			{0, "int fd_in"},
			{1, "loff_t *off_in"},
			{2, "int fd_out"},
			{3, "loff_t *off_out"},
			{4, "size_t len"},
			{5, "unsigned int flags"},
		}
	};
	case 286: return {
		.nr = 286,
		.name = "preadv2",
		.args = {
			{0, "unsigned long fd"},
			{1, "const struct iovec *vec"},
			{2, "unsigned long vlen"},
			{3, "unsigned long pos_l"},
			{4, "unsigned long pos_h"},
			{5, "rwf_t flags"},
		}
	};
	case 287: return {
		.nr = 287,
		.name = "pwritev2",
		.args = {
			{0, "unsigned long fd"},
			{1, "const struct iovec *vec"},
			{2, "unsigned long vlen"},
			{3, "unsigned long pos_l"},
			{4, "unsigned long pos_h"},
			{5, "rwf_t flags"},
		}
	};
	case 288: return {
		.nr = 288,
		.name = "pkey_mprotect",
		.args = {
			{0, "unsigned long start"},
			{1, "size_t len"},
			{2, "unsigned long prot"},
			{3, "int pkey"},
		}
	};
	case 289: return {
		.nr = 289,
		.name = "pkey_alloc",
		.args = {
			{0, "unsigned long flags"},
			{1, "unsigned long init_val"},
		}
	};
	case 290: return {
		.nr = 290,
		.name = "pkey_free",
		.args = {
			{0, "int pkey"},
		}
	};
	case 291: return {
		.nr = 291,
		.name = "statx",
		.args = {
			{0, "int dfd"},
			{1, "const char *path"},
			{2, "unsigned flags"},
			{3, "unsigned mask"},
			{4, "struct statx *buffer"},
		}
	};
    case 293: return {
        .nr = 293,
        .name = "rseq",
        .args = {
            {0, "struct rseq *rseq"},
            {1, "uint32_t rseq_len"},
            {2, "int flags"},
            {3, "uint32_t sig"}
        }
    };
    default: return {
        .nr = 0,
        .name = "UNKNOWN",
        .args = {
            {0, "unknown0"},
            {1, "unknown1"},
            {2, "unknown2"},
            {3, "unknown3"},
            {4, "unknown4"},
            {5, "unknown5"}
        }
    };
    }
}