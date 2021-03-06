/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>

/* socket extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

/* plan 9 */
#include "lib.h"
#include "sys9.h"

#include "priv.h"

int
bind(int fd, void *a, int alen)
{
	int n, len, cfd;
	struct sockaddr *sa;
	Rock *r;
	char msg[128];

	/* assign the address */
	r = _sock_findrock(fd, 0);
	if(r == 0){
		errno = ENOTSOCK;
		return -1;
	}
	sa = (struct sockaddr*)a;
	if(sa->sa_family != r->domain){
		errno = EAFNOSUPPORT;
		return -1;
	}
	if(alen > sizeof(r->addr)){
		errno = ENAMETOOLONG;
		return -1;
	}
	memmove(&r->addr, a, alen);

	/* the rest is IP sepecific */
	if (r->domain != PF_INET && r->domain != PF_INET6)
		return 0;

	cfd = open(r->ctl, O_RDWR);
	if(cfd < 0){
		errno = EBADF;
		return -1;
	}

	strcpy(msg, "bind ");
	_sock_inaddr2string(r, msg + 5, sizeof msg - 5);

	n = write(cfd, msg, strlen(msg));
	if(n < 0){
		_syserrno();
		if(errno == EPLAN9)
			errno = EOPNOTSUPP;
		close(cfd);
		return -1;
	}

	close(cfd);

	if(_sock_inport(&r->addr) == 0)
		_sock_ingetaddr(r, &r->addr, 0, "local");

	return 0;
}


