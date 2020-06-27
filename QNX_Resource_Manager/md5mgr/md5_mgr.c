//============================================================================
// Copyright (C) 2020 Xiaodan Tang
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/sysmgr.h>
#include <sys/dispatch.h>
#include <sys/iofunc.h>
#include <openssl/md5.h>

/* extend iofunc_ocb_t */
typedef struct {
	iofunc_ocb_t ocb;
	int			 total_len;
	MD5_CTX      md5_ctx;
} md5mgr_ocb_t;

static IOFUNC_OCB_T *md5mgr_ocb_calloc(resmgr_context_t *ctp, iofunc_attr_t *attr)
{
	md5mgr_ocb_t *o;

	o = calloc(1, sizeof(md5mgr_ocb_t));
	if (o != 0) {
		MD5_Init(&o->md5_ctx);
	}
	return (IOFUNC_OCB_T *)o;
}

static void md5mgr_ocb_free(IOFUNC_OCB_T *o)
{
	free(o);
}

static int md5mgr_write(resmgr_context_t *ctp, io_write_t *msg, iofunc_ocb_t *ocb)
{
	md5mgr_ocb_t *o = (md5mgr_ocb_t *)ocb;
	char buffer[16 * 1024];
	int n, off, rest, len;

	off = sizeof(*msg);
	rest = msg->i.nbytes;

	while (rest > 0) {
		if (rest > 16 * 1024)
			len = 16 * 1024;
		else
			len = rest;
		n = MsgRead(ctp->rcvid, buffer, len, off);
		if (n == -1) {
			return errno;
		}
		rest -= n;
		off += n;
		MD5_Update(&o->md5_ctx, buffer, n);
	}

	o->total_len += msg->i.nbytes;
	_IO_SET_WRITE_NBYTES(ctp, msg->i.nbytes);
	return EOK;
}

static int md5mgr_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb)
{
	md5mgr_ocb_t *o = (md5mgr_ocb_t *)ocb;
	unsigned char digest[16];

	if (o->total_len == 0) {
		_IO_SET_READ_NBYTES(ctp, 0);
		return EOK;
	}

	MD5_Final(digest, &o->md5_ctx);
	MsgReply(ctp->rcvid, 16, digest, 16);

	o->total_len = 0;
	MD5_Init(&o->md5_ctx);

	return _RESMGR_NOREPLY;
}


int md5_mgr(char *name)
{
    resmgr_context_t         *ctp;
    resmgr_connect_funcs_t   connect_funcs;
    resmgr_io_funcs_t        io_funcs;
    iofunc_attr_t          	 io_attr;
    resmgr_attr_t            res_attr;
    dispatch_t               *dispatch;
    int                      rmgid;

    dispatch = dispatch_create();
    if( dispatch == NULL )
    {
        printf("md5_service: Unable to create dispatch context: %s.", strerror(errno));
        return -1;
    }

    memset( &res_attr, 0, sizeof( res_attr ) );
    res_attr.nparts_max = 10;
    res_attr.msg_max_size = 0;

    iofunc_funcs_t ocb_funcs = {
        _IOFUNC_NFUNCS,
        md5mgr_ocb_calloc,
		md5mgr_ocb_free
    };

    // io_attr
    iofunc_mount_t 	mountpoint = { 0, 0, 0, 0, &ocb_funcs };
    iofunc_attr_init(&io_attr, 0666 | S_IFCHR, 0, 0);
    io_attr.mount = &mountpoint;

    iofunc_func_init( _RESMGR_CONNECT_NFUNCS, &connect_funcs,
                      _RESMGR_IO_NFUNCS, &io_funcs );
    io_funcs.read   = md5mgr_read;
    io_funcs.write  = md5mgr_write;

    rmgid = resmgr_attach(dispatch, &res_attr, name, _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr);
    if (rmgid == -1)
    {
    	printf("md5_service: Unable to attach resmgr: %s", strerror(errno));
    	return -1;
    }

    ctp = resmgr_context_alloc(dispatch);
    if (ctp == NULL)
    {
    	printf("md5_service: Unable to allocate resmgr context: %s.", strerror(errno));
    	return -1;
    }

    while (1)
    {
    		ctp = resmgr_block(ctp);
    		if (ctp == NULL)
    		{
    			printf("md5_service: resmgr_block() failed: %s.", strerror(errno));
    		}

    		resmgr_handler(ctp);
    }

    return 0;
}

int main(int argc, char **argv)
{
   	char *name = "/dev/md5";
	int c;

    while( ( c = getopt( argc, argv, "n" ) ) != -1 )
    {
        switch( c )
        {
            case 'n':
            	name = optarg;
                break;
        }
    }

	return md5_mgr(name);
}
