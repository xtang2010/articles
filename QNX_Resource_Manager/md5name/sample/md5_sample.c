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
#include <fcntl.h>
#include <unistd.h>
#include <sys/dispatch.h>
#include "md5name.h"

int main(int argc, char **argv)
{
	int fd, cfd;
	unsigned char buf[16 * 1024];
	unsigned char digest[16];
	int n, total;

	if ((fd = name_open("md5name", 0)) == -1) {
		perror("name_open");
		return -1;
	}

	if ((cfd = open(argv[1], O_RDONLY)) == -1) {
		perror("open");
		return -1;
	}

	total = 0;
	for (;;) {
		n = read(cfd, buf, 16 * 1024);
		if (n <= 0)
			break;

		if (md5_send(fd, buf, n) <= 0) {
			perror("md5_send");
			return -1;
		}

		total += n;
	}

	md5_recv(fd, digest, 16);
	printf("%10d\t\t\t", total);
	for (n = 0; n < 16; n++) {
		printf(" %02X", digest[n]);
	}
	printf("\n");
	return 0;
}
