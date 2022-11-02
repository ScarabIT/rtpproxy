/*
 * Copyright (c) 2023 Sippy Software, Inc., http://www.sippysoft.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <string.h>

#include "rtpp_module.h"
#include "rtpp_module_if_static.h"

extern struct rtpp_minfo rtpp_module_acct_csv;
extern struct rtpp_minfo rtpp_module_acct_rtcp_hep;
extern struct rtpp_minfo rtpp_module_catch_dtmf;
extern struct rtpp_minfo rtpp_module_dtls_gw;

const struct rtpp_modules rtpp_modules = {
    .acct_csv = &rtpp_module_acct_csv,
    .acct_rtcp_hep = &rtpp_module_acct_rtcp_hep,
    .catch_dtmf = &rtpp_module_catch_dtmf,
    .dtls_gw = &rtpp_module_dtls_gw,
};

struct rtpp_minfo *
rtpp_static_modules_lookup(const char *name)
{

    for (int i = 0; rtpp_modules.all[i] != NULL; i++) {
        if (strcmp(rtpp_modules.all[i]->descr.name, name) == 0) {
             return (rtpp_modules.all[i]);
        }
    }
    return (NULL);
}
