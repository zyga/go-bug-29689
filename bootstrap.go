// -*- Mode: Go; indent-tabs-mode: t -*-

package main

/*

#include <stdlib.h>
#include "bootstrap.h"

// The bootstrap function is called by the loader before passing
// control to main. We are using `preinit_array` rather than
// `init_array` because the Go linker adds its own initialisation
// function to `init_array`, and having ours run second would defeat
// the purpose of the C bootstrap code.
//
// The `used` attribute ensures that the compiler doesn't optimise out
// the variable on the mistaken belief that it isn't used.
__attribute__((section(".preinit_array"), used)) static typeof(&bootstrap) init = &bootstrap;

// NOTE: do not add anything before the following `import "C"'
*/
import "C"

import (
	"fmt"
	"syscall"
)

// BootstrapError returns error (if any) encountered in pre-main C code.
func BootstrapError() error {
	if C.bootstrap_msg == nil {
		return nil
	}
	errno := syscall.Errno(C.bootstrap_errno)
	if errno != 0 {
		return fmt.Errorf("%s: %s", C.GoString(C.bootstrap_msg), errno)
	}
	return fmt.Errorf("%s", C.GoString(C.bootstrap_msg))
}
