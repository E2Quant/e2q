/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2022年07月28日 11时19分33秒
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

#include <stdlib.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include <cstdlib>

#include "E2Q.hpp"
#include "Toolkit/DaemonProcess.hpp"
#include "ast/ParserCtx.hpp"

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  only_run
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void only_run(const char* f)
{
    e2::ParserCtx ctx;
    int ret = ctx.toparse(f);

    if (ret == -1) {
        return;
    }

    std::int64_t a = 19;
    CodeGenContext context;

    createCoreFunctions(context);

    Block* block = ctx.RootBlock();
    bool isgc = context.generateCode(block);

    if (isgc) {
        context.runCode();
        int ret = context.runFunction(a, a);
        log::echo("ret:", ret);
    }
    else {
        log::bug("generateCode is error");
    }

} /* -----  end of function only_run  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  e2q_action
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int e2q_action(int argc, char* argv[])
{
    char* e = nullptr;
    char* s = nullptr;
    char* p = nullptr;
    int index;

    int run = 0;  // 第几次运行进程

    std::string properties = "";
    std::vector<std::string> eas_el;
    std::string oms_el;
    rlimit rlim;
    int h;
    ssize_t do_daemonize = 0;
    e2q::DProcess process;
    char pid_file[] = "/tmp/e2q.pid";
    std::string help =
        "%s -e script.e2 \n \
                Usage: \n \
                -h help \n \
                -e which loading ea e2l script \n \
                -s which loading oms e2l script \n \
                -p which loading db properties \n \
                -l debug e2l \n \
                -r e2l run number \n \
                -o only test e2l script \n \
                -v show e2q version \n \
                -d daemon run \n";

    if (argc < 2) {
        printf(help.c_str(), argv[0]);

        exit(-1);
    }
    e2q::process_debug = false;

    //./e2q -e node.e2 -e node2.e2 -e node3.e2
    while ((h = getopt(argc, argv, "hdlve:s:p:r:o:")) != -1) {
        switch (h) {
            case 'h': {
                printf(help.c_str(), argv[0]);

                exit(-1);
            }
            case 'd':

                do_daemonize = 1;
                break;

            case 'l':
                e2q::process_debug = true;
                break;
            case 'v': {
                log::echo(version::version_full);
                exit(0);
            }
            case 'e': {
                e = optarg;
                if (e != nullptr) {
                    eas_el.push_back(std::string(e));
                }
                break;
            }
            case 's': {
                s = optarg;
                if (s != nullptr) {
                    oms_el = std::string(s);
                }
                break;
            }
            case 'p': {
                p = optarg;
                if (p != nullptr) {
                    properties = std::string(p);
                }
                break;
            }
            case 'r': {
                if (optarg != nullptr) {
                    run = atoi(optarg);
                }
                break;
            }
            case 'o': {
                e = optarg;
                if (e != nullptr) {
                    only_run(e);
                }
                return 0;
            }

            case '?':
            default:
                printf("%s -h\n", argv[0]);
                exit(-1);
        }
    }

    for (index = optind; index < argc; index++) {
        printf("Non-option argument %s\n", argv[index]);
        printf("%s -h\n", argv[0]);
        exit(-1);
    }

    if (do_daemonize == 1) {
        process.sig();
        process.init(argv[0]);

        process.save_pid(getpid(), pid_file);
    }

    if (getrlimit(RLIMIT_NOFILE, &rlim) != 0) {
        log::bug("failed to getrlimit number of files");
        exit(1);
    }
    else {
        rlim.rlim_cur = MAXCONNS;
        rlim.rlim_max = MAXCONNS;
        if (setrlimit(RLIMIT_NOFILE, &rlim) != 0) {
            log::bug(
                "failed to set rlimit for open files. Try starting as root or "
                "requesting smaller maxconns value.");
            exit(1);
        }
    }

    const auto processor_count = std::thread::hardware_concurrency();

    e2q::UtilTime ut;
    // e mathematical constant deci start
    std::size_t now = ut.time() - 718281828;

    int proce = eas_el.size();

    pid_t pids[proce];
    int m = 0;
    e2q::E2Q _e2q;
    for (m = 0; m < proce; m++) {
        pids[m] = fork();
        switch (pids[m]) {
            // PID == -1 代表 fork 出錯
            case -1:
                perror("fork()");
                exit(-1);

            // PID == 0 代表是子程序
            case 0: {
                printf("[Child] I'm Child process\n");
                printf("[Child] Child's PID is %d name: %s \n", getpid(),
                       eas_el[m].c_str());
                if (eas_el.size() != 0) {
                    _e2q.setCfg(eas_el[m], properties);
                    _e2q.trader(m, now, run);
                }
                return 0;
            }
            // PID > 0 代表是父程序
            default:
                // printf("{parent} Parent PID is %d\n", getpid());
                break;
        }
        now += processor_count;
    }

    if (oms_el.size() != 0) {
        _e2q.setCfg(oms_el, properties);
        _e2q.exchange();
    }
    int exit_status;
    pid_t pid;
    for (m = 0; m < proce; m++) {
        pid = wait(&exit_status);
        log::echo("child pid ", pid, " status 0x%x", (long)pid, exit_status);
    }
    if (do_daemonize == 1) {
        process.rm_pid(pid_file);
    }
    return 0;

} /* -----  end of function e2q_action  ----- */
/*
 * ===  FUNCTION  =============================
 *         Name:  main
 *  Description:
 * ============================================
 */
int main(int argc, char* argv[])
{
    e2q_action(argc, argv);
    return EXIT_SUCCESS;
} /* ----------  end of function main  ---------- */
