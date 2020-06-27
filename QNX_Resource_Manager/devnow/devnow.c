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
#include <time.h>

static int now_read(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
	iofunc_ocb_t *o = (iofunc_ocb_t *)ocb;
	char nowstr[128];
	time_t t;
	int n;

//	if (o->offset != 0) {
//		return 0;
//	}

	t = time(NULL);
	n = strftime(nowstr, 128, "%Y-%m-%d %H:%M:%S\n", localtime(&t));
	o->offset += n;
	MsgReply(ctp->rcvid, n, nowstr, n);

	return _RESMGR_NOREPLY;
}

int main(void) {
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
        printf("dev-now: Unable to create dispatch context: %s.", strerror(errno));
        return -1;
    }

    memset( &res_attr, 0, sizeof( res_attr ) );
    res_attr.nparts_max = 10;
    res_attr.msg_max_size = 0;

    iofunc_attr_init(&io_attr, 0666 | S_IFCHR, 0, 0);
    iofunc_func_init( _RESMGR_CONNECT_NFUNCS, &connect_funcs,
                      _RESMGR_IO_NFUNCS, &io_funcs );

    io_funcs.read = now_read;


    rmgid = resmgr_attach(dispatch, &res_attr, "/dev/now", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr);
    if (rmgid == -1)
    {
    	printf("dev-now: Unable to attach resmgr: %s", strerror(errno));
    	return -1;
    }

    ctp = resmgr_context_alloc(dispatch);
    if (ctp == NULL)
    {
    	printf("dev-now: Unable to allocate resmgr context: %s.", strerror(errno));
    	return -1;
    }

    while (1)
    {
    		ctp = resmgr_block(ctp);
    		if (ctp == NULL)
    		{
    			printf("dev-now: resmgr_block() failed: %s.", strerror(errno));
    			break;
    		}

    		resmgr_handler(ctp);
    }

	return EXIT_SUCCESS;
}
