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

//simple client
#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>

int main(int argc, char **argv)
{
        pid_t spid;
        int chid, coid, i;
        char buf[128];

        if (argc < 3) {
                fprintf(stderr, "Usage: simple_client <pid> <chid>\n");
                return -1;
        }

        spid = atoi(argv[1]);
        chid = atoi(argv[2]);

        if ((coid = ConnectAttach(0, spid, chid, 0, 0)) == -1) {
                perror("ConnectAttach");
                return -1;
        }

        /* sent 3 pairs of "Hello" and "Ni Hao" */
        for (i = 0; i < 3; i++) {
                sprintf(buf, "Hello");
                printf("client: sent '%s'\n", buf);
                if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != 0) {
                        perror("MsgSend");
                        return -1;
                }
                printf("client: returned '%s'\n", buf);

                sprintf(buf, "Ni Hao");
                printf("client: sent '%s'\n", buf);
                if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != 0) {
                        perror("MsgSend");
                        return -1;
                }
                printf("client: returned '%s'\n", buf);
        }

        /* sent a bad message, see if we get an error */
        sprintf(buf, "Unknown");
        printf("client: sent '%s'\n", buf);
        if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != 0) {
                perror("MsgSend");
                return -1;
        }

        ConnectDetach(coid);

        return 0;
}
