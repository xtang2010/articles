#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/sysmgr.h>
#include <sys/dispatch.h>
#include <sys/iofunc.h>

int main(int argc, char *argv[]) {
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
        printf("posix_file: Unable to create dispatch context: %s.", strerror(errno));
        return -1;
    }

    memset( &res_attr, 0, sizeof( res_attr ) );
    res_attr.nparts_max = 10;
    res_attr.msg_max_size = 0;

    iofunc_attr_init(&io_attr, 0666 | S_IFCHR, 0, 0);
    iofunc_func_init( _RESMGR_CONNECT_NFUNCS, &connect_funcs,
                      _RESMGR_IO_NFUNCS, &io_funcs );

    connect_funcs.open  = iofunc_open_default;
    io_funcs.read       = iofunc_read_default;
    io_funcs.write      = iofunc_write_default;
    io_funcs.chmod      = iofunc_chmod_default;
    io_funcs.chown      = iofunc_chown_default;
    io_funcs.stat       = iofunc_stat_default;
    io_funcs.close_ocb  = iofunc_close_ocb_default;

    rmgid = resmgr_attach(dispatch, &res_attr, "/posix_file", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr);
    if (rmgid == -1)
    {
    	printf("posix_file: Unable to attach resmgr: %s", strerror(errno));
    	return -1;
    }

    ctp = resmgr_context_alloc(dispatch);
    if (ctp == NULL)
    {
    	printf("posix_file: Unable to allocate resmgr context: %s.", strerror(errno));
    	return -1;
    }

    while (1)
    {
    		ctp = resmgr_block(ctp);
    		if (ctp == NULL)
    		{
    			printf("posix_file: resmgr_block() failed: %s.", strerror(errno));
    			break;
    		}

    		resmgr_handler(ctp);
    }
}
