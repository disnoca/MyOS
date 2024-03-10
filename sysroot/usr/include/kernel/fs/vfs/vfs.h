/**
 * Header definitions for the VFS as specified by "Vnodes: An Architecture for Multiple File System Types in Sun UNIX" by S.R. Kleiman
*/

#pragma once


typedef struct vfs_s {
	struct vfs_s *vfs_next; 		/* next vfs in list */
	struct vfsops_s *vfs_op; 		/* operations on vfs */
	struct vnode *vfs_vnodecovered; /* vnode we cover */
	int vfs_flag; 					/* flags */
	int vfs_bsize; 					/* native block size */
	void* vfs_data; 				/* private data */
} vfs_t;

typedef struct vfsops_s {
	int (*vfs_mount)();
	int (*vfs_unmount)();
	int (*vfs_root)();
	int (*vfs_statfs)();
	int (*vfs_sync)();
	int (*vfs_fid)();
	int (*vfs_vget)();
} vfsops_t;

typedef enum vtype_e { VNON, VREG, VDIR, VBLK, VCHR, VLNK, VSOCK, VBAD } vtype_t;

typedef struct vnode_s {
	unsigned short v_flag; 			/* vnode flags */
	unsigned short v_count; 		/* reference count */
//	unsigned short v_shlockc; 		/* # of shared locks */
//	unsigned short v_exlockc; 		/* # of exclusive locks */
	struct vfs_s *v_vfsmountedhere; /* covering vfs */
//	struct vnodeops *v_op; 			/* vnode operations */
//	union {
//		struct socket *v_Socket; 	/* unix ipc */
//		struct stdata *v_Stream; 	/* stream */
//	};
	struct vfs_s *v_vfsp; 			/* vfs we are in */
	enum vtype_e v_type; 			/* vnode type */
	void* v_data; 					/* private data */
} vnode_t;

typedef struct vnodeops_s {
	int (*vn_open)();
	int (*vn_close)();
	int (*vn_rdwr)();
	int (*vn_ioctl)();
	int (*vn_select)();
	int (*vn_getattr)();
	int (*vn_setattr)();
	int (*vn_access)();
	int (*vn_lookup)();
	int (*vn_create)();
	int (*vn_remove)();
	int (*vn_link)();
	int (*vn_rename)();
	int (*vn_mkdir)();
	int (*vn_rmdir)();
	int (*vn_readdir)();
	int (*vn_symlink)();
	int (*vn_readlink)();
	int (*vn_fsync)();
	int (*vn_inactive)();
	int (*vn_bmap)();
	int (*vn_strategy)();
	int (*vn_bread)();
	int (*vn_brelse)();
} vnodeops_t;
