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
#include <unistd.h>
#include <sys/dispatch.h>
#include <openssl/md5.h>
#include "../md5name_priv.h"

int md5_name(char *name)
{
	name_attach_t *natt;
	union {
		struct _pulse pulse;
		md5_msg_t hdr;
	} msg;
	MD5_CTX ctx;
	unsigned char buffer[16 * 1024];
	unsigned char digest[16];
	int rcvid, n, off, rest;

	natt = name_attach(NULL, name, 0);
	if (!natt) {
		perror("name_attach");
		return -1;
	}

	MD5_Init(&ctx);

	for (;;) {
		rcvid = MsgReceive(natt->chid, &msg, sizeof(msg), 0);
		if (rcvid == -1) {
			perror("MsgReceive");
			return -1;
		}

		switch (msg.hdr.msgtype) {
		case MD5_SEND_DATA:
			rest = msg.hdr.msglen;
			off = sizeof(msg.hdr);
			while (rest > 0) {
				n = MsgRead(rcvid, &buffer, 16 * 1024, off);
				if (n == -1) {
					perror("MsgRead");
					return -1;
				}

				if (n == 0)
					break;

				MD5_Update(&ctx, buffer, n);
				rest -= n;
				off += n;
			}
			MsgReply(rcvid, msg.hdr.msglen - rest, 0, 0);
			break;

		case MD5_RECV_DIGEST:
			MD5_Final(digest, &ctx);
			MsgReply(rcvid, sizeof(digest), digest, sizeof(digest));
			MD5_Init(&ctx);
			break;
		}
	}
}

int main(int argc, char *argv[]) {
   	char *name = "md5name";
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

	return md5_name(name);
}
