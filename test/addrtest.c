/*************************************************************************
 *                                                                       *
 *                           Copyright (C)  2015                         *
 *                          Brocade Communications                       *
 *                           All Rights Reserved                         *
 *                                                                       *
 *************************************************************************
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#include <I2util/addr.h>

static int failed_tests = 0;

#define TEST_PORT "8862"

#define ok(test, message, ...) ok_(test, __FILE__, __LINE__, message, ##__VA_ARGS__)

static void ok_(
    int test, const char *file, int line, const char *message, ...) __attribute__ ((format (printf, 4, 5)));

static void ok_(
    int test, const char *file, int line, const char *message, ...)
{
    va_list va;

    printf("%s %s:%d - ", test ? "SUCCESS" : "FAIL", file, line);
    va_start(va, message);
    vprintf(message, va);
    va_end(va);
    if (!test) {
        failed_tests++;
    }
    putchar('\n');
}

static void client_test (I2ErrHandle eh)
{
    I2Addr addr;
    struct addrinfo *fai;
    struct addrinfo *ai;
    int fd = -1;

    addr = I2AddrByNode(eh, "127.0.0.1:" TEST_PORT);
    ok(addr != NULL, "I2AddrByNode");

    fai = I2AddrAddrInfo(addr, NULL, TEST_PORT);
    ok(fai != NULL, "I2AddrAddrInfo");

    for(ai = fai;ai;ai = ai->ai_next){
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if(fd < 0)
            continue;

        if(connect(fd, ai->ai_addr, ai->ai_addrlen) == 0){
            ok(I2AddrSetSAddr(addr,ai->ai_addr,ai->ai_addrlen), "I2AddrSetSAddr");
            ok(I2AddrSetProtocol(addr,ai->ai_protocol), "I2AddrSetProtocol");
            ok(I2AddrSetSocktype(addr,ai->ai_socktype), "I2AddrSetSocktype");
            ok(I2AddrSetFD(addr,fd,True), "I2AddrSetFD");

            break;
        }
    }
    if (fd < 0) {
        ok(False, "connected");
        exit(1);
    }

    I2AddrFree(addr);
}

static I2Addr server_init (I2ErrHandle eh)
{
    struct addrinfo *fai;
    struct addrinfo *ai;
    int fd = -1;
    I2Addr addr;
    I2Boolean res;

    addr = I2AddrByWildcard(eh, SOCK_STREAM, TEST_PORT);
    ok(addr != NULL, "I2AddrByWildcard");

    res = I2AddrSetPassive(addr, True);
    ok(res, "I2AddrSetPassive");

    fai = I2AddrAddrInfo(addr, NULL, TEST_PORT);
    ok(fai != NULL, "I2AddrAddrInfo");

    for(ai = fai;ai;ai = ai->ai_next){
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if(fd < 0)
            continue;

        if(bind(fd, ai->ai_addr, ai->ai_addrlen) == 0){
            ok(I2AddrSetSAddr(addr,ai->ai_addr,ai->ai_addrlen), "I2AddrSetSAddr");
            ok(I2AddrSetProtocol(addr,ai->ai_protocol), "I2AddrSetProtocol");
            ok(I2AddrSetSocktype(addr,ai->ai_socktype), "I2AddrSetSocktype");
            ok(I2AddrSetFD(addr,fd,True), "I2AddrSetFD");

            break;
        }

        if(errno == EADDRINUSE)
            exit(77);
    }
    if (fd < 0) {
        ok(False, "bound");
        exit(1);
    }

    res = (listen(fd, 1) >= 0);
    ok(res, "listen(%d,%d): %s", fd, 1, strerror(errno));

    return addr;
}

static void server_test (I2ErrHandle eh, I2Addr listenaddr)
{
    struct sockaddr_storage sbuff;
    socklen_t sbufflen;
    int listenfd;
    int connfd;
    I2Addr clientaddr;
    I2Boolean res;
    char node[NI_MAXHOST], serv[NI_MAXSERV];
    size_t nodelen = sizeof(node);
    size_t servlen = sizeof(serv);
    I2Addr fdaddr;

    listenfd = I2AddrFD(listenaddr);
    sbufflen = sizeof(sbuff);
    connfd = accept(listenfd, (struct sockaddr *)&sbuff, &sbufflen);
    ok(connfd >= 0, "accept: %s", strerror(errno));

    clientaddr = I2AddrBySAddr(
        eh, (struct sockaddr *)&sbuff, sbufflen, SOCK_STREAM, 0);
    ok(clientaddr != NULL, "I2AddrBySAddr");

    res = I2AddrNodeName(clientaddr, node, &nodelen) != NULL;
    ok(res, "I2AddrNodeName");
    res = I2AddrServName(clientaddr, serv, &servlen) != NULL;
    ok(res, "I2AddrNodeName");

    printf("Connection from [%s]:%s\n", node, serv);

    /*
     * Try the same again using BySockFD function. Don't close on free to avoid double close of fd.
     */
    fdaddr = I2AddrBySockFD(eh, connfd, False);
    res = I2AddrNodeName(fdaddr, node, &nodelen) != NULL;
    ok(res, "I2AddrNodeName");
    res = I2AddrServName(fdaddr, serv, &servlen) != NULL;
    ok(res, "I2AddrNodeName");

    printf("Connection (fd) from [%s]:%s\n", node, serv);

    I2AddrFree(clientaddr);
}

int
main(
        int     argc    __attribute__((unused)),
        char    **argv  __attribute__((unused))
    )
{
    I2LogImmediateAttr ia;
    I2ErrHandle eh;
    I2Addr addr;
    pid_t child_pid;
    int child_status;
    I2Boolean res;

    ia.line_info = (I2NAME|I2MSG);
    ia.fp = stderr;
    eh = I2ErrOpen("addrtest", I2ErrLogImmediate,&ia, NULL, NULL);
    ok(eh != NULL, "I2ErrOpen");

    addr = server_init(eh);

    child_pid = fork();
    ok(child_pid >= 0, "fork: %s", strerror(errno));
    if (child_pid == 0) {
        I2AddrFree(addr);
        client_test(eh);
    } else {
        server_test(eh, addr);
        I2AddrFree(addr);

        /*
         * Wait for client tests to finish.
         */
        res = (waitpid(child_pid, &child_status, 0) >= 0);
        ok(res, "waitpid: %s", strerror(errno));
        ok(WEXITSTATUS(child_status) == 0, "child tests passed");
    }

    return failed_tests != 0;
}
