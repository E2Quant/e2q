/*
 * =====================================================================================
 *
 *       Filename:  DaemonProcess.cpp
 *
 *    Description:  DaemonProcess
 *
 *        Version:  1.0
 *        Created:  2024年02月01日 09时56分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (vz), vyouzhi@gmail.com
 *   Organization:  Etomc2.com
 *        LICENSE:  BSD-3-Clause license
 *
 *  Copyright (c) 2019-2022, vyouzhi
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *  * Neither the name of vyouzhi and/or the DataFrame nor the
 *  names of its contributors may be used to endorse or promote products
 *  derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL vyouzhi BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * =====================================================================================
 */
#include "Toolkit/DaemonProcess.hpp"

#include <csignal>
#include <cstdlib>
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  DProcess::init
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int DProcess::init(const char *pname)
{
    int i;
    pid_t pid;
    char id[10];
    pid = fork();
    if (pid < 0)
        return (-1);
    else if (pid) {
        //       log::bug("daemon pid error");
        _exit(0);
    }

    if (setsid() < 0) return (-1);

    signal(SIGHUP, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    pid = fork();
    if (pid < 0)
        return (-1);
    else if (pid) {
        //        log::bug("daemon pid ");
        _exit(0);
    }
    umask(0);

    // chdir("/");

    for (i = 0; i < MAXFD; i++) close(i);
    sprintf(id, "%d\n", getpid());
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    return 0;
} /* -----  end of function DProcess::init  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  DProcess::save_pid
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void DProcess::save_pid(const pid_t pid, const char *pfile)
{
    FILE *fp;
    if (pfile == NULL) return;

    if ((fp = fopen(pfile, "w")) == NULL) {
        log::bug("Could not open the pid file  for writing");
        return;
    }

    fprintf(fp, "%ld\n", (long)pid);
    if (fclose(fp) == -1) {
        log::bug("Could not close the pid file ");
        return;
    }

} /* -----  end of function DProcess::save_pid  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  DProcess::rm_pid
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void DProcess::rm_pid(const char *pfile)
{
    if (pfile == NULL) return;

    if (unlink(pfile) != 0) {
        log::bug("Could not remove the pid file ");
    }

} /* -----  end of function DProcess::rm_pid  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  DProcess::sig
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void DProcess::sig()
{
    auto handler = [](const int s) { exit(-1); };  // -----  end lambda  -----

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGQUIT, handler);
    signal(SIGKILL, handler);
    signal(SIGHUP, handler);
    signal(SIGPIPE, SIG_IGN);
} /* -----  end of function DProcess::sig  ----- */
}  // namespace e2q
