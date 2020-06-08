//============================================================================
// Copyright (C) 2009 Xiaodan Tang
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

// Simple server
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>

int main()
{
        int chid, rcvid, status;
        char buf[128];

        if ((chid = ChannelCreate(0)) == -1) {
                perror("ChannelCreate");
                return -1;
        }

        printf("Server is ready, pid = %d, chid = %d\n", getpid(), chid);

        for (;;) {
                if ((rcvid = MsgReceive(chid, buf, sizeof(buf), NULL)) == -1) {
                        perror("MsgReceive");
                        return -1;
                }

                printf("Server: Received '%s'\n", buf);

                /* Based on what we receive, return some message */
                if (strcmp(buf, "Hello") == 0) {
                        MsgReply(rcvid, 0, "World", strlen("World") + 1);
                } else if (strcmp(buf, "Ni Hao") == 0) {
                        MsgReply(rcvid, 0, "Zhong Guo", strlen("Zhong Guo") + 1);
                } else {
                        MsgError(rcvid, EINVAL);
                }
        }

        ChannelDestroy(chid);
        return 0;
}
