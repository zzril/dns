#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>

#include <netinet/in.h>

//#define IPV4_ONLY

// --------

#define INET_ANY_BUFFER_SIZE (INET_ADDRSTRLEN < INET6_ADDRSTRLEN ? INET6_ADDRSTRLEN + 1 : INET_ADDRSTRLEN + 1)

// --------

static int print_addrinfo(struct addrinfo* addr_info);

// --------

static const char unknown[] = "unknown";

// --------

int main(int argc, const char** argv) {

	if(argc < 2 || argc > 3) {
		printf("Usage: %s hostname|IP [service|port] \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	const char* node = argv[1];
	const char* service = argv[2];

	struct addrinfo* results;

	int error_code;

	struct addrinfo* hint_ptr;

	#ifdef IPV4_ONLY
		struct addrinfo hints;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;
		hint_ptr = &hints;
	#else
		hint_ptr = NULL;
	#endif

	if((error_code = getaddrinfo(node, service, hint_ptr, &results))) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error_code));
		exit(EXIT_FAILURE);
	}

	if((error_code = print_addrinfo(results))) {
		fprintf(stderr, "getnameinfo: %s\n", gai_strerror(error_code));
	}

	freeaddrinfo(results);

	exit(EXIT_SUCCESS);
}

// --------

static int print_addrinfo(struct addrinfo* addr_info) {

	for(struct addrinfo* ap = addr_info; ap != NULL; ap = ap->ai_next)  {

		char host[NI_MAXHOST];
		char service[NI_MAXSERV];

		char address[INET_ANY_BUFFER_SIZE];

		int error_code;

		if((error_code = getnameinfo(addr_info->ai_addr, addr_info->ai_addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0))) {
			return error_code;
		}

		void* addr_src;
		bool addr_family_supported = true;

		switch(ap->ai_family) {
			case AF_INET:
				addr_src = &(((struct sockaddr_in*) (ap->ai_addr))->sin_addr);
				break;
			case AF_INET6:
				addr_src = &(((struct sockaddr_in6*) (ap->ai_addr))->sin6_addr);
				break;
			default:
				addr_family_supported = false;
				break;
		}

		if(!addr_family_supported || !inet_ntop(ap->ai_family, addr_src, address, INET_ANY_BUFFER_SIZE)) {
			strncpy(address, unknown, INET_ANY_BUFFER_SIZE-1);
		}

		printf("Host:\t\t%s\n", host);
		printf("Service:\t%s\n", service);

		printf("Family:\t\t%s\n", ap->ai_family == AF_INET ? "AF_INET" : (ap->ai_family == AF_INET6 ? "AF_INET6" : unknown));
		printf("Socktype:\t%s\n", ap->ai_socktype == SOCK_STREAM ? "SOCK_STREAM" : (ap->ai_socktype == SOCK_DGRAM ? "SOCK_DGRAM" : unknown));

		printf("Address:\t%s\n", address);

		printf("\n");
	}

	return 0;
}


